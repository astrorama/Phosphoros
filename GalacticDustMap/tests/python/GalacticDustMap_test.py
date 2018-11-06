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
File: tests/python/GalacticDustMap_test.py

Created on: 11/06/18
Author: fdubath
"""
from __future__ import division, print_function
import sys
if sys.version_info[0] < 3:
    from future_builtins import *

import py.test
import GalacticDustMap.GalacticDustMap as GalacticDustMap
import numpy as np
import math as math

class TestGalacticDustMap(object):
    """
    @class TestGalacticDustMap
    @brief Unit Test class
    
    All expected values where computed by the IDL code from which 
    GalacticDustMap.py has been ported. These test just check that the 2 codes 
    provide the same results.
    """
    def test_init_xy2pix(self):
        # Given
        x2pix_expected = [0,1,4,5,16,17,20,21,64,65,68,69,80,81,84,85,256,257,  
        260,261,272,273,276,277,320,321,324,325,336,337,340,341,1024,1025,1028,
        1029,1040,1041,1044,1045,1088,1089,1092,1093,1104,1105,1108,1109,1280, 
        1281,1284,1285,1296,1297,1300,1301,1344,1345,1348,1349,1360,1361,1364,
        1365,4096,4097,4100,4101,4112,4113,4116,4117,4160,4161,
        4164,4165,4176,4177,4180,4181,4352,4353,4356,4357,4368,4369,4372,4373,
        4416,4417,4420,4421,4432,4433,4436,4437,5120,5121,5124,
        5125,5136,5137,5140,5141,5184,5185,5188,5189,5200,5201,5204,5205,
        5376,5377,5380,5381,5392,5393,5396,5397,5440,5441,5444,5445,5456,5457,5460,5461]
        
        y2pix_expected = [0,2,8,10,32,34,40,42, 128, 130, 136, 138, 160, 162, 168, 
        170, 512, 514, 520, 522, 544, 546, 552, 554, 640, 642, 648, 650, 672, 674, 
        680, 682,2048,2050,2056,2058,2080,2082,2088,2090,2176,2178,2184,2186,2208,2210,
        2216,2218,2560,2562,2568,2570,2592,2594,2600,2602,2688,2690,2696,2698,2720,
        2722,2728,2730,8192,8194,8200,8202,8224,8226,8232,8234,8320,8322,8328,
        8330,8352,8354,8360,8362,8704,8706,8712,8714,8736,8738,8744,8746,8832,
        8834,8840,8842,8864,8866,8872,8874,10240,10242,10248,10250,10272,10274,
        10280,10282,10368,10370,10376,10378,10400,10402,10408,10410,10752,10754,
        10760,10762,10784,10786,10792,10794,10880,10882,10888,10890,10912,10914,10920,10922]
        
        # WHEN
        x2pix, y2pix = GalacticDustMap.init_xy2pix()
        
        # THEN
        assert len(x2pix_expected)==len(x2pix), "Length of x2pix do not match"
        assert len(y2pix_expected)==len(y2pix), "Length of y2pix do not match"
        for i in range(len(x2pix_expected)):
            assert x2pix_expected[i]==x2pix[i], 'Element number '+str(i)+ ' of x2pix do not match'
            assert y2pix_expected[i]==y2pix[i], 'Element number '+str(i)+ ' of y2pix do not match'
        
    def test_ang2pix_nest(self):
        # GIVEN        
        nside=2048
        theta = np.array([0,180,20,40,60,80,90,110,130,150,170,
                          20,40,60,80,90,110,130,150,170,
                          20,40,60,80,90,110,130,150,170])*math.pi/180
        phi   = np.array([0, 0,30,30,30,30,30,30, 30, 30, 30, 
                          90,90,90,90,90,90, 90, 90, 90,
                          360,360,360,360,360,360,360,360,360]) 
        ipix_expected = [4194303,33554432,16440540,15431481,33380969,32600336,31087209,29773264,48920284,48284168
,46302413, 7990252, 7147573, 6396249,22912224,22452569,41587488,40296492,38519304,37898763
, 8072512, 7135286, 6460821,22987275,22648213,21315275,40487399,39869553,37894434] 

        # WHEN
        ipix = GalacticDustMap.ang2pix_nest(nside,theta,phi) 
        
        # THEN
        assert len(ipix_expected)==len(ipix), "Length of ipix do not match"
        for i in range(len(ipix_expected)):
            assert ipix_expected[i]==ipix[i], 'Element number '+str(i)+ ' of ipix do not match'
        
        
    def test_radetolb(self): 
        # GIVEN  
        ra = [  0, 90,180, 30,60,90,120,150,60, 192.84,  266.40]
        dec= [-90,-60,-30,  0,10,20, 30, 60,90,27.1283, -28.929]   
        
        l_expected=[302.92431,268.82596,289.85989,157.01316,180.42564,189.35432,
        191.26976,152.31915,122.92431,213.13837,0.000560166339596]
        b_expected=[-27.128333,-29.594671,31.564374,-58.256844,-31.123705,
        -1.7214120,27.082141,46.148339,27.128333,89.991096,0.0003419167118] 
        
        # WHEN 
        l,b =GalacticDustMap.raDecToLB(ra,dec)
        
        # THEN 
        assert len(l_expected)==len(l), 'Length of l do not match'
        assert len(b_expected)==len(b), 'Length of b do not match'
        for i in range(len(l_expected)):
            assert abs(abs(l_expected[i]-l[i])/l_expected[i])<0.001, 'Element number '+str(i)+ ' of l do not match' 
            assert abs(abs(b_expected[i]-b[i])/b_expected[i])<0.001, 'Element number '+str(i)+ ' of b do not match' 
        
 
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
