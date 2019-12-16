#!/usr/bin/env python

"""
Guillaume Desprez - 2019 - (guillaume.desprez@unige.ch)
Script to plot PDZ versus spec-z
"""
import numpy as np
import scipy.integrate as integrate
from scipy.optimize import curve_fit


import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm

# Point estimate determination

def getMaxSamplingPointEstimate(pdf_bins, pdf_data):
    """For each  object, return the value of the sampling for which the 1D-PDF has the higgest value
    
    Parameters:
    
    pdf_bins (array): Sampling of the 1D-PDF
    pdf_data (array(array)): an array (1 element by object) of 1-PDF values (evaluated at the 'pdf-bins' point)
    
    Returns:
    
    array: list of value of the sampling for wich the object's 1D-PDF has the higgest value
    
    """
    return np.array([pdf_bins[np.argmax(pdf)] for pdf in pdf_data])
    
def getMeanPointEstimate(pdf_bins, pdf_data):
    """For each  object, return the mean value of the 1D-PDF
    
    Parameters:
    
    pdf_bins (array): Sampling of the 1D-PDF
    pdf_data (array(array)): an array (1 element by object) of 1-PDF values (evaluated at the 'pdf-bins' point)
    
    Returns:
    
    array: list of mean value (in the sampling axis) of the object's 1D-PDF
    
    """
    return integrate.simps(np.multiply(pdf_bins,pdf_data), pdf_bins)/integrate.simps(pdf_data,  pdf_bins)
    
def getMedianPointEstimate(pdf_bins, pdf_data):
    """For each  object, return the median value of the 1D-PDF
    
    Parameters:
    
    pdf_bins (array): Sampling of the 1D-PDF
    pdf_data (array(array)): an array (1 element by object) of 1-PDF values (evaluated at the 'pdf-bins' point)
    
    Returns:
    
    array: list of median value (in the sampling axis) of the object's 1D-PDF
    
    """
    cumul = np.cumsum(pdf_data, axis=1)
    half = np.sum(pdf_data, axis=1)*0.5
    return np.array([np.interp(half[index],cumul[index,:],pdf_bins) for index in range(len(pdf_data))])  
    
def quad(x, a, b, c):
    return a*x**2 + b*x + c

def getFitModeEstimate(pdf_bins, pdf_data, threshold = 0.7):
    """For each  object, fit a parabola around the highest value of the 1D-PDF and retrun the hoggest point of the curve
    
    Parameters:
    
    pdf_bins (array): Sampling of the 1D-PDF
    pdf_data (array(array)): an array (1 element by object) of 1-PDF values (evaluated at the 'pdf-bins' point)
    threshold (float): selection of the reange on which the parabola is fitted: if set to 1 take only th highest point, to 0 all the sampling
    
    Returns:
    
    array: list of center of the fitted parabola 
    
    """
    values = [pdf_bins[np.argmax(pdf)] for pdf in pdf_data]
    for index in range(len(pdf_data)):
        id_center = np.argwhere(pdf_bins==values[index])[0][0]
        id_min = id_center
        treshold_value = threshold*pdf_data[index][id_center]  
        while id_min>0 and pdf_data[index][id_min]>treshold_value:
            id_min = id_min - 1
        id_max = id_center
        while id_max<len(pdf_bins)-1 and pdf_data[index][id_max]>treshold_value:
            id_max = id_max + 1

        if id_max - id_min > 2:
            fit = curve_fit(quad, pdf_bins[id_min:id_max], pdf_data[index][id_min:id_max])
            if (fit[0][0]<0):
                values[index] = -fit[0][1]/(2*fit[0][0])   

    return np.array(values)
        
# Stacked PDF 

def _getShiftedPdfBins(pdf_bins):
    minus_bins = -1.*pdf_bins
    minus_bins.sort()
    if 0 in pdf_bins:
        full_bins = np.concatenate((minus_bins[:-1],pdf_bins))
    else :
        full_bins = np.concatenate((minus_bins,[0],pdf_bins))
    
    extended_bins = np.concatenate((full_bins,[2*full_bins[-1]-full_bins[-2]]))
    return full_bins, extended_bins
    
