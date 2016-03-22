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
File: python/EmissionLines/PhosphorosAddEmissionLines.py

Created on: 03/09/16
Author: nikoapos
"""

from __future__ import division, print_function
from future_builtins import *

import argparse
import os
import math
import astropy.table as table
from scipy.interpolate import interp1d
import ElementsKernel.Logging as log


logger = log.getLogger('PhosphorosAddEmissionLines')
phos_dir = os.getenv('PHOSPHOROS_ROOT', None)
aux_dir = next((p+os.path.sep+'EmissionLines' for p in os.environ['ELEMENTS_AUX_PATH'].split(os.pathsep) if os.path.isdir(p+os.path.sep+'EmissionLines')), None)


def defineSpecificProgramOptions():

    parser = argparse.ArgumentParser()

    parser.add_argument('--hydrogen-lines', default=None, type=str, metavar='FILE',
        help='The hydrogen lines file or OFF (default: $ELEMENTS_AUX_PATH/EmissionLines/hydrogen_lines.txt)')
    parser.add_argument('--metalic-lines', default=None, type=str, metavar='FILE',
        help='The metalic lines file or OFF (default: $ELEMENTS_AUX_PATH/EmissionLines/metalic_lines.txt)')
    parser.add_argument('--hybrid-lines', default=None, type=str, metavar='FILE',
        help='The hybrid lines file or OFF (default: $ELEMENTS_AUX_PATH/EmissionLines/hybrid_lines.txt)')
    parser.add_argument('--metalicities', default=[0.0004, 0.004, 0.01], type=float, nargs='+',
        metavar='Z', help='The metalicities (in solar units) for each table column (default: 0.0004 0.004 0.01)')
    parser.add_argument('--first-metal-index', default=2, type=int,
        help='The sindex of the first metalicity column')
    parser.add_argument('--ionized-photons', default=None, type=str, metavar='FILE',
        help='The metalicity to ionized photons table (default: $ELEMENTS_AUX_PATH/EmissionLines/ionized-photons.txt)')
    parser.add_argument('--sed-dir', required=True, type=str, metavar='DIR',
        help='The directory containing the SEDs to add the mission lines on')
    parser.add_argument('--hydrogen-factors', default=[0.5, 1.], type=float, nargs='+',
        help='The differnet factors of the hydrogen lines')
    parser.add_argument('--metalic-factors', default=[0.3, 1., 2.], type=float, nargs='+',
        help='The differnet factors (relative to H lines) of the metalic lines')

    return parser


def readLinesFromFiles(hydrogen_file, metalic_file, hybrid_file):
    
    if hydrogen_file == None:
        hydrogen_file = aux_dir + os.path.sep + 'hydrogen_lines.txt'
    if metalic_file == None:
        metalic_file = aux_dir + os.path.sep + 'metalic_lines.txt'
    if hybrid_file == None:
        hybrid_file = aux_dir + os.path.sep + 'hybrid_lines.txt'
        
    if hydrogen_file == 'OFF':
        hydrogen_lines = []
    else:
        hydrogen_lines = table.Table.read(hydrogen_file, format='ascii')
    if metalic_file == 'OFF':
        metalic_lines = []
    else:
        metalic_lines = table.Table.read(metalic_file, format='ascii')
    if hybrid_file == 'OFF':
        hybrid_lines = []
    else:
        hybrid_lines = table.Table.read(hybrid_file, format='ascii')
    
    if len(hydrogen_lines) != 0 and len(metalic_lines) != 0 and len(hydrogen_lines.colnames) != len(metalic_lines.colnames):
        logger.info('Different number of columns in files '+hydrogen_file+' and '+metalic_file)
    if len(hybrid_lines) != 0 and len(metalic_lines) != 0 and len(hybrid_lines.colnames) != len(metalic_lines.colnames):
        logger.info('Different number of columns in files '+hybrid_file+' and '+metalic_file)
    if len(hydrogen_lines) != 0 and len(hybrid_lines) != 0 and len(hydrogen_lines.colnames) != len(hybrid_lines.colnames):
        logger.info('Different number of columns in files '+hydrogen_file+' and '+hybrid_file)
    
    return hydrogen_lines, metalic_lines, hybrid_lines


def readIonizedPhotonsFromFile(ionized_photons_file):
    if ionized_photons_file == None:
        ionized_photons_file = aux_dir + os.path.sep + 'ionized-photons.txt'
    t = table.Table.read(ionized_photons_file, format='ascii')
    xs = [x for x,y in t]
    ys = [math.pow(10, y) for x,y in t]
    xs = [0] + xs + [1]
    ys = ys[:1] + ys + ys[-1:]
    return interp1d(xs, ys)


def getSedDir(sed_dir):
    if os.path.exists(sed_dir):
        if not os.path.isdir(sed_dir):
            logger.error(sed_dir + ' is not a directory')
            exit(1)
        return sed_dir
    if not os.path.isabs(sed_dir):
        path_in_phos_sed = os.path.join(phos_dir, 'AuxiliaryData', 'SEDs', sed_dir)
        if os.path.isdir(path_in_phos_sed):
            return path_in_phos_sed
    logger.error('Unknown SED directory '+sed_dir)
    exit(1)


class Sed(object):

    def __init__(self, data, name=''):
        self.data = data
        self._interp = interp1d([x for x,y in data], [y for x,y in data])
        self.func = lambda x: self._interp([x])[0]
        self.name = name


def loadSed(filename):
    t = table.Table.read(filename, format='ascii')
    return Sed(list(zip(t.columns[0], t.columns[1])), os.path.basename(filename))


class EmissionLinesAdder(object):
    
    def __init__(self, ionized_photons_func, hydrogen_lines, metalic_lines, hybrid_lines):
        self.ionized_photons_func = ionized_photons_func
        self.hydrogen_lines = hydrogen_lines
        self.metalic_lines = metalic_lines
        self.hybrid_lines = hybrid_lines
        
    def _addSingleLine(self, sed, flux, wavelength):
        i = 0
        for pair in sed:
            if pair[0] > wavelength:
                break
            i = i + 1
        a = wavelength - 1
        b = wavelength + 1
        if a <= sed[i-1][0]:
            b = b + sed[i-1][0] - a
            a = sed[i-1][0]
        if b >= sed[i][0]:
            a = a + sed[i][0] - b
            b = sed[i][0]
        if a < sed[i-1][0] or b > sed[i][0]:
            raise Exception('Too small wavelength step to add emission line')
        total = sed[i][0] - sed[i-1][0]
        a_value = sed[i-1][1] * (sed[i][0] - a) / total + sed[i][1] * (a - sed[i-1][0]) / total
        b_value = sed[i-1][1] * (sed[i][0] - b) / total + sed[i][1] * (b - sed[i-1][0]) / total
        line_value = flux + (a_value + b_value) / 2.
        if a != sed[i-1][0]:
            sed.insert(i, (a, a_value))
            i = i + 1
        sed.insert(i, (wavelength, line_value))
        i = i + 1
        if b != sed[i][0]:
            sed.insert(i, (b, b_value))
            
    def __call__(self, sed, metal, metal_index, hydrogen_factor, metalic_factor):
        lym_cont = sed.func(1500) * self.ionized_photons_func(metal)
        h_beta_flux = lym_cont * 4.757E-13 * hydrogen_factor
        result = sed.data[:]
        for l in self.hydrogen_lines:
            wavelength = l[1]
            flux = h_beta_flux * l[metal_index]
            self._addSingleLine(result, flux, wavelength)
        for l in self.metalic_lines:
            wavelength = l[1]
            flux = h_beta_flux * l[metal_index] * metalic_factor
            self._addSingleLine(result, flux, wavelength)
        hybrid_factor = (metalic_factor - 1) / 2 + 1
        for l in self.hybrid_lines:
            wavelength = l[1]
            flux = h_beta_flux * l[metal_index] * hybrid_factor
            self._addSingleLine(result, flux, wavelength)
        return result


def mainMethod(args):

    hydrogen_lines, metalic_lines, hybrid_lines = readLinesFromFiles(args.hydrogen_lines, args.metalic_lines, args.hybrid_lines)
    ionized_photons_func = readIonizedPhotonsFromFile(args.ionized_photons)
    adder = EmissionLinesAdder(ionized_photons_func, hydrogen_lines, metalic_lines, hybrid_lines)
    
    sed_dir = getSedDir(args.sed_dir)
    out_dir = sed_dir + '_el'
    if os.path.exists(out_dir):
        logger.error('Output directory '+out_dir+' already exists')
        exit(1)
    os.makedirs(out_dir)
    for sed_file in os.listdir(sed_dir):
        logger.info('Handling SED '+sed_file)
        sed = loadSed(os.path.join(sed_dir, sed_file))
        for metal_i, metal in enumerate(args.metalicities):
            for hf in args.hydrogen_factors:
                for mf in args.metalic_factors:
                    out_sed = adder(sed, metal, metal_i+args.first_metal_index, hf, mf)
                    t = table.Table(rows=out_sed, names=('Wave', 'Flux'))
                    t.write(os.path.join(out_dir, sed_file+'_'+str(metal)+'_'+str(hf)+'_'+str(mf)+'.sed'),
                            format='ascii.commented_header')