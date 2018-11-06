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
File: python/GalacticDustMap/GalacticDustMap.py

Created on: 11/06/18
Author: fdubath

Ported from A.GALAMETZ IDL script 'ebv_planck' and the functions 
it use in the Healpix lib.   
"""

from __future__ import division, print_function
from astropy.io import fits
import math as math
import numpy as np
import sys
if sys.version_info[0] < 3:
    from future_builtins import *
    
    
def init_xy2pix():
#************************************************************************
#
#     sets the array giving the number of the pixel lying in (x,y)
#     x and y are in {1,128}
#     the pixel number is in {0,128**2-1}
#
#     if  i-1 = sum_p=0  b_p * 2^p
#     then ix = sum_p=0  b_p * 4^p
#          iy = 2*ix
#     ix + iy in {0, 128**2 -1}
#-
#************************************************************************
    x2pix = np.zeros(128)
    

    for i in range(128):        #for converting x,y into pixel numbers
        j  = i                
        k  = 0
        ip = 1
        while j>0:
            id = j % 2 
            j=int(j/2)
            k+=ip*id
            ip*=4
        x2pix[i] = k
      
    y2pix = 2 * x2pix
    return x2pix.astype(int), y2pix.astype(int)
    
    
    
def ang2pix_nest(nside, theta, phi):
    theta=np.atleast_1d(theta)
    phi=np.atleast_1d(phi)
    
    ns_max = 8192
    if nside<1 or nside>ns_max:
        raise ValueError('nside out of range')
        
    np0=len(theta)
    np1=len(phi)
    if np0!=np1:
        raise ValueError('inconsistent theta and phi in ang2pix_nest')
        
    if np.min(theta)<0 or np.max(theta)>math.pi:
        raise ValueError('theta out of range')
#------------------------------------------------------------
    x2pix, y2pix = init_xy2pix()
    twopi  = 2.*math.pi
    piover2 = 0.5*math.pi
    z = np.cos(theta)
    z0 = 2. /3.
    phi_in = phi % twopi
    mask_phi = phi_in<0.
    phi_in += twopi*mask_phi
    tt  = phi_in / piover2 # in [0,4[

    pix_eqt = (z <= z0) & (z > -z0) # equatorial strip
    pix_pol = (z > z0) | (z <= -z0) # polar caps


    ## Computation for the Equatorial strip:
    # (the index of edge lines increase when the longitude=phi goes up)
    jp = (ns_max*(0.5+ tt - z*0.75)).astype(int)#  ascend edge line index
    jm = (ns_max*(0.5 + tt + z*0.75)).astype(int)# descend edge line index

    # finds the face and pixel's (x,y)
    ifp = (jp / ns_max).astype(int) # in {0,4}
    ifm = (jm / ns_max).astype(int)
    p_np = ifp == ifm
    p_eq = ifp < ifm
    p_sp = ifp > ifm

    face_n = (ifp%4+4)*p_np + (ifp%4)*p_eq + (ifm%4 +8)*p_sp
    ix_st = jm % ns_max
    iy_st = ns_max - (jp % ns_max) - 1

    ## Computation for the polar caps:
    ntt = np.minimum(tt.astype(int), 3)
    tp = tt - ntt
    tmp = np.sqrt( 3.*(1. - np.abs(z)))  # in ]0,1]
    #(the index of edge lines increase when distance from the closest pole goes up)
    jp = ( ns_max * tp * tmp ).astype(int) # line going toward the pole as phi increases
    jm = ( ns_max * (1.- tp) * tmp ).astype(int) # that one goes away of the closest pole
    jp = np.minimum(jp, ns_max-1) # for points too close to the boundary
    jm = np.minimum(jm, ns_max-1)

    # finds the face and pixel's (x,y)
    p_np = z > 0.
    p_sp = z < 0.
    face_cap = ntt * p_np + (ntt+8) * p_sp
    ix_cap = (ns_max - jm - 1)*p_np + (jp)*p_sp
    iy_cap = (ns_max - jp - 1)*p_np + (jm)*p_sp
    
    
    ## build global Face and Pixel's (x,y)
    face_num = face_n * pix_eqt + face_cap * pix_pol
    
    ix = ix_st * pix_eqt + ix_cap * pix_pol
    iy = iy_st * pix_eqt + iy_cap * pix_pol

    ix_low = (ix % 128).astype(int)
    ix_hi  = (ix/128).astype(int)
    iy_low = (iy % 128).astype(int)
    iy_hi  = (iy/128).astype(int)

    ipix =  (x2pix[ix_hi] + y2pix[iy_hi])*16384 + (x2pix[ix_low] + y2pix[iy_low])
    ipix = (ipix / (int( ns_max/nside )**2)).astype(int)  # in {0, nside**2 - 1}
    ipix = ipix + face_num*(nside**2)  # in {0, 12*nside**2 - 1}

    return ipix
 
 
def raDecToLB(ra,dec):
    ra=np.atleast_1d(ra)
    dec=np.atleast_1d(dec)
    
    radeg = 180.0/math.pi        # Conversion to radians                                            
    ras = ra / radeg
    decs = dec /radeg
    rapol = (12 + 51.4/60.0) * 15.0   # RA of North Pole                                         
    rapol = rapol / radeg
    decpol = 27.0 + 77.0/600.0       # Dec of North Pole                                             
    decpol = decpol / radeg
    sdecs = np.sin(decs)
    cdecs = np.cos(decs)
    sdecpol = np.sin(decpol)
    cdecpol = np.cos(decpol)
    b1 = sdecs * sdecpol + cdecs*cdecpol*np.cos(ras-rapol)
    b = np.arcsin(b1) * radeg
    cb = np.cos(b/radeg)
    racen = (17 + 45.6/60.0) * 15.0    # RA of Galactic Center                                    
    racen = racen / radeg
    deccen = np.arctan(-np.cos(racen-rapol)/np.tan(decpol)) # Dec Galactic Center = -28.929 656 ...    
    sdeccen = np.sin(deccen)
    j = (((sdecs*cdecpol)-(cdecs*sdecpol*np.cos(ras-rapol))) / cb) * radeg
    k = np.arcsin((cdecs*np.sin(ras-rapol)) / cb) * radeg
    q = np.arccos(sdeccen/cdecpol) * radeg
    mask_j = j < 0
    l = (q + k - 180.0)*mask_j + (q - k)*np.logical_not(mask_j)
    mask_l = l < 0
    l = (l + 360)*mask_l + l*np.logical_not(mask_l)
    return (l,b)
    
def loadMap(dustmap_filename):
    with fits.open(dustmap_filename) as hdul:  # open a FITS file
        data = hdul[0].data
    
def ebv_planck(data,ra,dec):
    (l,b)=raDecToLB(ra,dec)
    phi = l / (180./math.pi)
    theta = (90 - b) / (180./math.pi)
    ipix = ang2pix_nest(2048,theta,phi)
    return data[ipix]
