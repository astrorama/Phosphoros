#
# Copyright (C) 2012-2022 Euclid Science Ground Segment
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

from __future__ import print_function
from os.path import exists , join
import argparse
import re
import sys
from astropy.table import Table
from astropy.table import join as tablejoin
import numpy as np

import _DisplayModelGrid as dmg
from ElementsKernel import Logging
import matplotlib.pyplot as plt
from configparser import ConfigParser


logger = Logging.getLogger('PlotFluxDifference')

def defineSpecificProgramOptions():
    """
    @brief Allows to define the (command line and configuration file) options
    specific to this program

    @details
        See the Elements documentation for more details.
    @return
        An  ArgumentParser.
    """
    description = """
            Get a folder containing the result of a ComputRedshift run from the GUI. 
            Compare the Sources photometry with the (scaled) Best Fit Model. 
            
            The code assume Phosphoros files location. 
    """
    
    parser = argparse.ArgumentParser(description=description)

    parser.add_argument("-rd", "--result-dir", type=str, required=True,
                        help="The folder containing the outputs and config of ComputeRedshift Run")
    parser.add_argument("-ip", "--intermediate-product-dir", type=str, required=True,
                        help="The folder containing Phosphoros Intermedaie Product")
    
    parser.add_argument("-zl", "--z-limit", type=float, default=0.95,
                        help="Max redshift limit as a fraction of the available data (default=0.95)")
    
    parser.add_argument("-vl", "--vertical-sigma-limit", type=float, default=3,
                        help="Y-Axis limit in multiple of the sigma of the data (default=3.0)")
    
    parser.add_argument("-sm", "--sliding-mean-sampling", type=int, default=10,
                        help="The number of points the sliding mean is computed (default=10)")

    return parser

def running_med(off, z, zmax, dz):
    z_arr = np.linspace(0., zmax, int(zmax/(dz/3.)))
    off_arr = np.zeros(len(z_arr))
    for i in range(len(z_arr)):
        off_arr[i] = np.median(off[(z>=z_arr[i]-dz/2.) & (z<=z_arr[i]+dz/2.)])
    return z_arr, off_arr
    
def get_run_config(conf_file):
    parser = ConfigParser()
    with open(conf_file) as stream:
        parser.read_string("[top]\n" + stream.read())
    return parser['top']
    
def get_conf_value(configs, keyword): 
    if not keyword in configs:
        logger.error('run config file do not contains the '+keyword+' keyword')
        sys.exit(1)
    else :
        return configs[keyword]
    
def read_config_file(folder_name): 
    config_file = join(folder_name,'run_config.config')
    if not exists(config_file):
        logger.error('Provided folder do not contains a config file  ('+str(config_file)+')')
        sys.exit(1) 
        
    configs = get_run_config(config_file)
        
    catalog = get_conf_value(configs, 'catalog-type')
    grid = get_conf_value(configs, 'model-grid-file')
    input_file = get_conf_value(configs, 'input-catalog-file')
    phosphoros_root = get_conf_value(configs, 'phosphoros-root')
    source_id = get_conf_value(configs, 'source-id-column-name')
     
    return catalog, grid, input_file, phosphoros_root, source_id

def read_result_file(folder_name):
    if  exists(join(folder_name, 'phz_cat.fits')):
        results = Table.read(join(folder_name, 'phz_cat.fits'))
    elif exists(join(folder_name,'phz_cat.txt')):
        results = Table.read(join(folder_name,'phz_cat.txt'), format='ascii')
        
        nb_col = len(results.colnames)
        with open(join(folder_name,'phz_cat.txt')) as f:
            for col_id in range(nb_col):
                col_search=False
                while not col_search :
                     line = f.readline()
                     col_search = re.search('# Column:\s*(\S*)',str(line))
                results['col'+str(col_id+1)].name=col_search.group(1).strip()
        
    else:
        logger.error('result file not found in folder ('+folder_name+')')
        sys.exit(1)
       
    if not 'SED-Index' in results.colnames:
        logger.error('The Best fitted model is missing into the result file')
        sys.exit(1)
    
    if not 'Scale' in results.colnames:
        logger.error('The Scale of the Best fitted model is missing into the result file')
        sys.exit(1)
    
    return results

