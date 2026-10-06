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

import re
import argparse
import os
import astropy.table as table
import numpy as np
import ElementsKernel.Logging as log
from XYDatasetSet import XYDatasetSetTools

C_ANGSTROM = 2.99792458e+18

logger = log.getLogger('PhosphorosAddEmissionLines')
phos_dir = os.getenv('PHOSPHOROS_ROOT', os.path.expanduser('~/Phosphoros'))
aux_dir = next(filter(
    os.path.isdir,
    map(lambda p: os.path.join(p, 'EmissionLines'), os.getenv('ELEMENTS_AUX_PATH', '').split(os.pathsep))
), None)
conda_prefix = os.getenv('CONDA_PREFIX', None)
if aux_dir is None and conda_prefix is not None:
    aux_dir = os.path.join(conda_prefix, 'share', 'auxdir', 'EmissionLines')


def defineSpecificProgramOptions():
    def wavelengthRange(s):
        try:
            start, end = map(float, s.split(','))
            return start, end
        except ValueError:
            raise argparse.ArgumentParser('A range must be start,end (in Angstrom)')

    parser = argparse.ArgumentParser()

    parser.add_argument('--emission-lines', default='Ha_lines.txt', type=str, metavar='FILE',
                        help='The emission lines file (default: Ha_lines.txt, use LePhare_lines.txt for LePhare like lines), local file will be prefered over defult file installed along the code')
    parser.add_argument('--uv-range', default=(1500.0, 2800.0), type=wavelengthRange,
                        help='The beginning of the UV range to integrate (default: 1500.0,2800.0 Angstrom, use  2100,2500 for LePhare like lines)' )
    parser.add_argument('--reference-factor', default=5.91e-6, type=float,
                        help='The luminosity factor between UV and the reference line (default:5.91e-6, use 1.0e13 for LePhare like  lines)')
    parser.add_argument('--sed-set', type=str, 
                        help='The directory/.fits file containing the SEDs to add the emission lines on')
    parser.add_argument('--velocity', default=None, type=float,
                        help='The velocity (in km/s) to compute the FWHM of the lines from (defaults to dirac)')
    parser.add_argument('--no-sed', action='store_true', help='Output only the emission lines')
    parser.add_argument('--copy-parameter', type=bool,
                        help='IF present, copy the header containing physical parameters has to be copied into the new SEDs')
                        
    parser.add_argument('--suffix', default="_el", type=str,
                        help='Suffix to be added to the directory name to form the output directory/file')
    parser.add_argument('--out-format', default="individual", type=str,
                        help='Select between individual SED files ("individual") or in a set .fits file ("set"). Default is "individual"')

    return parser

def readEmissionLinesFromFile(emission_lines_file):
    if emission_lines_file[0]=='/':
        # Absolute path: nothing to do 
        pass
    elif os.path.isfile(emission_lines_file):
        #local file exists : nothing to do
        pass
    elif aux_dir:
       # try to read the file from aux dir 
       emission_lines_file = os.path.join(aux_dir, emission_lines_file)

    logger.info('Reading emission lines from ' + emission_lines_file)
    return table.Table.read(emission_lines_file, format='ascii')


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
            low = [(x - self.a) * flux for x in xs if x <= self.wavelength]
            high = [(self.b - x) * flux for x in xs if x > self.wavelength]
            return low + high

    class Gaussian(object):

        def __init__(self, velocity):
            self.velocity = velocity

        def getRange(self, wavelength):
            self.fwhm = wavelength * self.velocity / 299792.458  # lambda * v / c
            self.a = wavelength - 2 * self.fwhm
            self.b = wavelength + 2 * self.fwhm
            self.sigma = self.fwhm / 2.355
            self.mu = wavelength
            return self.a, self.b

        def addKnots(self, xs):
            for x in np.linspace(self.a, self.b, 31):
                xs.add(x)

        def getValues(self, xs, flux):
            x = np.asarray(xs)
            delta = x - self.mu
            return flux * np.exp(-delta ** 2 / (2 * self.sigma ** 2)) / (self.sigma * np.sqrt(2 * np.pi))

    def __init__(self, uv_range, ref_factor, emission_lines, velocity, no_sed):
        self.uv_range = uv_range
        self.ref_factor = ref_factor
        self.emission_lines = emission_lines
        self.no_sed = no_sed
        if velocity is None:
            
            logger.info('Using Dirac')
            self.handler = EmissionLinesAdder.Dirac()
        else:
            self.handler = EmissionLinesAdder.Gaussian(velocity)

    def _addSingleLine(self, sed, flux, wavelength):
        # Compute the range where we add the line flux
        a, b = self.handler.getRange(wavelength)

        # Split the parts of the sed that are not affected by the line
        before_x = sed.x[sed.x < a]
        before_y = sed.y[sed.x < a]
        after_x = sed.x[sed.x > b]
        after_y = sed.y[sed.x > b]
        middle_filter = np.logical_and(sed.x>=a, sed.x<=b)
        middle_x = sed.x[middle_filter]
        middle_y = sed.y[middle_filter]

        # Compute all the knots of the part which is affected by the line by
        # combining the ones necessary for the line and the sed middle knots
        xs = set(middle_x)
        self.handler.addKnots(xs)
        xs = sorted(xs)

        # Compute the interpolated middle part of the sed
        sed_ys = np.interp(xs, sed.x, sed.y)

        # Compute the emission line points
        line_ys = self.handler.getValues(xs, flux)

        # Create and return the final sed
        ys = [y1 + y2 for y1, y2 in zip(sed_ys, line_ys)]
        sed.x = np.concatenate([before_x, xs, after_x])
        sed.y = np.concatenate([before_y, ys, after_y])
        return sed
            
    def __call__(self, sed):
        # Get subset relevant for the integration
        selection_filter = np.logical_and(sed.x >= self.uv_range[0], sed.x <= self.uv_range[1])
        trunc_sed_x = sed.x[selection_filter]
        trunc_sed_y = sed.y[selection_filter]

        # Integrate this segment
        uv_flux = np.trapz(trunc_sed_y, trunc_sed_x)

        # Calculate the reference band flux density
        ref_flux = (self.ref_factor * self.uv_range[1] * self.uv_range[0]) / (self.uv_range[1] - self.uv_range[0])
        ref_flux *= uv_flux

        if self.no_sed:
            sed.y[:] = 0

        for line in self.emission_lines:
            wavelength = line[1]
            flux_freq = ref_flux * line[2]
            sed = self._addSingleLine(sed, flux_freq, wavelength)
        return sed