def stackPdfFromBinInRef(pdf_data, reference_values, stack_bins):
    """Stack 1D-PDF in bins. Each object is attributed to a bin based on its reference value 
    
    Parameters:
    
    pdf_data (array(array)): an array (1 element by object) of 1-PDF values 
    reference_values (array): for each object the 'True' value of the PDF parameter
    stack_bins(array): The bin into which the objects are sorted.
    
    Returns:
    
    2D-array: stacked PDF for each bins
    
    """
    stack_bins = np.array(stack_bins)
    
    ref_map = np.zeros([len(pdf_data[0]),len(stack_bins)-1])
    for index, ref_value in enumerate(reference_values):
        index_bellow = stack_bins[stack_bins<ref_value] 
        if len(index_bellow)>0 and len(index_bellow)<len(stack_bins):
            ref_map[:,len(index_bellow)-1] += np.nan_to_num(pdf_data[index]/np.sum(pdf_data[index]))
    return ref_map    
    
def stackedShiftedPdf(pdf_data, pdf_bins, point_estimates, reference_values, stack_bins):
    """Stack 1D-PDF in bins. Each object is attributed to a bin based on its point-estimate value, PDFs are shifted to be centered on the reference value 
    
    Parameters:
    
    pdf_data (array(array)): an array (1 element by object) of 1-PDF values (evaluated at the 'pdf-bins' point)
    pdf_bins (array): Sampling of the 1D-PDF
    point_estimates (array): for each object the value of the parameter estimated from the PDF
    reference_values (array): for each object the 'True' value of the PDF parameter
    stack_bins(array): The bin into which the objects are sorted.
    
    Returns:
    
    2D-array: stacked PDF for each bins
    
    """
    full_bins, extended_bins = _getShiftedPdfBins(pdf_bins)
    added_nodes = int(len(full_bins)-len(pdf_bins))
    stack_bins = np.array(stack_bins)
    
    shift_map = np.zeros([len(full_bins),len(stack_bins)-1])
    for index, point_estimate in enumerate(point_estimates):
        index_bellow = stack_bins[stack_bins<point_estimate]
        if len(index_bellow)>0 and len(index_bellow)<len(stack_bins):
            # Force the PDF to 0 outside of the original sampling
            completed_pdf = np.concatenate((np.zeros(added_nodes), pdf_data[index]/np.sum(pdf_data[index]),[0]))
            shifted_pdf = np.interp(full_bins, extended_bins-reference_values[index], completed_pdf)
            shift_map[:,len(index_bellow)-1] += np.nan_to_num(shifted_pdf)
    return shift_map, full_bins   
    
# Confidence interval on stacked PDF   
def getInterval(data_map, pdf_map_bins, fraction):
    """Compute, for the stacked PDFs grid, median centered intervals containing a fraction of object
    
    Parameters:
    data_map (2D-array): stacked 1D-PDF for a set of bins
    pdf_map_bins (array): sampling of the stacked PDF axis
    fraction (float): the ratio of object to be into the computed interval
    
    Returns:
    
    array(array): for each bins an array containing the min and max of the interval
    
    """
    cumul = np.cumsum(data_map, axis=0)
    totals = np.sum(data_map, axis=0)
    
    interval_lim = np.zeros((data_map.shape[1],2))
    for index in range(data_map.shape[1]):
        if totals[index] == 0:
            interval_lim[index] = [0,0]
        else:
            interval_lim[index] = [np.interp(0.5*(1.-fraction)*totals[index], cumul[:,index], pdf_map_bins), 
                                   np.interp((0.5+0.5*fraction)*totals[index], cumul[:,index], pdf_map_bins)]   
    return interval_lim

# Probability Integral Transform (PIT) and Continuous Ranked Probabilty score (CRPS)    
def computePitAndCrps(pdf_data, pdf_bins, reference_values, progress_callback=None):
    """Compute the Probability Integral Transform (PIT) and Continuous Ranked Probabilty score (CRPS) for objects
    
    Parameters:
    
    pdf_data (array(array)): an array (1 element by object) of 1-PDF values(evaluated at the 'pdf-bins' point)
    pdf_bins (array): Sampling of the 1D-PDF 
    reference_values (array): for each object the 'True' value of the PDF parameter
    
    Returns:
    array, array: list of PIT, list of CRPS
    
    """
    total = len(reference_values)
    pits = np.zeros(total)
    crps = np.zeros(total)
    for index, ref_value in enumerate(reference_values):
        current_bin = np.array(pdf_bins)
        current_data = np.array(pdf_data[index])
        current_cumul_data =  [integrate.simps(pdf_data[index][:i], pdf_bins[:i]) if i>0 else 0 for i in range(len(pdf_bins))] / integrate.simps(pdf_data[index], pdf_bins)
        if not ref_value in current_bin:
            current_bin = np.concatenate((current_bin,[ref_value]))
            current_bin.sort()
            current_data = np.interp(current_bin, pdf_bins, current_data)
            current_cumul_data = np.interp(current_bin, pdf_bins, current_cumul_data)
        current_data = current_data / integrate.simps(current_data, current_bin)
        index_cut = np.argwhere(current_bin==ref_value)[0][0]
        if index_cut > 0:
            pits[index] = integrate.simps(current_data[:index_cut], current_bin[:index_cut]) 
            if index_cut < len(current_bin)-1:
                crps[index] = integrate.simps(np.square(current_cumul_data[:index_cut]), current_bin[:index_cut]) + integrate.simps(np.square(current_cumul_data[index_cut:] -1), current_bin[index_cut:])
            else:
                crps[index] = integrate.simps(np.square(current_cumul_data), current_bin) 
        else:
            crps[index] =  integrate.simps(np.square(current_cumul_data -1), current_bin)
        if progress_callback is not None:
            progress_callback(index,total)
    return pits, crps    
    
