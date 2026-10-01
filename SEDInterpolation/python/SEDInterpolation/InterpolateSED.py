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
import copy
import astropy.table as table
import numpy as np
import shutil
import ElementsKernel.Logging as log
from XYDatasetSet import XYDatasetSetTools

logger = log.getLogger('InterpolateSED')


def defineSpecificProgramOptions():
    """ Add the arguments to the arg parser
    
    Returns:
    The arg parser
    """

    parser = argparse.ArgumentParser()
    parser.add_argument('--sed-dir', required=True, type=str, metavar='DIR',
                        help='The directory containing the SEDs')
    parser.add_argument('--filter-dir', required=False, type=str,
                        help='The directory containing the Filters')
    parser.add_argument('--seds', type=str,  required=True,
                        help='List of comma separated SEDs files (relative to sed-dir), at least 2 SED must be provided')
    parser.add_argument('--numbers', type=str,  required=True,
                        help='List of comma separated non-negative integer indicating the number of SED to be computed between each input SEDs. The number of integer must be one less that the number of SED')
    parser.add_argument('--out-path', type=str,  required=True,
                        help='Folder (relative to sed-dir) or .fits name (depending on the "out-format") into which SEDs will be saved. If the folder/file exists it will be cleared/overwrited.')  
                        
    parser.add_argument('--out-format', default="individual", type=str,
                        help='Select between individual SED files ("individual") or in a set .fits file ("set"). Default is "individual"')
                        
    parser.add_argument('--normalization-filter', type=str,  required=True,  
                         help='(Qualified) Name of the Filter for which the normalization is done for Luminosity computation') 
    parser.add_argument('--normalization-solar-sed', type=str,  required=True,  
                         help='(Qualified) Name of the Solar SED @10pc used as a reference for Models normalization') 
                    
                                         
    parser.add_argument('--copy-sed', default="True", type=str,
                        help='If true copy the original SEDs into the output folder (True /False Default: True)' )
                        
    parser.add_argument('--interpolate-pp', default="True", type=str,
                        help='If true interpolate also the (common) physical parameter(s) found in SEDs headers (True /False Default: True)' )


    return parser
    
def compute_flux(sed, filter_transmission):
    new_sampling = get_sampling(sed, filter_transmission)
    resampled_sed = resample(sed, new_sampling)
    resampled_filter = resample(filter_transmission, new_sampling)
    x = resampled_sed.x
    y = np.nan_to_num(resampled_sed.y*resampled_filter.y)
    
    print(x)
    print(y)
    return np.trapz(y, x);
        
def do_normalise_sed(sed, current_norm, target_norm):
    sed.y = sed.y*target_norm/current_norm
    return sed
    
def normaliseSED(sed, solar_sed, filter_transmission):
    solar_flux = compute_flux(solar_sed, filter_transmission)
    sed_flux = compute_flux(sed, filter_transmission)
    print(f'Normalization of a SED with current norm {sed_flux} to target norm {solar_flux}')
    return do_normalise_sed(sed, sed_flux, solar_flux)

def getSedDir(sed_dir):
    """Check the SED directory exists
    
    Parameters:
    sed_dir (str): The path of the SED directory
    
    Returns:
    str:Validate SED directory
    """
    if os.path.exists(sed_dir):
        if not os.path.isdir(sed_dir):
            logger.error(sed_dir + ' is not a directory')
            exit(1)
        return sed_dir
    logger.error('Unknown SEDs directory ' + sed_dir)
    exit(1)
    
def getFilterDir(filter_dir):
    if os.path.exists(filter_dir):
        if not os.path.isdir(filter_dir):
            logger.error(filter_dir + ' is not a directory')
            exit(1)
        return filter_dir
    logger.error('Unknown Filters directory ' + filter_dir)
    exit(1)
    
def prepareOutFolder(out_dir):
    """ Create or clear the output directory
    
    Parameters:
    out_dir (str): The path of the output directory
    """
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


def get_sampling(sed_1, sed_2):
    """ Compute the common sampling (keep the existing sampling for the part non 
    overlapping and the sampling with the highest number of knots for the overlaping part)
    
    Returns:
    before (list): sampling bellow the common part
    common_part (list): sampling of the overlaping part
    after (list)]: sampling above the common part
    """
    sample_1 = sed_1.x
    sample_2 = sed_2.x
    
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


