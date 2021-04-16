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
File: python/PhzCLI/TableUtils.py

Created on: 2021-04-17
Author: dubathf
"""

from __future__ import division, print_function
import os.path
import argparse
import re
from ElementsKernel import Logging


# Read all keywords from a XYDataset file
def readXYDatasetKeyword(file_path):
    result = {}
    keyword_finder = re.compile('^\s*#\s*(\w+)\s*:\s*(.+)\s*$')
    with open(file_path) as f:
        content = f.readlines()
        for line in content:
            matches = keyword_finder.match(line)
            if matches:
                if not matches.groups()[0] in result:
                    result[matches.groups()[0]] = []
                if not matches.groups()[1] in result[matches.groups()[0]]:     
                    result[matches.groups()[0]].append(matches.groups()[1])
    
    # for backward comatibility
    index=0
    while len(content[index])==0:
        index+=1
    
    first_line = content[index]
    if "#" in first_line and not ":" in first_line:
        name = first_line.split("#")[1]
        result['NAME']=[name.strip()]

    return result
    

# Replace all keywork in a XYDataset file
def replaceXYDatasetKeyword(file_path, keyword_dict):
    content = []
    with open(file_path) as f:
        content = f.readlines()
    
    with open(file_path, 'w') as f:
        for key, values in keyword_dict.items():
            for val in values:
                f.write('# '+key+" : "+val+"\n")
         
        for line in content:
            if len(line)>0 and not "#" in line:
                f.write(line)
                










def defineSpecificProgramOptions():
    description = """
            Handle SED file arguments
            """

    parser = argparse.ArgumentParser(description=description)

    parser.add_argument("-f", "--file", type=str, required=True,
                        help="TEXT file containing the SED")
    
    parser.add_argument('-s', '--show', action='store_true',
                        help="Read and display the current KEYWORD arguments")
    
    
    parser.add_argument('-d', '--drop', action='store_true',
                        help="drop all the current KEYWORD arguments")
   
    parser.add_argument(
        '--keyword-argument',
        nargs='+',
        type=str,
        default=[],
        help='Add a keyword:argument key to the file (if not duplicated)'
    )
    
    return parser


def mainMethod(args):
    logger = Logging.getLogger('SED_Handler')
    logger.info('Entering SED_Handler mainMethod()')
    
    
    if not os.path.isfile(args.file):
        logger.error('Provided file '+args.file +'not accessible')
        return 1

    current = readXYDatasetKeyword(args.file)
    if args.show:
        logger.info("Listing the KEYWORD argument")
        for key, values in current.items():
            for val in values:
                logger.info(" -> "+key+" : "+val)
    
    new_header = {}            
    if not args.drop:
        new_header = current.copy()
        
    for input in args.keyword_argument:
        bits = input.split(':')
        if len(bits)!=2:
            logger.error('Provided keyword-argument ('+input +')not valid')
            return 1
        if not bits[0] in new_header:
            new_header[bits[0]]=[]
        if not bits[1] in new_header[bits[0]]:
            new_header[bits[0]].append(bits[1])
    
    replaceXYDatasetKeyword(args.file, new_header)
            



