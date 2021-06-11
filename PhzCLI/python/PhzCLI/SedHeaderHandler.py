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
File: python/PhzCLI/SedHeaderHandler.py

Created on: 2021-04-17
Author: dubathf
"""

from __future__ import division, print_function
import os.path
import argparse
import re

from EmissionLines import SedUtils
from ElementsKernel import Logging


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
        '--remove-key',
        type=str,
        default=[],
        action='append',
        help='remove a keyword key to the file (all the occurences - done before adding new key)'
    )
    
    parser.add_argument(
        '--add-argument',
        type=str,
        default=[],
        action='append',
        help='Add a keyword:argument key to the file (if not duplicated)'
    )
    
    return parser


def mainMethod(args):
    logger = Logging.getLogger('SED_Handler')
    logger.info('Entering SED_Handler mainMethod()')
    
    
    if not os.path.isfile(args.file):
        logger.error('Provided file '+args.file +'not accessible')
        return 1

    current = SedUtils.readXYDatasetKeyword(args.file)
  
    new_header = {}            
    if not args.drop:
        new_header = current.copy()
        
    for key in args.remove_key:
        if key in  new_header.keys():
            logger.info("Removing the KEYWORD "+key+" from the header")
            del new_header[key]
        else:
            logger.warn("Cannot remove the KEYWORD "+key+" : it was not present into the header")
        
        
    for input in args.add_argument:
        bits = input.split(':')
        if len(bits)!=2:
            logger.error('Provided keyword-argument ('+input +')not valid')
            return 1
        if not bits[0] in new_header:
            new_header[bits[0]]=[]
        if not bits[1] in new_header[bits[0]]:
            new_header[bits[0]].append(bits[1])
            
    if args.show:
        logger.info("Listing the KEYWORD argument")
        for key, values in new_header.items():
            for val in values:
                logger.info(" -> "+key+" : "+val)
    
    if not SedUtils.compareXYDatasetKeyword(current, new_header):
        logger.info("The header has been modified: overwriting the file")
        SedUtils.replaceXYDatasetKeyword(args.file, new_header)
            