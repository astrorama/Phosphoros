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

""" test module for the InterpolateSED"""

import unittest
import numpy as np
import astropy.table as table
from SEDInterpolation import InterpolateSED


class InterpolateSEDTestCase(unittest.TestCase):
    """
    Test case for the InterpolateSED 
    """

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)
   
    ####################################     
        
    def testFormatPP(self):
        # WHEN
        result = InterpolateSED.format_pp("Name", 1.5, 0.2, "U")
        
        # THEN
        assert result == "Name=1.5*L+0.2[U]"
        
    def testParsePP(self):
        # HAVING
         pps=["AGE=0*L+5[GY]","MASS=2*L+0[M0]","TEST=3*L+2[TT]","MASS2 = 2.1 *L + 7 [ M0 ]"]
         expected_name = ["AGE","MASS","TEST","MASS2"]
         expected_a = [0, 2, 3, 2.1]
         expected_b = [5, 0, 2, 7]
         expected_unit = ["GY", "M0", "TT", "M0"]
         
         # WHEN
         for index in range(len(pps)):
            name, A, B, unit = InterpolateSED.parse_pp(pps[index]) 
            
            # THEN
            assert expected_name[index] == name
            assert expected_a[index] == A
            assert expected_b[index] == B
            assert expected_unit[index] == unit

    def testCleanName(self):
        # HAVING
        full_names = ["/some/path/file.dot", "./relative/path/file.dot", "path//file.d1.d2", "file.d2.d1.dot","path/file","file"]
        expected_name = ["file", "file", "file.d1", "file.d2.d1","file","file"]
        
        # WHEN
        cleaned_name = [InterpolateSED.clean_name(name) for name in full_names]

        # THEN
        for index in range(len(full_names)):
            assert expected_name[index] == cleaned_name[index]
            
    def testBuildName(self):
        # WHEN
        result1 = InterpolateSED.build_name("Name1.sed","Name2.sed", 0, 2)
        result2 = InterpolateSED.build_name("Name1.sed","Name2.sed", 1, 2)
        
        # THEN
        assert result1 == "2:3_Name1_+_1:3_Name2.sed"
        assert result2 == "1:3_Name1_+_2:3_Name2.sed"
    
    def testDoInterpolatePp(self):
        # HAVING
        pp_1=["AGE=0*L+5[GY]","MASS=2*L+0[M0]","TEST=3*L+2[TT]","MASS2=2*L+0[M0]"]
        pp_2=["AGE=0*L+7[GY]","MASS=4*L+0[M0]","TEST2=5*L+1[TT]","MASS2=2*L+0[M_0]"]
        
        # WHEN
        new_pps = InterpolateSED.do_interpolate_pp(pp_1, pp_2, 0, 3)
         
        
        # THEN
        assert len(new_pps) == 2
        assert "AGE=0.0*L+5.5[GY]" in new_pps
        assert "MASS=2.5*L+0.0[M0]" in new_pps
         
         
        # WHEN
        new_pps = InterpolateSED.do_interpolate_pp(pp_1, pp_2, 1, 3)
         
        # THEN
        assert len(new_pps) == 2
        assert "AGE=0.0*L+6.0[GY]" in new_pps
        assert "MASS=3.0*L+0.0[M0]" in new_pps
        
        
        # WHEN
        new_pps = InterpolateSED.do_interpolate_pp(pp_1, pp_2, 2, 3)
         
        # THEN
        assert len(new_pps) == 2
        assert "AGE=0.0*L+6.5[GY]" in new_pps
        assert "MASS=3.5*L+0.0[M0]" in new_pps
        
    def testDoInterpolateSed(self):
        # HAVING
        sampling = np.array([1,2,3,4,5,6,7,8,9,10])
        values_1= np.array([1,2,3,4,5,6,7,8,9,10])
        values_2= np.array([100,90,80,70,60,50,40,30,20,10])
        
        t_1 = table.Table()
        t_1['Wave']=sampling
        t_1['Flux']=values_1
        
        t_2 = table.Table()
        t_2['Wave']=sampling
        t_2['Flux']=values_2
        
        # WHEN
        interpolated = InterpolateSED.do_interpolate_sed(t_1, t_2, 0, 3)
        expected = 0.75*values_1+0.25*values_2
        
        # THEN
        assert np.array_equal(sampling, interpolated['Wave'])
        assert np.array_equal(expected, interpolated['Flux'])

    def testGetSampling(self):
        # HAVING
        sampling_1 = np.array([1,2,3,5,7,9.1])
        values_1= np.array([1,2,3,4,5,6])
        t_1 = table.Table()
        t_1['Wave']=sampling_1
        t_1['Flux']=values_1
        
        sampling_2 = np.array([3,4,5,6,7,8,9,10,11,12,13])
        values_2= np.array([1,2,3,4,5,6,7,8,9,10,11])
        t_2 = table.Table()
        t_2['Wave']=sampling_2
        t_2['Flux']=values_2
        
        # WHEN
        (before, common, after) = InterpolateSED.get_sampling(t_1, t_2)
        
        # THEN
        assert np.array_equal([1, 2], before)
        assert np.array_equal([10,11,12,13], after)
        assert np.array_equal([3,4,5,6,7,8,9], common)

    def testResample(self):
        # HAVING
        sampling_1 = np.array([1,2,3,5,7,9,11])
        values_1 =  np.array([10,20,30,50,70,90,110])
        sampling_2 = np.array([3, 5, 7, 9,11,12, 13,14])
        values_2 =  np.array([30,50,70,90,110,120,130, 140])
        sampling_before = np.array([1,2])
        common = np.array([3,4,5,6,7,8,9,10,11])
        sampling_after = np.array([12,13,14])
        t_1 = table.Table()
        t_1['Wave'] = sampling_1
        t_1['Flux'] = values_1
        
        t_2 = table.Table()
        t_2['Wave'] = sampling_2
        t_2['Flux'] = values_2
        
        # WHEN
        r_1 = InterpolateSED.resample(t_1, [sampling_before, common, sampling_after]) 
        r_2 = InterpolateSED.resample(t_2, [sampling_before, common, sampling_after])   
                
        # THEN
        assert np.array_equal(r_1['Wave'], [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14])
        assert np.array_equal(r_1['Flux'], [10,20,30,40,50,60,70,80,90,100,110, 0,  0,  0])
        
        assert np.array_equal(r_2['Wave'], [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14])
        assert np.array_equal(r_2['Flux'], [0, 0,30,40,50,60,70,80,90,100,110,120,130,140])
              
    def testResampleEmpty(self):
        # HAVING
        sampling_1 = np.array([1,2,3,5,7,9,11])
        values_1 =  np.array([10,20,30,50,70,90,110])
        values_2 =  np.array([1,2,3,5,7,9,11])
        
        sampling_before = np.array([])
        common = np.array([1,2,3,5,7,9,11])
        sampling_after = np.array([])
        t_1 = table.Table()
        t_1['Wave'] = sampling_1
        t_1['Flux'] = values_1
        
        t_2 = table.Table()
        t_2['Wave'] = sampling_1
        t_2['Flux'] = values_2
        
        # WHEN
        r_1 = InterpolateSED.resample(t_1, [sampling_before, common, sampling_after]) 
        r_2 = InterpolateSED.resample(t_2, [sampling_before, common, sampling_after])   
        
        # THEN
        assert np.array_equal(r_1['Wave'], sampling_1)
        assert np.array_equal(r_1['Flux'], values_1)
        
        assert np.array_equal(r_2['Wave'], sampling_1)
        assert np.array_equal(r_2['Flux'], values_2)
        
        
        
if __name__ == '__main__':
    unittest.main()

