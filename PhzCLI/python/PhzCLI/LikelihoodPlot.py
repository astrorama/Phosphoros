
from __future__ import absolute_import, division, print_function
import numpy as np
import collections
from astropy.io import fits
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d.axes3d import Axes3D
from scipy.interpolate import interp1d


class Region:
    def __init__(self, name, data, sed_axis, redcurve_axis, ebv_axis, z_axis):
        self.name = name
        self.data = data
        self.sed_axis = sed_axis
        self.redcurve_axis = redcurve_axis
        self.ebv_axis = ebv_axis
        self.z_axis = z_axis


def getRegions(file):
    hdulist = fits.open(file)
    regions = []
    for i in range((len(hdulist)-1)//5):
        regions.append(Region(hdulist[5*i+1].header['EXTNAME'], np.exp(hdulist[5*i+1].data), hdulist[5*i+5].data, hdulist[5*i+4].data, hdulist[5*i+3].data, hdulist[5*i+2].data))
    return regions


def plotSed(file):
    regions = getRegions(file)
    results = collections.OrderedDict()
    for r in regions:
        for sed, name in r.sed_axis:
            value = 0
            for redcurve in r.redcurve_axis['Index']:
                 for ebv in r.ebv_axis['Index']:
                      for z in r.z_axis['Index']:
                          value += r.data[sed,redcurve,ebv,z]
            if not name in results:
                results[name] = 0;
            results[name] += value;
    x = []
    y = []
    x_labels = []
    for k, v in results.items():
        x.append(len(x))
        x_labels.append(k)
        y.append(v)
    fig, ax = plt.subplots()
    ax.bar(x, y, align='center')
    ax.set_xticks(x)
    ax.set_xticklabels(x_labels, rotation='vertical')
    plt.tight_layout()
    plt.show()
    

def plotEbv(file):
    regions = getRegions(file)
    results = collections.OrderedDict()
    all_x = set()
    func_list = []
    for r in regions:
        x = []
        y = []
        for ebv, ebv_value in r.ebv_axis:
            ebv_value = float(ebv_value)
            all_x.add(ebv_value)
            x.append(ebv_value)
            value = 0
            for sed in r.sed_axis['Index']:
                for redcurve in r.redcurve_axis['Index']:
                    for z in r.z_axis['Index']:
                        value += r.data[sed,redcurve,ebv,z] 
            y.append(value)
        func_list.append(interp1d(x, y, kind='linear'))
    plot_x = []
    plot_y = []
    for x in sorted(all_x):
        value = 0
        for f in func_list:
            if x >= f.x[0] and x <= f.x[-1]:
                value += f(x)
        plot_x.append(x)
        plot_y.append(value)
    plt.plot(plot_x, plot_y)
    plt.show()
    

def plotZ(file):
    regions = getRegions(file)
    results = collections.OrderedDict()
    all_x = set()
    func_list = []
    for r in regions:
        x = []
        y = []
        for z, z_value in r.z_axis:
            z_value = float(z_value)
            all_x.add(z_value)
            x.append(z_value)
            value = 0
            for sed in r.sed_axis['Index']:
                for redcurve in r.redcurve_axis['Index']:
                    for ebv in r.ebv_axis['Index']:
                        value += r.data[sed,redcurve,ebv,z] 
            y.append(value)
        func_list.append(interp1d(x, y, kind='linear'))
    plot_x = []
    plot_y = []
    for x in sorted(all_x):
        value = 0
        for f in func_list:
            if x >= f.x[0] and x <= f.x[-1]:
                value += f(x)
        plot_x.append(x)
        plot_y.append(value)
    plt.plot(plot_x, plot_y)
    plt.show()


def plotSedZ(file):
    regions = getRegions(file)
    sed_list = []
    z_list = set()
    for r in regions:
        for sed in r.sed_axis['Value']:
            if not sed in sed_list:
                sed_list.append(sed)
        for z in r.z_axis['Value']:
            z_list.add(float(z))
    z_list = sorted(z_list)
    region_sed_search = {}
    region_z_search = {}
    for r in regions:
        region_sed_search[r.name] = {}
        for i, sed in r.sed_axis:
            region_sed_search[r.name][sed] = int(i)
        region_z_search[r.name] = {}
        for i, z in r.z_axis:
            region_z_search[r.name][float(z)] = int(i)
    X, Y = np.meshgrid(range(len(sed_list)), z_list)
    Z = []
    for z in z_list:
        ar = []
        for sed in sed_list:
            value = 0
            for r in regions:
                if not sed in region_sed_search[r.name] or not z in region_z_search[r.name]:
                    continue
                sed_i = region_sed_search[r.name][sed]
                z_i = region_z_search[r.name][z]
                for redcurve_i in r.redcurve_axis['Index']:
                    for ebv_i in r.ebv_axis['Index']:
                        value += r.data[sed_i, redcurve_i, ebv_i, z_i] 
            ar.append(value)
        Z.append(ar)
    Z = np.array(Z)
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(1, 1, 1, projection='3d')
    p = ax.plot_wireframe(X, Y, Z)
    ax.set_xlabel('SED')
    ax.get_xaxis().set_ticks(range(len(sed_list)))
    ax.get_xaxis().set_ticklabels(sed_list, rotation='vertical')
    ax.set_ylabel('Z')
    plt.show()


def plotSedEbv(file):
    regions = getRegions(file)
    sed_list = []
    ebv_list = set()
    for r in regions:
        for sed in r.sed_axis['Value']:
            if not sed in sed_list:
                sed_list.append(sed)
        for ebv in r.ebv_axis['Value']:
            ebv_list.add(float(ebv))
    ebv_list = sorted(ebv_list)
    region_sed_search = {}
    region_ebv_search = {}
    for r in regions:
        region_sed_search[r.name] = {}
        for i, sed in r.sed_axis:
            region_sed_search[r.name][sed] = int(i)
        region_ebv_search[r.name] = {}
        for i, ebv in r.ebv_axis:
            region_ebv_search[r.name][float(ebv)] = int(i)
    X, Y = np.meshgrid(range(len(sed_list)), ebv_list)
    Z = []
    for ebv in ebv_list:
        ar = []
        for sed in sed_list:
            value = 0
            for r in regions:
                if not sed in region_sed_search[r.name] or not ebv in region_ebv_search[r.name]:
                    continue
                sed_i = region_sed_search[r.name][sed]
                ebv_i = region_ebv_search[r.name][ebv]
                for redcurve_i in r.redcurve_axis['Index']:
                    for z_i in r.z_axis['Index']:
                        value += r.data[sed_i, redcurve_i, ebv_i, z_i] 
            ar.append(value)
        Z.append(ar)
    Z = np.array(Z)
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(1, 1, 1, projection='3d')
    p = ax.plot_wireframe(X, Y, Z)
    ax.set_xlabel('SED')
    ax.get_xaxis().set_ticks(range(len(sed_list)))
    ax.get_xaxis().set_ticklabels(sed_list, rotation='vertical')
    ax.set_ylabel('E(B-V)')
    plt.show()


def plotEbvZ(file):
    regions = getRegions(file)
    ebv_list = set()
    z_list = set()
    for r in regions:
        for ebv in r.ebv_axis['Value']:
            ebv_list.add(float(ebv))
        for z in r.z_axis['Value']:
            z_list.add(float(z))
    ebv_list = sorted(ebv_list)
    z_list = sorted(z_list)
    region_ebv_search = {}
    region_z_search = {}
    for r in regions:
        region_ebv_search[r.name] = {}
        for i, ebv in r.ebv_axis:
            region_ebv_search[r.name][float(ebv)] = int(i)
        region_z_search[r.name] = {}
        for i, z in r.z_axis:
            region_z_search[r.name][float(z)] = int(i)
    X, Y = np.meshgrid(ebv_list, z_list)
    Z = []
    for z in z_list:
        ar = []
        for ebv in ebv_list:
            value = 0
            for r in regions:
                if not ebv in region_ebv_search[r.name] or not z in region_z_search[r.name]:
                    continue
                ebv_i = region_ebv_search[r.name][ebv]
                z_i = region_z_search[r.name][z]
                for redcurve_i in r.redcurve_axis['Index']:
                    for sed_i in r.sed_axis['Index']:
                        value += r.data[sed_i, redcurve_i, ebv_i, z_i] 
            ar.append(value)
        Z.append(ar)
    Z = np.array(Z)
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(1, 1, 1, projection='3d')
    p = ax.plot_wireframe(X, Y, Z)
    ax.set_xlabel('E(B-V)')
    ax.set_ylabel('Z')
    plt.show()


def plotSedEbvZ(file):
    regions = getRegions(file)
    sed_list = []
    ebv_list = set()
    z_list = set()
    for r in regions:
        for sed in r.sed_axis['Value']:
            if not sed in sed_list:
                sed_list.append(sed)
        for ebv in r.ebv_axis['Value']:
            ebv_list.add(float(ebv))
        for z in r.z_axis['Value']:
            z_list.add(float(z))
    ebv_list = sorted(ebv_list)
    z_list = sorted(z_list)
    region_sed_search = {}
    region_ebv_search = {}
    region_z_search = {}
    for r in regions:
        region_sed_search[r.name] = {}
        for i, sed in r.sed_axis:
            region_sed_search[r.name][sed] = int(i)
        region_ebv_search[r.name] = {}
        for i, ebv in r.ebv_axis:
            region_ebv_search[r.name][float(ebv)] = int(i)
        region_z_search[r.name] = {}
        for i, z in r.z_axis:
            region_z_search[r.name][float(z)] = int(i)
    max = 0
    for r in regions:
        for sed in r.sed_axis['Index']:
            for redcurve in r.redcurve_axis['Index']:
                 for ebv in r.ebv_axis['Index']:
                      for z in r.z_axis['Index']:
                          if r.data[sed,redcurve,ebv,z] > max:
                            max = r.data[sed,redcurve,ebv,z]
    X = []
    Y = []
    Z = []
    col = []
    for z in z_list:
        for ebv in ebv_list:
            for sed_list_i in range(len(sed_list)):
                X.append(sed_list_i)
                Y.append(ebv)
                Z.append(z)
                value = 0
                sed = sed_list[sed_list_i]
                for r in regions:
                    if not sed in region_sed_search[r.name] or not ebv in region_ebv_search[r.name]or not z in region_z_search[r.name]:
                        continue
                    sed_i = region_sed_search[r.name][sed]
                    ebv_i = region_ebv_search[r.name][ebv]
                    z_i = region_z_search[r.name][z]
                    for redcurve_i in r.redcurve_axis['Index']:
                        value += r.data[sed_i, redcurve_i, ebv_i, z_i] 
                col.append([1, 0, 0, value/max*.5])
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(1, 1, 1, projection='3d')
    p = ax.scatter(X, Y, Z, c=col, depthshade=False, marker='.',linewidths=0)
    ax.set_xlabel('SED')
    ax.set_xlim(0,len(sed_list))
    ax.get_xaxis().set_ticks(range(len(sed_list)))
    ax.get_xaxis().set_ticklabels(sed_list, rotation='vertical')
    ax.set_ylabel('E(B-V)')
    ax.set_zlabel('Z')
    plt.show()


def plotSedEbvZOld(file):
    hdulist = fits.open(file)
    data = hdulist[1].data
    sed_indices = range(len(hdulist[5].data))
    ebv_indices = range(len(hdulist[3].data))
    z_indices = range(len(hdulist[2].data))
    max = 0
    for sed in sed_indices:
        for ebv in ebv_indices:
            for z in z_indices:
                if data[sed,0,ebv,z] > max:
                    max = data[sed,0,ebv,z]
    X = []
    Y = []
    Z = []
    col = []
    for sed in sed_indices:
        for ebv in ebv_indices:
            for z in z_indices:
                #if data[sed,0,ebv,z] > max*.5:
                    X.append(sed)
                    Y.append(ebv)
                    Z.append(z)
                    col.append([1, 0, 0, data[sed,0,ebv,z]/max*.5])
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(1, 1, 1, projection='3d')
    p = ax.scatter(X, Y, Z, c=col, depthshade=False, marker='.',linewidths=0)
    ax.set_xlabel('SED')
    ax.set_xlim(0,len(sed_indices))
    ax.get_xaxis().set_ticks(sed_indices)
    sed_labels = [sed_name for _,sed_name in hdulist[5].data]
    ax.get_xaxis().set_ticklabels(sed_labels, rotation='vertical')
    ax.set_ylabel('E(B-V)')
    ax.set_ylim(0,len(ebv_indices))
    ebv_labels = [float(ebv_value) for _,ebv_value in hdulist[3].data]
    ax.get_yaxis().set_ticks(ebv_indices)
    ax.get_yaxis().set_ticklabels(ebv_labels)
    ax.set_zlabel('Z')
    ax.set_zlim(0, len(z_indices))
    z_ticks = np.linspace(0,len(z_indices),7)
    ax.set_zticks(z_ticks)
    z_labels = z_ticks * 6 / len(z_indices)
    ax.set_zticklabels(z_labels)
    plt.show()
