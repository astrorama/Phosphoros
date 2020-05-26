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
File: python/DataManagement/UpdateDataPack.py

Created on: 05/25/2020
Author: dubathf
"""

from __future__ import division, print_function


import argparse
import os
import astropy.table as table
import numpy as np
import ElementsKernel.Logging as log
import urllib.request
import requests
import sys
import tarfile
import shutil
import hashlib
from glob import glob
from pathlib import Path


logger = log.getLogger('UpdateDataPack')
data_dir = str(Path(os.getenv('PHOSPHOROS_ROOT', '~/Phosphoros') + '/AuxiliaryData/').expanduser())+'/'


def md5(fname):
    hash_md5 = hashlib.md5()
    with open(fname, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()
    

def defineSpecificProgramOptions():
    
    parser = argparse.ArgumentParser()

    parser.add_argument('--repo-url', default='<tobedefined>.unige.ch', type=str,
        help='URL of the Data Package server')
        
    parser.add_argument('--temp-folder', default='./temp', type=str,
        help='Where to download and untar the data pack')

    return parser
    
    
def checkVersions(url):
    version_local = '0.0'
    if os.path.exists(data_dir+"version.txt"):
        with open(data_dir+"version.txt") as f:
            lines = f.readlines()
            version_local=lines[0]
            
    version_remote = urllib.request.urlopen(url).read().decode("utf-8") 
    
    logger.info('Local Version = '+version_local)
    logger.info('Remote Version = '+version_remote)
    return version_local, version_remote
    
def downloadVersion(temp, url, version):
    if not os.path.exists(temp):
        os.makedirs(temp)
    full_url = url+'?version='+version
    r = requests.get(full_url,verify=False,stream=True)
    r.raw.decode_content = True
    with open(temp+'/downloaded.tar.xz', 'wb') as f:
        shutil.copyfileobj(r.raw, f)
    tar = tarfile.open(temp+'/downloaded.tar.xz')
    tar.extractall(path=temp)
    tar.close()
    os.remove(temp+'/downloaded.tar.xz')
    logger.info('Version = '+version+' dowloaded and uncompressed.')
    
def cleanTempDir(temp):
    shutil.rmtree(temp)
               
def listFiles(base_dir_new):
    new_files = {}
    for folder,sub_folder,file_name in os.walk(base_dir_new):
        for fname in file_name:
            new_files[folder+'/'+fname]=md5(folder+'/'+fname)
    conflicting = {}
    unchanged = {}
    new_data = {}
    for path, f_md5 in new_files.items():
        old_path = path.replace(base_dir_new, data_dir)
        if os.path.exists(old_path):
            old_md5=md5(old_path)
            if f_md5==old_md5:
                unchanged[path]= old_path
            else:
                conflicting[path]= old_path
        else:
            new_data[path]= old_path
    return new_data, unchanged, conflicting 

def importFiles(file_dict):
    for path in file_dict.keys():
        logger.info('Importing file: '+file_dict[path])
        shutil.copy(path, file_dict[path])


def mainMethod(args):
    logger.info('The Phosphoros AuxiliaryData directory is set to :' +data_dir)
    version_local, version_remote = checkVersions(args.repo_url)
    
    if version_remote!=version_local:
        user_input = input("The version "+str(version_remote)+" of the data package is available, do you want to download it (y/n)") 
        if user_input=='y':
            downloadVersion(args.temp_folder, args.repo_url, version_remote)
            base_dir_new = args.temp_folder+'/AuxiliaryData/'
            new_data, unchanged, conflicting = listFiles(base_dir_new)
            logger.info('In Version = '+version_remote + ' there is '+str(len(new_data) + len(unchanged) + len(conflicting))+' files')
            logger.info('In which '+str(len(new_data))+' are new, '+str(len(unchanged))+' are unchanged and '+str(len(conflicting))+' are conflicting.')
            
            if len(new_data)>0:
                logger.info('Importing new files')
                importFiles(new_data)
            
          
            if len(conflicting)>0:
                logger.info('Handling conflicting files')
                keys = list(conflicting.keys()).copy()
                
                for key in keys:
                    short = key.replace(base_dir_new, './')
                    user_input = input(short+" is in your data and is different in the version "+str(version_remote)+" (k-keep, r-replace,  l-keep all, e-replace all)") 
                    if user_input=='k':
                        del conflicting[key]
                    if user_input=='l' or user_input=='e':
                        break
                if user_input!='l' :
                
                    logger.info('Overwriting files')
                    importFiles(conflicting)
            
            
            cleanTempDir(args.temp_folder+'/AuxiliaryData')
            
                
            
            
        

    
        
    
    
   
