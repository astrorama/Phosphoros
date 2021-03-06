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
# Author(s) Nikolaos Apostolakos
#           Nicolas Morisset
from __future__ import absolute_import, division, print_function

import argparse
import os
from builtins import zip

import numpy as np
from astropy.table import Table
from scipy.interpolate import interp1d

from ElementsKernel import Logging

logger = Logging.getLogger('PhosphorosOrderSeds')


def readSed(filename):
    """
     read a SED file and return a Table or None.
     In addition, the function checks if the file is a SED 'file type'.
     It means:
     - it expects 2 columns with only float data.
     - It also reject any ".*" filename.

    """
    logger.info('Reading file: %s', filename)
    # Check the file is a SED file
    # reject all ".*" files
    # reject all empty file
    file_split = filename.split(os.sep)
    if not file_split[len(file_split) - 1].find('.') or \
            not os.stat(filename).st_size or \
            not os.path.isfile(filename):
        return None
    # Read file into table
    try:
        sed = Table.read(filename, format='ascii')
    except ValueError:
        return None

    # Check we have numbers at the first 2 rows
    if not np.issubdtype(sed[0][0].dtype, np.number) or \
            not np.issubdtype(sed[1][0].dtype, np.number):
        if not file_split[len(file_split) - 1] == 'order.txt':
            logger.warn('No float values, file rejected : ', filename)
        return None

    convert = lambda tab: list(zip(tab.columns[0], tab.columns[1]))
    return convert(sed)


def readDirectory(directory):
    """
     Read all files in the directory and put any SED data file into a
     dict container and return this container
    """
    # Check it is a directory
    logger.info('Reading directory : %s', directory)
    if not os.path.isdir(directory):
        raise NotADirectoryError('{} is not a directory! program aborted!'.format(directory))
    sed_dict = {}
    for f in os.listdir(directory):
        full_name = directory + os.sep + f
        sed = readSed(full_name)
        if sed is not None:
            sed_dict[f] = sed
    return sed_dict


def reSampledSeds(seds, xmin=None, xmax=None):
    """
    This function resamples all the SEDs. To do that it looks for
    the minimum and maximum values.
    The minimum is in fact the maximum value of all minimum SED values and the
    maximum is the minimum of all maximum SED values. A step is defined and all
    SEDs will be resampled by using a linear interpolation.
    """
    xmin = xmin if xmin else max([s[0][0] for s in seds.values()])
    xmax = xmax if xmax else min([s[-1][0] for s in seds.values()])
    if xmin >= xmax:
        raise Exception('Non overlapping SED templates')
    steps = max(sum(x >= xmin and x <= xmax for x, _ in s) for s in seds.values())
    xnew = np.linspace(xmin, xmax, steps)
    result = {}
    for s in seds.items():
        x = [x for x, _ in s[1]]
        y = [y for _, y in s[1]]
        f = interp1d(x, y, kind='linear')
        result[s[0]] = list(zip(xnew, f(xnew)))
    return result


def chi2(s1, s2):
    """
     Compute the Chi spare between two SEDs
    """
    if s1 == s2:
        return 0.
    for v1, v2 in list(zip(s1, s2)):
        if v1[0] != v2[0]:
            raise Exception("Incompatible wavelengths")
    a_up = 0
    a_down = 0
    for v1, v2 in list(zip(s1, s2)):
        a_up = a_up + v1[1] * v2[1]
        a_down = a_down + v1[1] * v1[1]
    alpha = a_up / a_down
    res = 0.
    for v1, v2 in list(zip(s1, s2)):
        res = res + (alpha * v1[1] - v2[1]) * (alpha * v1[1] - v2[1])
    return res


def getDistances(seds):
    """
     Compute distance between SEDs using Chi2
    """
    distances = {}
    names = list(seds.keys())
    for n in names:
        distances[n] = {}
    for i in range(len(names) - 1):
        s1 = names[i]
        for s2 in names[i + 1:]:
            d = chi2(seds[s1], seds[s2])
            distances[s1][s2] = d
            distances[s2][s1] = d
    return distances


def findBiggestDiff(distances):
    """
     Look for the two SEDs which have the biggest distance
    """
    result = (None, 0.)
    names = list(distances.keys())
    for i in range(len(names) - 1):
        s1 = names[i]
        for s2 in names[i + 1:]:
            d = distances[s1][s2]
            if result[1] < d:
                result = ((s1, s2), d)
    return result


def orderSeds(seds, start=None, distances=None):
    """
     Ordering the SEDs
    """
    if not distances:
        distances = getDistances(seds)
    if not start:
        s1, s2 = findBiggestDiff(distances)[0]
        s1_sum = sum([y for _, y in seds[s1]])
        s2_sum = sum([y for _, y in seds[s2]])
        start = s1 if s1_sum > s2_sum else s2
    ordered = [start]
    while len(ordered) != len(seds):
        next = min([(s, d) for s, d in distances[ordered[-1]].items() if s not in ordered], key=lambda x: x[1])[0]
        ordered.append(next)

    return ordered


def writeOnDisk(seds, dir):
    """
     Write the ordered SEDs in an ASCII file named : order.txt, at the location
     of the SEDs (location provided by the user)

    """
    out_filename = dir + os.sep + 'order.txt'
    logger.info('Writing file : %s', out_filename)
    output_file = open(out_filename, 'w')
    [output_file.write(s + '\n') for s in seds]
    output_file.close()


def manageOrderInDir(directory, start=None, resample=False):
    """
     Main function for reading SED files, resampling them, ordering
     them and storing them in the <order.txt> file.
    """

    sed_dict = readDirectory(directory)
    if resample:
        seds = reSampledSeds(sed_dict, xmin=None, xmax=None)
    seds_ordered = orderSeds(seds, start)
    writeOnDisk(seds_ordered, directory)


def defineSpecificProgramOptions():
    """
    @brief Allows to define the (command line and configuration file) options
    specific to this program

    @details
        See the Elements documentation for more details.
    @return
        An  ArgumentParser.
    """
    usage = """
            This script automatically orders the SEDs and writes the ordered SED
            names into an <order.txt> file at the SEDs location provided by the
            user.
            """

    parser = argparse.ArgumentParser(description=usage)

    parser.add_argument("-d", "--directory", type=str, required=True,
                        help="Directory of the SEDs location")
    parser.add_argument("-s", "--start", type=str,
                        help="First SED file")

    return parser


def mainMethod(args):
    """
    @brief The "main" method.
    @details
        This method is the entry point to the program. In this sense, it is
        similar to a main (and it is why it is called mainMethod()).
    """
    manageOrderInDir(args.directory, start=args.start, resample=True)