def read_flux_catalog(phosphoros_root, intermediate_product_dir, catalog, input_file):
    if not input_file.startswith('/'):
        input_file = join(phosphoros_root, 'Catalogs', catalog, input_file)
    if  exists(input_file):
        input_table = Table.read(input_file)
    else:
        logger.error('Input Flux file ('+input_file+') not found')
        sys.exit(1) 
    
    filter_mapping_file =  join(intermediate_product_dir, catalog, 'filter_mapping.txt')
    if  exists(filter_mapping_file):
        filter_mapping = Table.read(filter_mapping_file, format='ascii')
        filter_mapping['col1'].name='Filter'
        filter_mapping['col2'].name='Flux'
        filter_mapping['col3'].name='Err'
    else:
        logger.error('Filter mapping file ('+filter_mapping_file+') not found')
        sys.exit(1)
    
    for row_index in range(len(filter_mapping)):
        input_table[filter_mapping[row_index]['Flux']].name = filter_mapping[row_index]['Filter']+'_Measured'
        input_table.remove_column(filter_mapping[row_index]['Err'])
    return input_table


def plot_data(data_to_plot, band_column, z_limit, vertical_sigma_limit, sliding_mean_sampling):
    # Compute redshift limits 
    z_min=np.min(data_to_plot['Z'])
    z_max=np.max(data_to_plot['Z'])
    ordered_z = np.sort(np.copy(data_to_plot['Z']))
    
    limit_z = ordered_z[int(z_limit*(len(ordered_z)-1))]
    
    for band_index in range(len(band_column)):
        offset = (data_to_plot[band_column[band_index]+'_Measured'] - data_to_plot[band_column[band_index]+'_Model']) / data_to_plot[band_column[band_index]+'_Model']
        offset = np.nan_to_num(offset, nan=0.0, posinf=0.0, neginf=0.0)
        average = np.mean(offset)
        sigma = np.sqrt(np.var(offset))
        z_arr, off = running_med(offset, data_to_plot['Z'], limit_z, limit_z/sliding_mean_sampling)

        fig = plt.figure(band_index, figsize=[9,5],dpi=100)
        fig.subplots_adjust(wspace=0.18, hspace=0.0,left=0.09,bottom=0.15,right=0.98,top=0.98)
        ax = fig.add_subplot(111)
        ax.plot(data_to_plot['Z'], offset, '.', alpha=0.1)
        ax.plot([z_min,z_max],[0,0],'k-')
        ax.plot([z_min,z_max],[average,average],'k--')
        ax.plot(z_arr, off, color='r')
        
        ax.set_xlim(0, limit_z)
        ax.set_ylim(average -vertical_sigma_limit*sigma, average +vertical_sigma_limit*sigma)
        
        ax.text(0.4*limit_z, average +0.75*vertical_sigma_limit*sigma, r'{}-band'.format(band_column[band_index]), fontsize=18, bbox=dict(boxstyle='round',facecolor='white'))
       
        ax.set_xlabel(r'z', fontsize=18) 
        ax.set_ylabel(r'(Flux - Flux$_{Mod}$) / Flux$_{Mod}$', fontsize=18)
        
    plt.show()

def mainMethod(args):
    """
    @brief The "main" method.
    @details
        This method is the entry point to the program. In this sense, it is
        similar to a main (and it is why it is called mainMethod()).
    """

    # 1) Get the config file, extract the grid name, the catalog type and the input file
    folder_name = args.result_dir
    catalog, grid, input_file, phosphoros_root, source_id = read_config_file(folder_name)

    # 2) read the result file, check for availability of best fitted model and scaling
    results = read_result_file(folder_name)
    
    # 3) read the input flux file, filter mapping and convert the flux column name 
    input_table = read_flux_catalog(phosphoros_root, args.intermediate_product_dir, catalog, input_file)

    # 4) Read the grid
    grid_data = dmg.readModelGrid(catalog, args.intermediate_product_dir, grid)
    grid_columns =  dmg.getModelGridColumns(catalog, args.intermediate_product_dir, grid)
    dtypes = ['int32', 'int32', 'int32', 'int32', 'int32']
    for index in range(len(grid_columns)-5):
        dtypes.append('float64')
    grid_table = Table(rows=grid_data.T, names=grid_columns, dtype=dtypes)

    # 5) lookup the model flux in the grid, scale them, merge with flux 
    merged = tablejoin(results, grid_table, keys=('region-Index', 'SED-Index', 'ReddeningCurve-Index', 'E(B-V)-Index', 'Z-Index'))
    band_column = grid_columns[5:]
    for column in band_column:
        merged[column]= merged['Scale']* merged[column]
        merged[column].name = column+'_Model' 
    merged['ID'].name = source_id
    data_to_plot = tablejoin(merged, input_table, keys=(source_id))
    
    # plot the data
    plot_data(data_to_plot, band_column, args.z_limit,  args.vertical_sigma_limit,  args.sliding_mean_sampling)  
        