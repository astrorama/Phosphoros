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
File: python/GalacticDustMap/AddGalDustToCatalog.py

Created on: 11/07/18
Author: fdubath
"""

from __future__ import division, print_function
import sys
if sys.version_info[0] < 3:
    from future_builtins import *

import argparse
from astropy.table import Table
import ElementsKernel.Logging as log
import GalacticDustMap
import ElementsKernel.Auxiliary as aux
import ElementsKernel.Logging as log

def defineSpecificProgramOptions():
    """
    @brief Allows to define the (command line and configuration file) options
    specific to this program

    @details
        See the Elements documentation for more details.
    @return
        An  ArgumentParser.
    """

    parser = argparse.ArgumentParser()

    #
    # !!! Write your program options here !!!
    parser.add_argument('--planck-dust-map', type=str, help='Filename of the Planck dust map in HEALPIX format',
                        default='')
    parser.add_argument('--input-catalog', type=str, help='Input catalog filename')
    parser.add_argument('--output-catalog', type=str, help='Output catalog filename')
    parser.add_argument('--ra', type=str, help='Right Acsension column in the catalog (Degrees)')
    parser.add_argument('--dec', type=str, help='Declination column in the catalog (Degres)')
    parser.add_argument('--galatic-ebv-col', type=str, 
                        help='Name of the column to be added to the output catalog',
                        default='GAL_EBV')


    return parser


def mainMethod(args):
    """
    @brief The "main" method.
    @details
        This method is the entry point to the program. In this sense, it is
        similar to a main (and it is why it is called mainMethod()).
    """

    logger = log.getLogger('AddGalDustToCatalog')

    logger.info('#')
    logger.info('# Entering AddGalDustToCatalog mainMethod()')
    logger.info('#')

    # Read the Planck map
    if arg.planck_dust_map:
        aux_file = arg.planck_dust_map
    else:
        aux_file = aux.getAuxiliaryPath('PlanckEvb.fits')
    map_data = loadMap(aux_file)

    # open catalog
    input_cat = Table.read(args.input_catalog)
    if not args.ra in input_cat.colnames:
        raise ValueError("RA column  missing : %s " % args.ra)
    if not args.dec in input_cat.colnames:
        raise ValueError("DEC column  missing : %s " % args.dec)

    ra = input_cat[args.ra]
    dec = input_cat[args.dec]

    ebv = ebv_planck(map_data, ra, dec)
    input_cat[args.galatic_ebv_col] = ebv
    input_catalog.write(args.output_catalog)

    logger.info('#')
    logger.info('# Exiting AddGalDustToCatalog mainMethod()')
    logger.info('#')
