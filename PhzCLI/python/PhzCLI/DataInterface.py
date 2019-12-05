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
File: tests/python/DataInterface_test.py

Created on: 06/06/16
Author: nikoapos
"""

from __future__ import division, print_function

import logging
import subprocess
import os
import numpy as np
from astropy.table import Table

logger = logging.getLogger(__name__)


def _runCmdHelper(cmd):
    logger.debug(cmd)
    output = subprocess.check_output(cmd, stderr=subprocess.DEVNULL)
    if isinstance(output, bytes):
        output = output.decode('utf8')
    return output


def getFilterInfo(name):
    
    class FilterInfo(object):
        pass
    filter_info = FilterInfo()
    filter_info.name = name
    
    # If the filter does not exist return None
    cmd = 'Phosphoros DF'.split()
    available_filters = _runCmdHelper(cmd).split()
    if name not in available_filters:
        return None
    
    # Build the filter data from the CLI output
    cmd = 'Phosphoros DF --data'.split()
    cmd.append(name)
    data_str = _runCmdHelper(cmd).splitlines()
    filter_info.data = np.zeros([2, len(data_str)])
    for i, row in enumerate(data_str):
        row_d = row.split()
        filter_info.data[0][i] = row_d[0]
        filter_info.data[1][i] = row_d[1]
    
    # Compute the mean
    nom = 0
    den = 0
    for i in range(filter_info.data.shape[1]):
        nom += filter_info.data[0][i] * filter_info.data[1][i]
        den += filter_info.data[1][i]
    filter_info.mean = nom / den
        
    return filter_info



def parseFilterMapping(filename):
    
    # If the given file does not exist, we return None
    if not os.path.isfile(filename):
        return None
    
    # Parse the file and create the result
    with open(filename) as in_file:
        lines = in_file.readlines()
    mapping = {}
    for l in lines:
        l_data = l[:l.find('#')].strip().split()
        if len(l_data) == 3:
            mapping[l_data[0]] = (l_data[1], l_data[2])
    return mapping



def getModelPhotometry(grid, sed, red_curve, ebv, z):
    
    class Photometry(object):
        pass
    phot = Photometry()

    # Get all the regions names
    cmd = 'Phosphoros DMG --model-grid-file'.split()
    cmd.append(grid)
    lines = _runCmdHelper(cmd).splitlines()
    reg_line = next(line for line in lines if 'Regions names' in line)
    regions = [r.strip() for r in reg_line[reg_line.index(':') + 1:].strip().split('"') if len(r.strip()) != 0]

    # Get the IGM absorption method
    igm_line = next(line for line in lines if 'IGM absorption method' in line)
    phot.igm_type = igm_line[igm_line.index(':')+1:].strip()
    
    # Find the region and the indices
    region = None
    for r in regions:
        sed_i = -1
        red_curve_i = -1
        ebv_i = -1
        z_i = -1
        cmd = 'Phosphoros DMG --model-grid-file'.split()
        cmd.append(grid)
        cmd.append('--region')
        cmd.append(r)
        
        lines = _runCmdHelper(cmd + ['--sed']).splitlines()

        in_values = False
        for l in lines:
            if in_values:
                l_data = l.split()
                if len(l_data) == 2 and l_data[1] == sed:
                    sed_i = l_data[0]
            if l == 'Index\tValue':
                in_values = True
        
        lines = _runCmdHelper(cmd + ['--redcurve']).splitlines()
        in_values = False
        for l in lines:
            if in_values:
                l_data = l.split()
                if len(l_data) == 2 and l_data[1] == red_curve:
                    red_curve_i = l_data[0]
            if l == 'Index\tValue':
                in_values = True
        
        lines = _runCmdHelper(cmd + ['--ebv']).splitlines()
        in_values = False
        for l in lines:
            if in_values:
                l_data = l.split()
                if len(l_data) == 2 and (np.isclose(float(l_data[1]), ebv) or l_data[1] == str(ebv)):
                    ebv_i = l_data[0]
            if l == 'Index\tValue':
                in_values = True
        
        lines = _runCmdHelper(cmd + ['--z']).splitlines()
        in_values = False
        for l in lines:
            if in_values:
                l_data = l.split()
                if len(l_data) == 2 and (np.isclose(float(l_data[1]), z) or l_data[1] == str(z)):
                    z_i = l_data[0]
            if l == 'Index\tValue':
                in_values = True
        
        if sed_i != -1 and red_curve_i != -1 and ebv_i != -1 and z_i != -1:
            region = r
            break
    
    # Get the photometry values
    cmd = 'Phosphoros DMG --model-grid-file'.split()
    cmd.append(grid)
    cmd.append('--region')
    cmd.append(region)
    cmd.append('--phot')
    cmd.append(str(sed_i) + ',' + str(red_curve_i) + ',' + str(ebv_i) + ',' + str(z_i))
    lines = _runCmdHelper(cmd).splitlines()
    phot.photometry = {}
    in_values = False
    for l in lines:
        if in_values:
            l_data = l.split()
            if len(l_data) == 2:
                phot.photometry[l_data[0]] = (float(l_data[1]), 0)
        if 'Photometry:' in l:
            in_values = True
            
    return phot


def getBestFittedModelInfo(source_id, catalog, grid):
    
    class ModelInfo(object):
        pass
    model = ModelInfo()
    
    if not os.path.isfile(catalog):
        return None
    
    try:
        table = Table.read(catalog)
    except:
        table = Table.read(catalog, format='ascii')
    
    row = next(r for r in table if r['ID']==source_id)
    model.sed = row['SED'].strip()
    model.red_curve = row['ReddeningCurve'].strip()
    model.ebv = row['E(B-V)']
    model.z = row['Z']
    model.scale = row['Scale']
    
    phot = getModelPhotometry(grid, model.sed, model.red_curve, model.ebv, model.z)
    model.igm_type = phot.igm_type
    model.photometry = phot.photometry
    for k in model.photometry:
        model.photometry[k] = (model.photometry[k][0] * model.scale, model.photometry[k][1] * model.scale)
    
    # Get the SED template
    cmd = 'Phosphoros CMS --igm-absorption-type'.split()
    cmd.append(model.igm_type)
    cmd.append('--sed-name')
    cmd.append(model.sed)
    cmd.append('--reddening-curve-name')
    cmd.append(model.red_curve)
    cmd.append('--ebv-value')
    cmd.append(str(model.ebv))
    cmd.append('--z-value')
    cmd.append(str(model.z))
    lines = _runCmdHelper(cmd).splitlines()
    i = next(i for i,l in enumerate(lines) if l == 'Data:')
    lines = lines[i+1:-1]
    model.sed = np.zeros([2, len(lines)])
    for i, l in enumerate(lines):
        l_data = l.split()
        wavelength = float(l_data[0])
        value = float(l_data[1])
        # Convert erg/s/cm^2/A to erg/s/cm^2/Hz
        value = value * wavelength * wavelength / 2.99792458e+18;
        # convert erg/s/cm^2/Hz to micro-Jansky
        value = value * 1E29;
        # Scale according the scale factor
        value = value * model.scale
        model.sed[0][i] = wavelength
        model.sed[1][i] = value
    
    return model
