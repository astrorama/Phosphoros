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
File: python/PhzCLI/TableUtils.py

Created on: 04/11/17
Author: morisset
"""

from __future__ import division, print_function
from future_builtins import *

import os
import astropy.table as table
import numpy as np

def check_file_exists(filename):
    if not os.path.exists(filename):  
       print('ERROR : File', filename, 'not found')
       exit(-1)

def read_table(filename):
    check_file_exists(filename)
    try :
        data = table.Table.read(filename, format='fits')
    except:
        with open(filename) as f:
            all_lines = f.readlines()
        header_lines = [l for l in all_lines if l.strip().startswith('#')]
        comments = [l.replace('#','').strip() for l in header_lines]
        data_lines = [l for l in all_lines if l.strip() != '' and not l.strip().startswith('#')]
        column_no = len(data_lines[0].strip().split())
        header_lines = [l for l in header_lines if len(l.replace('#','').strip().split()) == column_no]
        text = ''.join(header_lines + data_lines)
        data = table.Table.read(text, format='ascii')
        data.meta = {'COMMENT': comments}
        for pdf_col in [col for col in data.colnames if col.endswith('-1D-PDF')]:
            pdf_data = [np.array(col_data.split(',')).astype(np.float) for col_data in data[pdf_col]]
            data.rename_column(pdf_col, pdf_col+'_str')
            data.add_column(table.Column(name=pdf_col, data=pdf_data))
    return data
