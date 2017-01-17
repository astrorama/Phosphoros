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


import argparse
import os
import math
import astropy.table as table
import numpy as np
from scipy.interpolate import interp1d
import ElementsKernel.Logging as log


logger = log.getLogger('PhosphorosAddEmissionLines')
phos_dir = os.getenv('PHOSPHOROS_ROOT', None)
aux_dir = next((p+os.path.sep+'EmissionLines' for p in os.environ['ELEMENTS_AUX_PATH'].split(os.pathsep) if os.path.isdir(p+os.path.sep+'EmissionLines')), None)


def defineSpecificProgramOptions():

    parser = argparse.ArgumentParser()

    parser.add_argument('--hydrogen-lines', default=None, type=str, metavar='FILE',
        help='The hydrogen lines file or OFF (default: $ELEMENTS_AUX_PATH/EmissionLines/hydrogen_lines.txt)')
    parser.add_argument('--metallic-lines', default=None, type=str, metavar='FILE',
        help='The metallic lines file or OFF (default: $ELEMENTS_AUX_PATH/EmissionLines/metallic_lines.txt)')
    parser.add_argument('--metallicities', default=[0.0004, 0.004, 0.01], type=float, nargs='+',
        metavar='Z', help='The metallicities (in solar units) for each table column (default: 0.0004 0.004 0.01)')
    parser.add_argument('--first-metal-index', default=2, type=int,
        help='The sindex of the first metallicity column')
    parser.add_argument('--ionized-photons', default=None, type=str, metavar='FILE',
        help='The metallicity to ionized photons table (default: $ELEMENTS_AUX_PATH/EmissionLines/ionized-photons.txt)')
    parser.add_argument('--sed-dir', required=True, type=str, metavar='DIR',
        help='The directory containing the SEDs to add the mission lines on')
    parser.add_argument('--hydrogen-factors', default=[0.5, 1.], type=float, nargs='+',
        help='The differnet factors of the hydrogen lines')
    parser.add_argument('--metallic-factors', default=[0.3, 1., 2.], type=float, nargs='+',
        help='The differnet factors (relative to H lines) of the metallic lines')
    parser.add_argument('--velocity', default=None, type=float,
        help='The velocity (in km/s) to compute the FWHM of the lines from (defaults to dirac)')
    parser.add_argument('--no-sed', action='store_true', help='Output only the emission lines')

    return parser


