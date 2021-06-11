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
File: python/EmissionLines/SedUtils.py

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
                




def compareXYDatasetKeyword(keyword_dict_1, keyword_dict_2):
    if (len(keyword_dict_1)==len(keyword_dict_2)):
        for key in keyword_dict_1.keys():
            if key in keyword_dict_2.keys():
                if len(keyword_dict_1[key])==len(keyword_dict_2[key]):
                    for val in keyword_dict_1[key]:
                        if not val in keyword_dict_2[key]:
                            return False
                else:
                    return False
            else:
                return False
        return True    
    else:
        return False        
