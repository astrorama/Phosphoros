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
from os.path import exists
import argparse
import re
from astropy.table import Table

import _DisplayModelGrid as dmg
from ElementsKernel import Logging

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

    return parser

def mainMethod(args):
    """
    @brief The "main" method.
    @details
        This method is the entry point to the program. In this sense, it is
        similar to a main (and it is why it is called mainMethod()).
    """

    # 1) Get the config file, extract the grid name and the catalog type
    folder_name = args.result_dir
    if not exists(folder_name+'/run_config.config'):
        logger.error('Provided folder do not contains a config file  ('+folder_name+'/run_config.config)')
        return 1 
    grid='' # model-grid-file
    catalog ='' # catalog-type
    contents=''
    with open(folder_name+'/run_config.config') as f:
        contents = f.readlines()
     
    grid_search = re.search('\s*model-grid-file\s*=(.*)', "\n".join(contents))
    catalog_search = re.search('\s*catalog-type\s*=(.*)',  "\n".join(contents))

    if grid_search :
        catalog=catalog_search.group(1).strip() 
    else:
        logger.error('config file  ('+folder_name+'/run_config.config) do not contains the model-grid-file keyword')
        return 1 
    if catalog_search:
        grid=grid_search.group(1).strip()
    else:
        logger.error('config file  ('+folder_name+'/run_config.config) do not contains the catalog-type keyword')
   

    # 2) read the result file, check for  best fitted model and scaling
    if  exists(folder_name+'/phz_cat.fits'):
        results = Table.read(folder_name+'/phz_cat.fits')
    elif exists(folder_name+'/phz_cat.txt'):
        results = Table.read(folder_name+'/phz_cat.txt', format='ascii')
        
        nb_col = len(results.colnames)
        with open(folder_name+'/phz_cat.txt') as f:
            for col_id in range(nb_col):
                col_search=False
                while not col_search :
                     line = f.readline()
                     col_search = re.search('# Column:\s*(\S*)',str(line))
                results['col'+str(col_id+1)].name=col_search.group(1).strip()
        
    else:
        logger.error('result file not found in folder ('+folder_name+')')
        return 1  
       
    if not 'SED-Index' in results.colnames:
        logger.error('The Best fitted model is missing into the result file')
        return 1 
    
    print(results)
    # 3) Read the grid
    
    # 4) lookup the model flux in the grid
    
    # 5) plots the data