# Nuber of sources per bin
def getSourcesPerBin(data_map):
    return np.sum(data_map, axis=0)  

# Bias per bin 
def flat(x):
    return 1.0

def onePlusX(x):
    return 1.0 + x

def lin(pe,x):
    return pe

def aff(pe, x):
    return pe - x

def getBiasPerBin(data_map, pdf_map_bins, stack_bins, numerator=lin, denominator=flat, estimator="mean"):
    est_funct = getMeanPointEstimate
    if estimator =="max":
        est_funct = getMaxSamplingPointEstimate
    elif estimator == "med":
        est_funct = getMedianPointEstimate
    elif estimator == "fit":
        est_funct = getFitModeEstimate
        
    bias = np.zeros(len(stack_bins)-1)
    for bin_id in range(len(stack_bins)-1):
        pdf_data = np.array([data_map[:,bin_id]])
        bin_center = (stack_bins[bin_id+1]+ stack_bins[bin_id])/2.0
        pe = est_funct(pdf_map_bins, pdf_data)
        bias[bin_id] = numerator(pe, bin_center)/denominator(bin_center)
        
    return bias  

# Fraction
def getFraction(data_map, pdf_map_bins, stack_bins, ratio, estimator="mean"):
    est_funct = getMeanPointEstimate
    if estimator =="max":
        est_funct = getMaxSamplingPointEstimate
    elif estimator == "med":
        est_funct = getMedianPointEstimate
    elif estimator == "fit":
        est_funct = getFitModeEstimate
        
    cumul = np.cumsum(data_map, axis=0)
    totals = np.sum(data_map, axis=0)
    
    
    bin_centers = [(stack_bins[bin_id+1]+ stack_bins[bin_id])/2.0 for bin_id in range(len(stack_bins) -1)]
    
    frac = np.zeros(len(stack_bins)-1)
    for bin_id, x in enumerate(bin_centers):
        x_min = -ratio*(1+x)
        x_max = ratio*(1+x)
        if totals[bin_id] == 0 :
            continue
            
        pdf_data = np.array([data_map[:,bin_id]])
        pe = est_funct(pdf_map_bins, pdf_data)     
            
            
        cumVal = np.interp([x_min, x_max], pdf_map_bins-pe, cumul[:,bin_id])
        frac[bin_id] = np.abs(cumVal[1]-cumVal[0])/totals[bin_id]
              
    return frac

   
        
# Ploting tools
def plotPdfMap(data_map, pdf_map_bins, stack_bins, fractions, colors = ['r','darkorange','orange','yellow','lightgrey'], title="", **kwargs):
    """Plot stacked PDF with confidence intervals 
    
    Parameters:
    
    data_map (2D-array): stacked 1D-PDF for a set of bins
    pdf_map_bins (array): sampling of the stacked PDF axis
    stack_bins(array): The bin into which the objects are sorted.
    fractions (list of float): List of values for which the confidence intervals have to be plotted 
    colors (list of color): a list of color to be used for the confidence interval plots. If the 'fraction' list is longer than 'colors' the last colors is used multiple-time  

    """
    if len(title)>0:
        plt.title(title)
    
    cs = plt.imshow(data_map,origin='lower',
                    extent=(0,stack_bins[-1], pdf_map_bins[0],pdf_map_bins[-1]),
                    aspect='auto',**kwargs)
    
    plt.colorbar(cs)
     
    color_index = 0
    for frac in fractions:
        interval = getInterval(data_map, pdf_map_bins, frac) 
        plt.plot(stack_bins,np.append(interval[:,0],interval[-1,0]) ,c=colors[color_index],linewidth=1,drawstyle='steps-post',alpha=0.8,label=str(int(frac*100))+'% Lim')
        plt.plot(stack_bins,np.append(interval[:,1],interval[-1,1]),c=colors[color_index],linewidth=1,drawstyle='steps-post',alpha=0.8)
        if color_index<len(colors) - 1:
            color_index = color_index + 1
    plt.legend()  
    
