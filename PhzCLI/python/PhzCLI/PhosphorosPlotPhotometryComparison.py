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
File: python/PhzCLI/PhosphorosPlotPhotometryComparison.py

Created on: 06/06/16
Author: nikoapos
"""

from __future__ import division, print_function
from future_builtins import *

import argparse
import matplotlib.pyplot as plt
import ElementsKernel.Logging as log
from astropy.table import Table

import PhzCLI.DataInterface as di

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
    
    parser.add_argument('--source-id', type=int,
                help='The ID of the source to create the plot for')
    parser.add_argument('--filter-mapping-file', type=str,
                help='The file containing the photometry mapping of the catalog columns')
    parser.add_argument('--best-model-catalog', type=str,
                help='The file containing the best fitted model catalog')
    parser.add_argument('--model-grid-file', type=str,
                help='The file containing the grid with the model photometries')
    parser.add_argument('--source-catalog', type=str,
                help='The file containing the catalog with the source photometries')

    #
    # !!! Write your program options here !!!
    # e.g. parser.add_argument('--string-value', type=str, help='A string option')
    #

    return parser


def mainMethod(args):
    """
    @brief The "main" method.
    @details
        This method is the entry point to the program. In this sense, it is 
        similar to a main (and it is why it is called mainMethod()).
    """

    logger = log.getLogger('PhosphorosPlotPhotometry')
    
    filter_mapping = di.parseFilterMapping(args.filter_mapping_file)
    filter_info = [di.getFilterInfo(f) for f in filter_mapping.keys()]
    
    model = di.getBestFittedModelInfo(args.source_id, args.best_model_catalog, args.model_grid_file)
    
    # Create the figure and the axis 
    fig, ax_phot = plt.subplots()
    
    # Plot the filter transmissions
    ax_filt = ax_phot.twinx()
    for f in filter_info:
        lines = ax_filt.plot(f.data[0], f.data[1], alpha=.3)
        f.color = lines[0].get_color()
    ax_filt.set_ylim(0, 1)
    
    # Add some labels for the filters
    for f in filter_info:
        ax_filt.text(f.mean, .01, f.name, {'ha': 'left', 'va': 'bottom'}, color=f.color, rotation=90, alpha=.3)
    
    # Plot the model photometries
    for f in filter_info:
        ax_phot.errorbar([f.mean], [model.photometry[f.name][0]], yerr=[model.photometry[f.name][1]], fmt='s', color=f.color)
    
    # Plot the model SED
    x_lim = ax_phot.get_xlim()
    sed_x = []
    sed_y = []
    for i in range(model.sed.shape[1]):
        if model.sed[0][i] >= x_lim[0] and model.sed[0][i] <= x_lim[1]:
            sed_x.append(model.sed[0][i])
            sed_y.append(model.sed[1][i])
    ax_phot.plot(sed_x, sed_y, c='k')
    
    # Plot the source photometries
    try:
        table = Table.read(args.source_catalog)
    except:
        table = Table.read(args.source_catalog, format='ascii')
    row = next(r for r in table if r['ID']==args.source_id)
    for f in filter_info:
        f_c, e_c = filter_mapping[f.name]
        ax_phot.errorbar([f.mean], [row[f_c]], yerr=[row[e_c]], fmt='o', color=f.color)
    
    ax_phot.set_ylim(bottom=0.)
    plt.show()
    
    