def getSedPath(sed_set):
    if os.path.exists(sed_set):
        if os.path.isdir(sed_set):
            return sed_set, ""
        elif sed_set.endswith(".fits"):
            return "/".join(sed_set.split('/')[:-1]), sed_set.split('/')[-1].replace(".fits", "")
        else:    
            logger.error(sed_set + ' is not a directory or a .fits file')
            exit(1)
    elif os.path.exists(sed_set+".fits"):
        return "/".join(sed_set.split('/')[:-1]), sed_set.split('/')[-1]
            
    elif not os.path.isabs(sed_set):
        path_in_phos_sed = os.path.join(sed_set, 'AuxiliaryData', 'SEDs', sed_dir)
        if os.path.isdir(path_in_phos_sed):
            if os.path.isdir(path_in_phos_sed):
                return path_in_phos_sed, ""
            elif path_in_phos_sed.endswith(".fits"):
                return "/".join(path_in_phos_sed.split('/')[:-1]), path_in_phos_sed.split('/')[-1].replace(".fits", "")
            else:    
                logger.error(path_in_phos_sed + ' is not a directory or a .fits file')
                exit(1)
        elif os.path.exists(path_in_phos_sed+".fits"):
            return "/".join(path_in_phos_sed.split('/')[:-1]), path_in_phos_sed.split('/')[-1]
                
    logger.error('Unknown SED group ' + sed_set)
    exit(1)


def mainMethod(args):
    sed_dir, sed_file = getSedPath(args.sed_set)
        
    logger.info('SED directory: %s', sed_dir)
    logger.info('Aux directory: %s', aux_dir)
    
    out_dir = os.path.join(sed_dir, sed_file).rstrip(os.path.sep) + args.suffix
    if args.out_format=="individual":
        if os.path.exists(out_dir):
            logger.error('Output directory ' + out_dir + ' already exists')
            exit(1)
        else:
            os.makedirs(out_dir)
        
        logger.info('Output directory: %s', out_dir)
    else:
        logger.info('Output file: %s', out_dir)

    emission_lines = readEmissionLinesFromFile(args.emission_lines)
    adder = EmissionLinesAdder(
        args.uv_range,
        args.reference_factor,
        emission_lines,
        args.velocity,
        args.no_sed
    )

    SED_dict = XYDatasetSetTools.listDataset(sed_dir)
    out_seds = []
    for sed_name in SED_dict:
        if sed_file=="" or (len(sed_name.split('/'))>1 and sed_name.split('/')[-2]==sed_file):
            sed = XYDatasetSetTools.readDataset(sed_dir, sed_name, SED_dict)
            out_sed = adder(sed)
            out_sed=sed
            if not args.copy_parameter:
                for p in out_sed.listParam():
                    out_sed.removeParam(p)
            out_seds.append(out_sed)
          
        
    if args.out_format=="individual":
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
        out_seds = XYDatasetSetTools.checkSampling(out_seds, True)
        XYDatasetSetTools.writeDatasetSet(out_seds, out_dir+'.fits')
        logger.info(f'write {out_dir+'.fits'}')
        
      
    