def readLinesFromFiles(hydrogen_file, metallic_file):
    
    if hydrogen_file == None:
        hydrogen_file = aux_dir + os.path.sep + 'hydrogen_lines.txt'
    if metallic_file == None:
        metallic_file = aux_dir + os.path.sep + 'metallic_lines.txt'
        
    if hydrogen_file == 'OFF':
        hydrogen_lines = []
    else:
        hydrogen_lines = table.Table.read(hydrogen_file, format='ascii')
    if metallic_file == 'OFF':
        metallic_lines = []
    else:
        metallic_lines = table.Table.read(metallic_file, format='ascii')
    
    if len(hydrogen_lines) != 0 and len(metallic_lines) != 0 and len(hydrogen_lines.colnames) != len(metallic_lines.colnames):
        logger.info('Different number of columns in files '+hydrogen_file+' and '+metallic_file)
    
    return hydrogen_lines, metallic_lines


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
    
    class Dirac(object):
        
        def getRange(self, wavelength):
            self.a = wavelength - 1
            self.b = wavelength + 1
            self.wavelength = wavelength
            return self.a, self.b
        
        def addKnots(self, xs):
            xs.add(self.a)
            xs.add(self.b)
            xs.add(self.wavelength)
            
        def getValues(self, xs, flux):
            low = [(x-self.a)*flux for x in xs if x<=self.wavelength]
            high = [(self.b-x)*flux for x in xs if x>self.wavelength]
            return low + high
        
    class Gaussian(object):
        
        def __init__(self, velocity):
            self.velocity = velocity
        
        def getRange(self, wavelength):
            self.fwhm = wavelength * self.velocity / 299792.458 # lambda * v / c
            self.a = wavelength - 2 * self.fwhm
            self.b = wavelength + 2 * self.fwhm
            self.sigma = self.fwhm / 2.355
            self.mu = wavelength
            return self.a, self.b
        
        def addKnots(self, xs):
            for x in np.linspace(self.a, self.b, 31):
                xs.add(x)
            
        def getValues(self, xs, flux):
            return [flux*np.exp(-np.power((x-self.mu), 2.)/(2*np.power(self.sigma, 2.)))/(self.sigma*np.sqrt(2*np.pi)) for x in xs]
    
    def __init__(self, ionized_photons_func, hydrogen_lines, metallic_lines, velocity):
        self.ionized_photons_func = ionized_photons_func
        self.hydrogen_lines = hydrogen_lines
        self.metallic_lines = metallic_lines
        if (velocity == None):
            self.handler = EmissionLinesAdder.Dirac()
        else:
            self.handler = EmissionLinesAdder.Gaussian(velocity)

    def _addSingleLine(self, sed, flux, wavelength):

        # Compute the range where we add the line flux
        a,b = self.handler.getRange(wavelength)

        # Split the parts of the sed that are not afffected by the line
        before = [x for x in sed if x[0] < a]
        after = [x for x in sed if x[0] > b]
        middle = [x for x in sed if x[0] >= a and x[0] <= b]

        # Compute all the knots of the part which is affected by the line by
        # combining the ones necessary for the line and the sed middle knots
        xs = set([x[0] for x in middle])
        self.handler.addKnots(xs)
        xs = sorted(xs)

        # Compute the interpolated middle part of the sed
        sed_ys = np.interp(xs, [x for x,y in sed], [y for x,y in sed])

        # Compute the emission line points
        line_ys = self.handler.getValues(xs, flux)

        # Create and return the final sed
        ys = [y1+y2 for y1,y2 in zip(sed_ys, line_ys)]
        middle = list(zip(xs, ys))
        del sed[:]
        sed.extend(before)
        sed.extend(middle)
        sed.extend(after)
            
    def __call__(self, sed, metal, metal_index, hydrogen_factor, metallic_factor, no_sed):
        lym_cont = sed.func(1500) * self.ionized_photons_func(metal)
        h_beta_flux = lym_cont * 4.757E-13 * hydrogen_factor
        result = sed.data[:]
        if no_sed:
            result = [[r[0], 0] for r in result]
        for l in self.hydrogen_lines:
            wavelength = l[1]
            flux = h_beta_flux * l[metal_index]
            self._addSingleLine(result, flux, wavelength)
        for l in self.metallic_lines:
            wavelength = l[1]
            flux = h_beta_flux * l[metal_index] * metallic_factor
            self._addSingleLine(result, flux, wavelength)
        return result


def mainMethod(args):

    hydrogen_lines, metallic_lines = readLinesFromFiles(args.hydrogen_lines, args.metallic_lines)
    ionized_photons_func = readIonizedPhotonsFromFile(args.ionized_photons)
    adder = EmissionLinesAdder(ionized_photons_func, hydrogen_lines, metallic_lines, args.velocity)
    
    sed_dir = getSedDir(args.sed_dir)
    out_dir = sed_dir + '_el'
    if os.path.exists(out_dir):
        logger.error('Output directory '+out_dir+' already exists')
        exit(1)
    os.makedirs(out_dir)
    for sed_file in os.listdir(sed_dir):
        logger.info('Handling SED '+sed_file)
        sed = loadSed(os.path.join(sed_dir, sed_file))
        for metal_i, metal in enumerate(args.metallicities):
            for hf in args.hydrogen_factors:
                for mf in args.metallic_factors:
                    out_sed = adder(sed, metal, metal_i+args.first_metal_index, hf, mf, args.no_sed)
                    t = table.Table(rows=out_sed, names=('Wave', 'Flux'))
                    t.write(os.path.join(out_dir, sed_file+'_'+str(metal)+'_'+str(hf)+'_'+str(mf)+'.sed'),
                            format='ascii.commented_header')