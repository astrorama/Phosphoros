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
File: python/CreateLuminosityCurve/CreateSquareLuminosityCurve.py

Created on: 28/02/2019
Author: fdubath
"""

from __future__ import division, print_function


import argparse
import os
import astropy.table as table
import numpy as np
import ElementsKernel.Logging as log


logger = log.getLogger('PhosphorosCreateSquareLuminosityCurve')


def defineSpecificProgramOptions():
   
    parser = argparse.ArgumentParser()

    parser.add_argument('--step-begin', required=True,type=float,
        help='Lowest value (in MAG) of the step')

    parser.add_argument('--step-stop', required=True,type=float,
        help='Highest value (in MAG) of the step') 
       
    parser.add_argument('--output-file', required=True, type=str, 
        help='The path to the output file')
        
    return parser


def mainMethod(args):
    min_mag = -100
    max_mag = 50
    
    begin_step = args.step_begin
    end_step = args.step_stop
    
    delta=0.001
    
    if begin_step > end_step:
        swap = begin_step
        begin_step = end_step
        end_step = swap
        
    if min_mag > begin_step:
        raise argparse.ArgumentParser('the step-begin argument must be above '+str(min_mag))
        
    if end_step > max_mag:
        raise argparse.ArgumentParser('the step-stop argument must be below '+str(max_mag))
    
    with open(args.output_file, "w") as text_file:
        text_file.write('{} {}\n'.format(min_mag, 0.0))
        text_file.write('{} {}\n'.format(begin_step - delta, 0.0))
        text_file.write('{} {}\n'.format(begin_step, 1.0))
        text_file.write('{} {}\n'.format(end_step, 1.0))
        text_file.write('{} {}\n'.format(end_step + delta, 0.0))
        text_file.write('{} {}\n'.format(max_mag, 0.0))