def resample(sed, sampling):
    """ Resample the SED according to the new set of sample
    
    Parameters: 
    sed: XYDataset
    sampling ([before (list),common_part (list),after (list)]: new sampling 
    
    Returns:
    XYDataset: table containg the resampled SED
    """
    before = np.zeros(len(sampling[0]))
    start = 0
    if len(sampling[0])>0 and sed.x[0] == sampling[0][0]:
        before = sed.y[0:len(sampling[0])]
        start = len(sampling[0])
        
    after =  np.zeros(len(sampling[2]))
    end = len(sed.x)
    
    if len(sampling[2]) >0 and sed.x[-1] == sampling[2][-1]:
        after = sed.y[-len(sampling[2]):]
        end = len(sed.x)-len(sampling[2])
  
    common_current_sampling = sed.x[start:end]
    common_current_values =  sed.y[start:end]
    
    common_new = np.interp(sampling[1], common_current_sampling, common_current_values)
    
    total_sampling = np.concatenate((sampling[0], sampling[1], sampling[2]), axis=None)
    total_values = np.concatenate((before,common_new, after), axis=None)
    
    sed.x = total_sampling
    sed.y = total_values
    return sed
  
def parse_pp(pp):
    """ Parse the string encoding the Physical parameters
    
    Parameters: 
    pp (str): Input string of the form A*L0+B[UNIT]
    
    Returns:
    (str,float, float, str): name, A, B, unit
    
    """
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
    return A, B, unit

def format_pp(A, B, unit):
    """ Convert the PP into the normalized string used to store it
    
    Parameters:
    A (float): Term proportional to the luminosity
    B (float): constant term
    unit (str): PP unit
    
    Returns:
    str: the formated string
    """
    return str(A)+"*L+"+str(B)+"["+unit+"]"
    
    
def do_interpolate_pp(pp_1, pp_2, idx, total):
    """Interpolate the common PP (common mean same name and same unit)
    
    Parameters:
    pp_1  PP of the first SED
    pp_2  PP of the second SED
    idx (int): Index of the interpolated SEDs
    total(int): total number of SED to be created between the 2 existing SEDs 
    
    Returns:
    list(str): the list of interpolates PP
    """
    # pp_i is a dict of "<Name>:<number1 = A>*L+<number2 = B>[<unit>]"
    pp1_dict = {}
    for pp in pp_1:
        A, B, unit = parse_pp(pp_1[pp])
        pp1_dict[pp]={"A":A, "B":B, "unit": unit}
        
    pp2_dict = {}
    for pp in pp_2:
        A, B, unit = parse_pp(pp_2[pp])
        pp2_dict[pp]={"A":A, "B":B, "unit": unit}
         
    compatible_pp={}
    for name_1 in pp1_dict:
        unit_1 = pp1_dict[name_1]["unit"]
        if name_1 in pp2_dict and pp2_dict[name_1]["unit"] == unit_1:
            compatible_pp[name_1] = [pp1_dict[name_1], pp2_dict[name_1]]
    
    logger.info('Found '+str(len(compatible_pp)) + ' PP compatible')
   
    
    frac_1 = (total - idx)/(total+1.0)
    frac_2 = (idx+1)/(total+1.0)

    new_pp = {}
    for pp in compatible_pp:
        new_A = frac_1*compatible_pp[pp][0]['A'] + frac_2*compatible_pp[pp][1]['A']
        new_B = frac_1*compatible_pp[pp][0]['B'] + frac_2*compatible_pp[pp][1]['B']
        new_unit = compatible_pp[pp][0]['unit']
        new_pp[pp] = format_pp(new_A, new_B, new_unit)
    
    return new_pp
    

def do_interpolate_sed(sed_1, sed_2, idx, total):
    """Interpolate the SED between the 2 provided SEDs (which must have the same sampling)
    """
    frac_1 = (total - idx)/(total+1.0)
    frac_2 = (idx+1)/(total+1.0)
    
    values = frac_1*sed_1.y + frac_2*sed_2.y

    return values

 
def clean_name(name):
    """ Extract the SED short name from the file name
    
    Parameters:
    name(str): The name of the file containing the SED
    
    Returns:
    str: the SED short name 
    """
    name = clean_name_folder(name)
    if '.' in name:
        name = '.'.join(name.split(".")[:-1])

    return name
        
def clean_name_folder(name):
    """ Extract the file name from the path
    
    Parameters:
    name(str): The name of the file containing the SED
    
    Returns:
    str: the SED file name
    """
    if '/' in name:
        name = name.split("/")[-1]
   
    return name
         
    
def build_name(name_1, name_2, idx, total):
    """Create the name for the interpolated SED from the names of the SED and the interpolation index
    
    Parameters:
    name_1 (str): First SED short name
    name_2 (str): Second SED short name
    idx (int): Index of the interpolated SEDs
    total(int): total number of SED to be created between the 2 existing SEDs 
    
    Returns:
    str: the name of the interpolated SED
    """
    number_1 = str(total - idx)+":"+str(total+1)
    number_2 = str(idx+1)+":"+str(total+1)
    
    return number_1 + "_" + clean_name(name_1) + "_+_" + number_2 + "_" + clean_name(name_2)

