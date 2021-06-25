#  
# Copyright (C) 2012-2020 Euclid Science Ground Segment
#   
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free 
# Software Foundation; either version 3.0 of the License, or (at your option)  
# any later version.  
#  
# This library is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
# details.  
#   
# You should have received a copy of the GNU Lesser General Public License 
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
#  
  
  
"""  
File: python/SEDInterpolation/InterpolateSED.py

Created on: 23/06/2021
Author: dubathf
"""

from __future__ import division, print_function

import argparse
import os
import astropy.table as table
import numpy as np
import shutil
import ElementsKernel.Logging as log
from EmissionLines import SedUtils

logger = log.getLogger('InterpolateSED')


def defineSpecificProgramOptions():
    

    parser = argparse.ArgumentParser()
    parser.add_argument('--sed-dir', required=True, type=str, metavar='DIR',
                        help='The directory containing the SEDs')
    parser.add_argument('--seds', type=str,  required=True,
                        help='List of comma separated SEDs files (relative to sed-dir), at least 2 SED must be provided')
    parser.add_argument('--numbers', type=str,  required=True,
                        help='List of comma separated non-negative integer indicating the number of SED to be computed between each input SEDs. The number of integer must be one less that the number of SED')
    parser.add_argument('--out-dir', type=str,  required=True,
                        help='Folder (relative to sed-dir) into which SEDs will be saved. If the folder exists it will be cleared.')                    
                                         
    parser.add_argument('--copy-sed', default=True, type=bool,
                        help='If true copy the original SEDs into the output folder (Default: True)' )
                        
    parser.add_argument('--interpolate-pp', default=True, type=bool,
                        help='If true interpolate also the (common) physical parameter(s) found in SEDs headers (Default: True)' )


    return parser

def getSedDir(sed_dir):
    if os.path.exists(sed_dir):
        if not os.path.isdir(sed_dir):
            logger.error(sed_dir + ' is not a directory')
            exit(1)
        return sed_dir
    if not os.path.isabs(sed_dir) and not os.path.exists(sed_dir):
        path_in_phos_sed = os.path.join(phos_dir, 'AuxiliaryData', 'SEDs', sed_dir)
        if os.path.isdir(path_in_phos_sed):
            return path_in_phos_sed
    logger.error('Unknown SED directory ' + sed_dir)
    exit(1)
    
def prepareOutFolder(out_dir):
    if not os.path.exists(out_dir):
        os.makedirs(out_dir) 
        logger.info('Output directory %s created', out_dir)   
    else:
        logger.info('Output directory %s exists, cleaning it', out_dir)
        for filename in os.listdir(out_dir):
            file_path = os.path.join(out_dir, filename)
            try:
                if os.path.isfile(file_path) or os.path.islink(file_path):
                    os.unlink(file_path)
                elif os.path.isdir(file_path):
                    shutil.rmtree(file_path)
            except Exception as e:
                logger.info('Failed to delete %s. Reason: %s' % (file_path, e))
        
def copy_seds(out_dir, sed_dir, seds):
    logger.info('Copy original SEDs into the output folder')  
    for sed in seds:
        in_sed_path = os.path.join(sed_dir, sed)
        if '/' in sed:
            sed = sed.split("/")[-1]
        out_sed_path = os.path.join(out_dir, sed)
        shutil.copyfile(in_sed_path, out_sed_path)


def get_sampling(table_1, table_2):
    sample_1 = np.array(table_1.columns[0])
    sample_2 = np.array(table_2.columns[0])
    
    common_start = max(sample_1[0],sample_2[0])
    common_end = min(sample_1[-1],sample_2[-1])
    if sample_1[0]>sample_2[0]:
        swap = sample_1
        sample_1 = sample_2
        sample_2 = swap
    
    common_1 = sample_1[sample_1>=common_start] 
    common_1 = common_1[common_1<=common_end]
    common_2 = sample_2[sample_2>=common_start] 
    common_2 = common_2[common_2<=common_end]
    
    common_part = common_1
    if len(common_2)>len(common_1):
        common_part = common_2
    
    before = sample_1[sample_1<common_start]
    
    after =  sample_1[sample_1>common_end]
    if sample_1[-1]<sample_2[-1]:
           after =  sample_2[sample_2>common_end]
           
    return [before, common_part, after]


def resample(table_1, sampling):
    before = np.zeros(len(sampling[0]))
    start = 0
    if len(sampling[0])>0 and table_1.columns[0][0] == sampling[0][0]:
        before = table_1.columns[1][0:len(sampling[0])]
        start = len(sampling[0])
    
    after =  np.zeros(len(sampling[2]))
    end = len(table_1)
    if len(sampling[2]) >0 and table_1.columns[0][-1] == sampling[2][-1]:
        after = table_1.columns[1][-len(sampling[2]):]
        end = len(table_1)-len(sampling[2])
  
    common_current_sampling =  table_1.columns[0][start:end]
    common_current_values =  table_1.columns[1][start:end]
    
    common_new = np.interp(sampling[1], common_current_sampling, common_current_values)
    
    total_sampling = np.concatenate((sampling[0], sampling[1], sampling[2]), axis=None)
    total_values = np.concatenate((before,common_new, after), axis=None)
    
    t=table.Table()
    t['Wave']=total_sampling
    t['Flux']=total_values
    return t
  
def parse_pp(pp):
    name = (pp.split("=")[0]).strip()
    pp = pp.split("=")[1]
    unit = ""
    u_bits = pp.split("[")
    if len(u_bits)==2:
       unit = (u_bits[1].split("]")[0]).strip()
       pp = u_bits[0]

    A=0.0
    B=0.0
    num_bits = pp.split("+")
    for bit in num_bits:
        if "*L" in bit:
          A = float(bit.replace("*L",""))
        else:
          B=float(bit)
    return name, A, B, unit