def plotPIT( pits, histo_bins=20,**kwargs):
    """Plot an histogram of the PIT values 
    
    Parameters:
    
    pits (array): set of PIT value
    histo_bins(int): Number of bins for the histogram

    """
    mean = 1
    plt.title("PDF's PIT")
    plt.hist(pits,bins=histo_bins,range=(0,1),density=True,**kwargs)
    plt.plot([0,1],[mean,mean])

def plotCRPS( crps, histo_bins=20,**kwargs):
    """Plot an histogram of the CRPS values 
    
    Parameters:
    
    crps (array): set of PIT value
    histo_bins(int): Number of bins for the histogram

    """
    plt.title("PDF's CRPS")
    plt.hist(crps,bins=histo_bins,range=(0,crps.max()),density=True,**kwargs)

def plotSourcesPerBin(data_map, stack_bins, title = "Number of sources per bin"):
    """Plot the number of source in each stcak bin of the map
    
    Parameters:
    
    data_map (2D-array): stacked 1D-PDF for a set of bins
    stack_bins(array): The bin into which the objects are sorted.
    
    """
    data = getSourcesPerBin(data_map)
    av = np.sum(data)/(len(stack_bins)-1)
    plt.title(title)
    plt.fill_between(stack_bins,np.append(data,data[-1]), step="post", alpha=0.9)
    plt.plot(stack_bins,np.append(data,data[-1]) ,linewidth=1,drawstyle='steps-post')
    
    plt.plot([stack_bins[0],stack_bins[-1]],[av,av] ,linewidth=1)

def plotBiasForRefStacked(data_map, pdf_bins, stack_bins, estimator="mean",**kwargs):
    """Plot the bias in each stack bin of the reference stacked PDF map 
    
    Parameters:
    
    data_map (2D-array): stacked 1D-PDF for a set of bins
    pdf_bins (array): sampling of the stacked PDF axis
    stack_bins(array): The bin into which the objects are sorted
    estimator(string): the point estimate method: one of {"max", "fit", "med", "mean"}
    
    """
    plt.title("Bias per bin for stacked PDF with respect to Ref. value")
    bias = getBiasPerBin(data_map, pdf_bins, stack_bins, numerator=aff, denominator=onePlusX, estimator=estimator)
    bin_centers = [(stack_bins[bin_id+1]+ stack_bins[bin_id])/2.0 for bin_id in range(len(stack_bins) -1)]
    plt.plot(bin_centers, bias, linewidth=1,label=estimator,**kwargs)
    plt.legend()
    
def plotBiasForShiftedStack(data_map, pdf_bins, stack_bins, estimator="mean",**kwargs):
    """Plot the bias in each stack bin of the shifted stacked PDF map 
    
    Parameters:
    
    data_map (2D-array): stacked 1D-PDF for a set of bins
    pdf_bins (array): sampling of the stacked PDF axis
    stack_bins(array): The bin into which the objects are sorted
    estimator(string): the point estimate method: one of {"max", "fit", "med", "mean"}
    
    """
    plt.title("Bias per bin for shifted and stacked PDF")
    bias = getBiasPerBin(data_map, pdf_bins, stack_bins, numerator=lin, denominator=onePlusX, estimator=estimator)
    bin_centers = [(stack_bins[bin_id+1]+ stack_bins[bin_id])/2.0 for bin_id in range(len(stack_bins) -1)]
    plt.plot(bin_centers, bias, linewidth=1,label=estimator,**kwargs)
    plt.legend()
            
def plotFraction(data_map, pdf_map_bins, stack_bins, ratio, estimator="mean", title="Fraction Plot",**kwargs):
    plt.title(title)
    frac = getFraction(data_map, pdf_map_bins, stack_bins, ratio, estimator=estimator)
    bin_centers = [(stack_bins[bin_id+1]+ stack_bins[bin_id])/2.0 for bin_id in range(len(stack_bins) -1)]
    plt.plot(bin_centers, frac, linewidth=1,label=estimator+" F{:03d}".format(int(100*ratio)),**kwargs)
    plt.legend()
