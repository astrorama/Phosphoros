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
import astropy.table as table
import numpy as np
import ElementsKernel.Logging as log
from EmissionLines import SedUtils

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
                        help='The emission lines file (default: Ha_lines.txt, use LePhare_lines.txt for LePhare like lines)')
    parser.add_argument('--uv-range', default=(1500.0, 2800.0), type=wavelengthRange,
                        help='The beginning of the UV range to integrate (default: 1500.0,2800.0 Angstrom, use  2100,2500 for LePhare like lines)' )
    parser.add_argument('--reference-factor', default=5.91e-6, type=float,
                        help='The luminosity factor between UV and the reference line (default:5.91e-6, use 1.0e13 for LePhare like  lines)')
    parser.add_argument('--sed-dir', required=True, type=str, metavar='DIR',
                        help='The directory containing the SEDs to add the emission lines on')
    parser.add_argument('--velocity', default=None, type=float,
                        help='The velocity (in km/s) to compute the FWHM of the lines from (defaults to dirac)')
    parser.add_argument('--no-sed', action='store_true', help='Output only the emission lines')
    parser.add_argument('--suffix', default="_el", type=str,
                        help='Suffix to be added to the directory name to form the output directory')
    parser.add_argument('--copy-parameter', default=True, type=bool,
                        help='Define if the header containing physical parameters has to be copied into the new SEDs')

    return parser


def readEmissionLinesFromFile(emission_lines_file):
    if "/" not in emission_lines_file and aux_dir:
        emission_lines_file = os.path.join(aux_dir, emission_lines_file)

    logger.info('Reading emission lines from ' + emission_lines_file)
    return table.Table.read(emission_lines_file, format='ascii')


def getSedDir(sed_dir):
    if os.path.exists(sed_dir):
        if not os.path.isdir(sed_dir):
            logger.error(sed_dir + ' is not a directory')
            exit(1)
        return sed_dir
    if not os.path.isabs(sed_dir) and not os.path.exists(sed_dir):
        path_in_phos_sed = os.path.join(phos_dir, 'AuxiliaryData', 'SEDs', sed_dir)
        if os.path.isdir(path_in_phos_sed):
            return path_in_phos_sed
    logger.error('Unknown SED directory ' + sed_dir)
    exit(1)


def isSedFile(filename):
    logger.info('Testing if file ' + filename + ' contains a SED')
    try:
        t = table.Table.read(filename, format='ascii')
        if len(t.colnames) != 2:
            logger.info('Not a SED: Wrong column number')
            return False
        if np.issubdtype(t[t.colnames[0]].dtype, np.number) and np.issubdtype(t[t.colnames[1]].dtype, np.number):
            return True
        else:
            logger.info('Not a SED: wrong column type')
            return False
    except Exception as e:
        logger.info('Not a SED: unable to open the table: ' + str(e))
        return False


class Sed(object):

    def __init__(self, data, name=''):
        self.data = data
        self.name = name

    @staticmethod
    def load(filename):
        t = table.Table.read(filename, format='ascii')
        return Sed(
            np.array([t.columns[0].data, t.columns[1].data]).transpose(),
            os.path.basename(filename)
        )


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
        before = sed[sed[:, 0] < a]
        after = sed[sed[:, 0] > b]
        middle_filter = np.vectorize(lambda x: b >= x >= a)(sed[:, 0])
        middle = sed[middle_filter]

        # Compute all the knots of the part which is affected by the line by
        # combining the ones necessary for the line and the sed middle knots
        xs = set(middle[:0])
        self.handler.addKnots(xs)
        xs = sorted(xs)

        # Compute the interpolated middle part of the sed
        sed_ys = np.interp(xs, sed[:, 0], sed[:, 1])

        # Compute the emission line points
        line_ys = self.handler.getValues(xs, flux)

        # Create and return the final sed
        ys = [y1 + y2 for y1, y2 in zip(sed_ys, line_ys)]
        middle = np.array([xs, ys]).transpose()
        return np.concatenate([before, middle, after])
            
    def __call__(self, sed):
        # Get subset relevant for the integration
        selection_filter = np.logical_and(sed.data[:, 0] >= self.uv_range[0], sed.data[:, 0] <= self.uv_range[1])
        trunc_sed = sed.data[selection_filter, :]

        # Integrate this segment
        uv_flux = np.trapz(trunc_sed[:, 1], trunc_sed[:, 0])

        # Calculate the reference band flux density
        ref_flux = (self.ref_factor * self.uv_range[1] * self.uv_range[0]) / (self.uv_range[1] - self.uv_range[0])
        ref_flux *= uv_flux

        result = np.array(sed.data, copy=True)
        if self.no_sed:
            result[:, 1] = 0

        for line in self.emission_lines:
            wavelength = line[1]
            flux_freq = ref_flux * line[2]
            result = self._addSingleLine(result, flux_freq, wavelength)
        return result


def mainMethod(args):
    sed_dir = getSedDir(args.sed_dir)
    out_dir = sed_dir.rstrip(os.path.sep) + args.suffix

    if os.path.exists(out_dir):
        logger.error('Output directory ' + out_dir + ' already exists')
        exit(1)
    else:
        os.makedirs(out_dir)

    logger.info('SED directory: %s', sed_dir)
    logger.info('Output directory: %s', out_dir)
    logger.info('Aux directory: %s', aux_dir)

    emission_lines = readEmissionLinesFromFile(args.emission_lines)
    adder = EmissionLinesAdder(
        args.uv_range,
        args.reference_factor,
        emission_lines,
        args.velocity,
        args.no_sed
    )

    for sed_file in os.listdir(sed_dir):
        if isSedFile(os.path.join(sed_dir, sed_file)):
            logger.info('Handling SED ' + sed_file)
            sed = Sed.load(os.path.join(sed_dir, sed_file))
            out_sed = adder(sed)
            t = table.Table(rows=out_sed, names=('Wave', 'Flux'))
            t.write(os.path.join(out_dir, sed_file), format='ascii.commented_header')
            if (args.copy_parameter):
                logger.info('Copy the header from ' + sed_file)
                parameters = SedUtils.readXYDatasetKeyword(os.path.join(sed_dir, sed_file))
                parameters.pop('NAME', None)
                SedUtils.replaceXYDatasetKeyword(os.path.join(out_dir, sed_file), parameters)
