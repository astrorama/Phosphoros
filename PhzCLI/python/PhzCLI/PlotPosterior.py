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

from __future__ import print_function

import argparse

import PhzCLI.LikelihoodPlot as plt
from ElementsKernel import Logging

logger = Logging.getLogger('PlotPosterior')


def defineSpecificProgramOptions():
    """
    @brief Allows to define the (command line and configuration file) options
    specific to this program

    @details
        See the Elements documentation for more details.
    @return
        An  ArgumentParser.
    """
    description = """
            Plots views of the multi-dimensional posterior. Note that the
            axes collapse is done as simple addition. For more correct (Bayesian)
            axes collapse, see the 1D PDF output of PhosphorosComputeRedshifts.
            """

    parser = argparse.ArgumentParser(description=description)

    parser.add_argument("-f", "--file", type=str, required=True,
                        help="FITS file containing the posterior grid")
    parser.add_argument("-p", "--plot-type", type=str, required=True,
                        choices=["Ebv", "Sed", "Z", "Sed-Z", "Ebv-Z", "Sed-Ebv", "Sed-Ebv-Z"],
                        help="The type of plot to show")

    return parser


def mainMethod(args):
    """
    @brief The "main" method.
    @details
        This method is the entry point to the program. In this sense, it is
        similar to a main (and it is why it is called mainMethod()).
    """
    func_name = 'plot' + args.plot_type.replace('-', '')
    if hasattr(plt, func_name):
        getattr(plt, func_name)(args.file)
    else:
        logger.critical("No such plot type (" + args.plot_type + ")")