def interpolate(sed_list, sed_number, interpolate_pp, solar_sed, normalisation_filter, copy_seds) :
    output_sed_list = []
    for index in range(len(sed_number)):
        if copy_seds:
            output_sed_list.append(sed_list[index]) 
        logger.info('Interpolation between SED %s and %s', sed_list[index].name,  sed_list[index+1].name )  
        sed_1= copy.deepcopy(sed_list[index])
        sed_2= copy.deepcopy(sed_list[index+1])
        
        # Get the new sampling
        new_sampling = get_sampling(sed_1, sed_2)
 
        # re-sample if needed
        resampled_sed_1 = resample(sed_1, new_sampling)
        resampled_sed_2 = resample(sed_2, new_sampling)
        
        # normalize the SEDs
        sed_1 = normaliseSED(sed_1, solar_sed, normalisation_filter)
        sed_2 = normaliseSED(sed_2, solar_sed, normalisation_filter)
        
        new_sampling_array =  sed_1.x

        interpolate_num  = sed_number[index]
        for idx in range(interpolate_num):
            name_i = build_name(sed_1.name, sed_2.name, idx, interpolate_num)
            values_i = do_interpolate_sed(sed_1, sed_2, idx, interpolate_num)
            interpolated_sed_i = XYDatasetSetTools.XYDataset(new_sampling_array, values_i, name_i,{})
            if interpolate_pp:
                pp_1 = sed_1.listParam()
                pp_2 = sed_2.listParam()
                new_pp = do_interpolate_pp(pp_1, pp_2, idx, interpolate_num)
                for pp in new_pp:
                    interpolated_sed_i.addParam(pp, new_pp[pp])
            output_sed_list.append(interpolated_sed_i)
    if copy_seds:
        output_sed_list.append(sed_list[-1])
    return output_sed_list

def mainMethod(args):
    sed_dir = getSedDir(args.sed_dir)
    if sed_dir=="":
        raise ValueError("sed_dir must be provided")
        
    logger.info('Listing available SEDs')
    available_seds = XYDatasetSetTools.listDataset(sed_dir)
    
    filter_dir = getFilterDir(args.filter_dir)
    if filter_dir=="":
        raise ValueError("filter-dir must be provided")
    logger.info('Listing available Filters')
    available_filters = XYDatasetSetTools.listDataset(filter_dir)


    sed_list = args.seds.split(',')
    sed_number = len(sed_list)
    if sed_number<2:
        raise ValueError("At least 2 SEDs must be provided")
    interp_number = [int(bite) for bite in args.numbers.split(',')]    
    if len(interp_number)!=sed_number-1:
        raise ValueError("numbers must have one elements less than seds")
    
    logger.info('Reading the SEDs')
    sed_data = []
    for sed_name in sed_list:
        if sed_name in available_seds:
            sed_data.append(XYDatasetSetTools.readDataset(sed_dir,sed_name, available_seds))
        else:
            raise ValueError(f"Sed {sed_name} is not available in folder {sed_dir} ")
           
    if not args.normalization_solar_sed in available_seds:  
        raise ValueError(f"Unable to find the Solar SED {args.normalization_solar_sed} in {sed_dir}")
    logger.info('Reading the Solar SEDs')   
    solar_sed =  XYDatasetSetTools.readDataset(sed_dir, args.normalization_solar_sed, available_seds)
    
    if not args.normalization_filter in available_filters:  
        raise ValueError(f"Unable to find the Filter {args.normalization_filter} in {filter_dir}")
    logger.info('Reading the Filter')
    normalisation_filter =  XYDatasetSetTools.readDataset(filter_dir,args.normalization_filter, available_filters)
    
    
    logger.info('Interpolating')
    out_seds = interpolate(sed_data, interp_number, args.interpolate_pp.lower() == "true", solar_sed, normalisation_filter, args.copy_sed.lower() == "true")  

    # Write on disk
    if args.out_format=="individual":
        out_dir = args.out_path
        if out_dir=="":
            raise ValueError("out_path must be provided")
        out_dir = os.path.join(sed_dir, out_dir)
        prepareOutFolder(out_dir)
    
        all_names = [sed.name.split('/')[-1] for sed in out_seds]
        if len(all_names)!=len(np.unique(all_names)):
            raise Exception("Multiple SEDs with the same name cannot be saved in a single folder.")
        file_names = []
        for sed in out_seds:
            file_names.append(XYDatasetSetTools.writeDataset(sed, out_dir))
            
        logger.info('Add the order file')
        with open(out_dir+'/order.txt', 'w') as fh:
            for name in file_names:
                fh.write(f'{name.split('/')[-1]}\n') 
    else:
        out_file = args.out_path
        if out_file=="" or out_file.split('.')[-1]!='fits':
            raise ValueError("out_path must be provided and be a file name with a .fits extension")
        out_seds = XYDatasetSetTools.checkSampling(out_seds, True)
        XYDatasetSetTools.writeDatasetSet(out_seds, os.path.join(sed_dir, out_file))
        logger.info(f'write {out_file}')    
