import numpy as np
from astropy.io import fits
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d.axes3d import Axes3D

def plotEbv(file):
    hdulist = fits.open(file)
    data = hdulist[0].data
    res = {}
    for ebv in range(len(hdulist[2].data)):
        value = 0
        for sed in range(len(hdulist[4].data)):
            for z in range(len(hdulist[1].data)):
                value = value + data[sed,0,ebv,z]
        res[ebv] = value
    x = []
    y = []
    ebv_values = hdulist[2].data
    for k,v in res.items():
        x.append(ebv_values[k][1])
        y.append(v)
    plt.plot(x,y)
    plt.show()

def plotSed(file):
    hdulist = fits.open(file)
    data = hdulist[0].data
    res = {}
    for sed in range(len(hdulist[4].data)):
        value = 0
        for ebv in range(len(hdulist[2].data)):
            for z in range(len(hdulist[1].data)):
                value = value + data[sed,0,ebv,z]
        res[sed] = value
    x = []
    y = []
    for k,v in res.items():
        x.append(k)
        y.append(v)
    fig, ax = plt.subplots()
    ax.bar(x,y,align='center')
    sed_labels = [sed_name for _,sed_name in hdulist[4].data]
    ax.set_xticks(x)
    ax.set_xticklabels(sed_labels, rotation='vertical')
    plt.tight_layout()
    plt.show()

def plotZ(file):
    hdulist = fits.open(file)
    data = hdulist[0].data
    res = {}
    for z in range(len(hdulist[1].data)):
        value = 0
        for sed in range(len(hdulist[4].data)):
            for ebv in range(len(hdulist[2].data)):
                value = value + data[sed,0,ebv,z]
        res[z] = value
    x = []
    y = []
    z_values = hdulist[1].data
    for k,v in res.items():
        x.append(z_values[k][1])
        y.append(v)
    plt.plot(x,y)
    plt.show()

def plotSedZ(file):
    hdulist = fits.open(file)
    data = hdulist[0].data
    sed_indices = range(len(hdulist[4].data))
    z_indices = range(len(hdulist[1].data))
    X, Y = np.meshgrid(sed_indices, z_indices)
    Z = []
    for z in z_indices:
        ar = []
        for sed in sed_indices:
            value = 0
            for ebv in range(len(hdulist[2].data)):
                value = value + data[sed,0,ebv,z]
            ar.append(value)
        Z.append(ar)
    Z = np.array(Z)
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(1, 1, 1, projection='3d')
    p = ax.plot_wireframe(X, Y, Z)
    ax.set_xlabel('SED')
    ax.get_xaxis().set_ticks(sed_indices)
    sed_labels = [sed_name for _,sed_name in hdulist[4].data]
    ax.get_xaxis().set_ticklabels(sed_labels, rotation='vertical')
    ax.set_ylabel('Z')
    z_ticks = np.linspace(0,len(z_indices),7)
    ax.get_yaxis().set_ticks(z_ticks)
    z_labels = z_ticks * 6 / len(z_indices)
    ax.get_yaxis().set_ticklabels(z_labels)
    plt.show()

def plotEbvZ(file):
    hdulist = fits.open(file)
    data = hdulist[0].data
    ebv_indices = range(len(hdulist[2].data))
    z_indices = range(len(hdulist[1].data))
    X, Y = np.meshgrid(ebv_indices, z_indices)
    Z = []
    for z in z_indices:
        ar = []
        for ebv in ebv_indices:
            value = 0
            for sed in range(len(hdulist[4].data)):
                value = value + data[sed,0,ebv,z]
            ar.append(value)
        Z.append(ar)
    Z = np.array(Z)
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(1, 1, 1, projection='3d')
    p = ax.plot_wireframe(X, Y, Z)
    ax.set_xlabel('E(B-V)')
    ebv_labels = [float(ebv_value) for _,ebv_value in hdulist[2].data]
    ax.get_xaxis().set_ticks(ebv_indices)
    ax.get_xaxis().set_ticklabels(ebv_labels)
    ax.set_ylabel('Z')
    z_ticks = np.linspace(0,len(z_indices),7)
    ax.get_yaxis().set_ticks(z_ticks)
    z_labels = z_ticks * 6 / len(z_indices)
    ax.get_yaxis().set_ticklabels(z_labels)
    plt.show()

def plotSedEbv(file):
    hdulist = fits.open(file)
    data = hdulist[0].data
    sed_indices = range(len(hdulist[4].data))
    ebv_indices = range(len(hdulist[2].data))
    X, Y = np.meshgrid(sed_indices, ebv_indices)
    Z = []
    for ebv in ebv_indices:
        ar = []
        for sed in sed_indices:
            value = 0
            for z in range(len(hdulist[1].data)):
                value = value + data[sed,0,ebv,z]
            ar.append(value)
        Z.append(ar)
    Z = np.array(Z)
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(1, 1, 1, projection='3d')
    p = ax.plot_wireframe(X, Y, Z)
    ax.set_xlabel('SED')
    ax.get_xaxis().set_ticks(sed_indices)
    sed_labels = [sed_name for _,sed_name in hdulist[4].data]
    ax.get_xaxis().set_ticklabels(sed_labels, rotation='vertical')
    ax.set_ylabel('E(B-V)')
    ebv_labels = [float(ebv_value) for _,ebv_value in hdulist[2].data]
    ax.get_yaxis().set_ticks(ebv_indices)
    ax.get_yaxis().set_ticklabels(ebv_labels)
    plt.show()

def plotSedEbvZ(file):
    hdulist = fits.open(file)
    data = hdulist[0].data
    sed_indices = range(len(hdulist[4].data))
    ebv_indices = range(len(hdulist[2].data))
    z_indices = range(len(hdulist[1].data))
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
    sed_labels = [sed_name for _,sed_name in hdulist[4].data]
    ax.get_xaxis().set_ticklabels(sed_labels, rotation='vertical')
    ax.set_ylabel('E(B-V)')
    ax.set_ylim(0,len(ebv_indices))
    ebv_labels = [float(ebv_value) for _,ebv_value in hdulist[2].data]
    ax.get_yaxis().set_ticks(ebv_indices)
    ax.get_yaxis().set_ticklabels(ebv_labels)
    ax.set_zlabel('Z')
    ax.set_zlim(0, len(z_indices))
    z_ticks = np.linspace(0,len(z_indices),7)
    ax.set_zticks(z_ticks)
    z_labels = z_ticks * 6 / len(z_indices)
    ax.set_zticklabels(z_labels)
    plt.show()