def format_pp(name, A, B, unit):
    return name + "="+str(A)+"*L+"+str(B)+"["+unit+"]"
    
    
def do_interpolate_pp(pp_1, pp_2, idx, total):
    # pp_i is a list of "<Name>=<number1 = A>*L+<number2 = B>[<unit>]"
    pp1_dict = {}
    for pp in pp_1:
        name, A, B, unit = parse_pp(pp)
        pp1_dict[name]={"A":A, "B":B, "unit": unit}
        
    pp2_dict = {}
    for pp in pp_2:
        name, A, B, unit = parse_pp(pp)
        pp2_dict[name]={"A":A, "B":B, "unit": unit}
         
    compatible_pp={}
    for name_1 in pp1_dict:
        unit_1 = pp1_dict[name_1]["unit"]
        if name_1 in pp2_dict and pp2_dict[name_1]["unit"] == unit_1:
            compatible_pp[name_1] = [pp1_dict[name_1], pp2_dict[name_1]]
    
    logger.info('Found '+str(len(compatible_pp)) + ' PP compatible')
   
    
    frac_1 = (total - idx)/(total+1.0)
    frac_2 = (idx+1)/(total+1.0)
    
    
    new_pp = []
    for pp in compatible_pp:
        new_A = frac_1*compatible_pp[pp][0]['A'] + frac_2*compatible_pp[pp][1]['A']
        new_B = frac_1*compatible_pp[pp][0]['B'] + frac_2*compatible_pp[pp][1]['B']
        new_unit = compatible_pp[pp][0]['unit']
        new_pp.append(format_pp(pp, new_A, new_B, new_unit))
    
    return new_pp
    

def do_interpolate_sed(table_1, table_2, idx, total):
    frac_1 = (total - idx)/(total+1.0)
    frac_2 = (idx+1)/(total+1.0)
    
    sampling = table_1['Wave']
    
    values = frac_1*table_1['Flux'] + frac_2*table_2['Flux'] 
    
    t=table.Table()
    t['Wave']=sampling
    t['Flux']=values
    return t

 
def clean_name(name):
    if '/' in name:
        name = name.split("/")[-1]
    if '.' in name:
        return '.'.join(name.split(".")[:-1])
    else:
        return name
         
    
def build_name(name_1, name_2, idx, total):
    number_1 = str(total - idx)+":"+str(total+1)
    number_2 = str(idx+1)+":"+str(total+1)
    
    return number_1 + "_" + clean_name(name_1) + "_+_" + number_2 + "_" + clean_name(name_2)+".sed"

def interpolate(sed_dir, sed_list, sed_number, interpolate_pp, out_dir) :
    for index in range(len(sed_number)):
        logger.info('Interpolation between SED %s and %s', sed_list[index],  sed_list[index+1] )  
        path_sed_1 = os.path.join(sed_dir, sed_list[index])   
        path_sed_2 = os.path.join(sed_dir, sed_list[index + 1])  
        interpolate_num  = sed_number[index]
        
        sed_1_table = table.Table.read(path_sed_1, format='ascii')
        sed_2_table = table.Table.read(path_sed_2, format='ascii')
        
        pp_1 = []
        pp_2 = []
        if interpolate_pp:
            keyword_1 = SedUtils.readXYDatasetKeyword(path_sed_1)
            if "PARAMETER" in keyword_1:
                pp_1 = keyword_1["PARAMETER"]   
                
            keyword_2 = SedUtils.readXYDatasetKeyword(path_sed_2)
            if "PARAMETER" in keyword_2:
                pp_2 = keyword_2["PARAMETER"]
        
        # Get the new sampling
        new_sampling = get_sampling(sed_1_table, sed_2_table)
     
        # re-sample if needed
        resampled_sed_1 = resample(sed_1_table, new_sampling)
        resampled_sed_2 = resample(sed_2_table, new_sampling)
        
        for idx in range(interpolate_num):
            pp_i = []
            if interpolate_pp:
                pp_i = do_interpolate_pp(pp_1, pp_2, idx, interpolate_num)  
            table_i = do_interpolate_sed(resampled_sed_1, resampled_sed_2, idx, interpolate_num)
       
            name_i = build_name(sed_list[index], sed_list[index + 1], idx, interpolate_num)
            
            path_i = os.path.join(out_dir, name_i)
            
            logger.info('Writing the file %s', path_i )  
            table_i.write(path_i, format='ascii.commented_header')
             
            if len(pp_i)>0:
                SedUtils.replaceXYDatasetKeyword(path_i, {'PARAMETER' : pp_i})
        

def mainMethod(args):
    sed_dir = getSedDir(args.sed_dir)
    if sed_dir=="":
        raise ValueError("sed_dir must be provided")
    out_dir = args.out_dir
    if out_dir=="":
        raise ValueError("out_dir must be provided")
    out_dir = os.path.join(sed_dir, out_dir)
    
    sed_list = args.seds.split(',')
    sed_number = len(sed_list)
    if sed_number<2:
        raise ValueError("At least 2 SEDs must be provided")
    interp_number = [int(bite) for bite in args.numbers.split(',')]    
    if len(interp_number)!=sed_number-1:
        raise ValueError("numbers must have one elements less than seds")
    
    prepareOutFolder(out_dir)
    
    if args.copy_sed:
        copy_seds(out_dir, sed_dir, sed_list) 
        
    interpolate(sed_dir, sed_list, interp_number, args.interpolate_pp, out_dir)             
  

