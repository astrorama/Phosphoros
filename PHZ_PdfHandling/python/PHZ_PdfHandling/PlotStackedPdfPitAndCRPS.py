
import argparse
from ElementsKernel import Logging
from ElementsKernel.Program import str_to_bool
from ElementsKernel import Exit

import numpy as np

from astropy.io import fits
import re
from astropy import table

import matplotlib.pyplot as plt
import PHZ_PdfHandling.PdfPlot
from matplotlib.colors import LogNorm


def defineSpecificProgramOptions():
    """
    @brief Allows to define the (command line and configuration file) options specific to
    this program

    @details
        See the ElementsProgram documentation for more details.
    @return
        An  ArgumentParser.
    """
    parser = argparse.ArgumentParser()

    parser.add_argument('--pdz-catalog-file', type=str, help='Path to the .fits file containing the PDZ and the point estimate')
    parser.add_argument('--refz-catalog-file', type=str, default='', help='Path to the .fits file containing the reference redshift ( If not provided the program looks for the refZ into pdz-catalog-file)')
    parser.add_argument('--pdz-col-id', type=str, default='ID', help='Column containing the identifiant of the source in the pdz-catalog-file')
    parser.add_argument('--pdz-col-pdf', type=str, default='Z-1D-PDF', help='Column containing the PDZ in the pdz-catalog-file')
    parser.add_argument('--pdz-col-pe', type=str, default='Z', help='Column containing the point estimate of the redshift in the pdz-catalog-file')
    parser.add_argument('--refz-col-id', type=str, default='ID', help='Column containing the identifiant of the source in the refz-catalog-file (Not used if refz-catalog-file is skipped or identical to pdz-catalog-file)')
    parser.add_argument('--refz-col-ref', type=str, default='Z-TRUE', help='Column containing the reference Redshift of the source in the refz-catalog-file')
    parser.add_argument('--stack-bins', type=int, default='20', help='Number of bin for the stacking of the pdf')
    parser.add_argument('--hist-bins', type=int, default='20', help='Number of bin for the histograms')
    parser.add_argument('--ref-plot', type=str, default='True', help='Display the stacked PDF VS reference plot (default "True")')
    parser.add_argument('--shift-plot', type=str, default='True', help='Display the shifted and stacked PDF plot (default "True")')
    parser.add_argument('--pit-plot', type=str, default='True', help='Display the pit plot (computation of PIT and CRPS may ) (default "True")')
    parser.add_argument('--crps-plot', type=str, default='True', help='Display the crps plot (computation of PIT and CRPS may ) (default "True")')
    
    return parser
 
def getFitsComment(file_path):
    hdul = fits.open(file_path)    
    comment =  str(hdul[1].header['COMMENT'])

    hdul.close();
    return comment
 
def readPdzSampling(comment):
    reg = "Z-BINS:\{[\d,.Ee-]+"
    if "LIKELIHOOD-Z-BINS" in comment:
        reg = "LIKELIHOOD-"+reg
    data = re.findall(reg,comment.replace('\n','').replace(' ',''))
    if len(data)!=1:
        raise Exception('Unable to find sampling in the file.')
    nodes = data[0].replace('\n','').replace(' ','').split('{')[1].split(',')
    pdz_bins = [float(z) for z in nodes]
    return pdz_bins


def readTable(file_path, columns, index =1):
    hdul = fits.open(file_path) 
    checkColumns(hdul[index], columns)
    data = hdul[index].data
    return data  
  
def checkColumns(hdu, columns):
    for col in columns:
        if col not in hdu.columns.names:
           raise Exception('The Column '+col+' is missing in the fits file.') 

def progressCallback(value,total):
    ten = int(total/20)
    if value%ten==0:
        logger = Logging.getLogger('PlotStackedPdfPitAndCrps')
        logger.info('Computing PIT & CRPS:'+str(int(100*value/total))+"%")
        
            

def mainMethod(args):
    """
    @brief The "main" method.
    @details This method is the entry point to the program. In this sense, it is similar to a main
    (and it is why it is called mainMethod()). The code below contains the calls to the
    different classes created for the first developer's workshop

        See the ElementsProgram documentation for more details.
    """
    logger = Logging.getLogger('PlotStackedPdfPitAndCrps')
    logger.info('Entering PlotStackedPdfPitAndCrps mainMethod()')
    
    columns_in_pdz = [args.pdz_col_id, args.pdz_col_pdf, args.pdz_col_pe]
    column_in_refz = [args.refz_col_id, args.refz_col_ref]
    
    two_files = True
    if args.refz_catalog_file == '' or args.refz_catalog_file == args.pdz_catalog_file:
        columns_in_pdz.append(args.refz_col_ref) 
        two_files = False
    
    # Get the PDZ sampling
    pdz_bins = np.array(readPdzSampling(getFitsComment(args.pdz_catalog_file))) 
    nb_bin = args.stack_bins
    stack_bins= [ pdz_bins[0] + (pdz_bins[-1]+pdz_bins[0])/(1.0*nb_bin)*index for index in range(nb_bin+1)]
    logger.info('Bins borders for the stacking of the PDF :' + str(stack_bins))

    #get the data
    data = readTable(args.pdz_catalog_file, columns_in_pdz)
    
    pdf_data = data[:][args.pdz_col_pdf]
    point_estimates = data[:][args.pdz_col_pe]
    
    
    if two_files:
        data2 = readTable(args.refz_catalog_file, column_in_refz)
        reference_values = data2[:][args.refz_col_ref]
    else:
        reference_values = data[:][args.refz_col_ref]
    if args.ref_plot.upper()=="TRUE":
        logger.info('Computing the reference map')  
        ref_map = PHZ_PdfHandling.PdfPlot.stackPdfFromBinInRef(pdf_data, reference_values, stack_bins)
    if args.shift_plot.upper()=="TRUE":
        logger.info('Computing the shift map')  
        shift_map,bins = PHZ_PdfHandling.PdfPlot.stackedShiftedPdf(pdf_data, pdz_bins, point_estimates, reference_values, stack_bins)
    if args.pit_plot.upper()=="TRUE" or args.crps_plot.upper()=="TRUE": 
        pits, crps  = PHZ_PdfHandling.PdfPlot.computePitAndCrps(pdf_data, pdz_bins, point_estimates, progressCallback)
      
      
    if args.ref_plot.upper()=="TRUE": 
        f = plt.figure(1)   
        PHZ_PdfHandling.PdfPlot.plotPdfMap(ref_map, pdz_bins, stack_bins, [0.68, 0.9], norm=LogNorm(vmin=0.01,vmax=np.max(ref_map)))
        f.show()
    if args.shift_plot.upper()=="TRUE":
        f = plt.figure(2)   
        PHZ_PdfHandling.PdfPlot.plotPdfMap(shift_map, bins, stack_bins, [0.68,0.9] )
        f.show()
    if args.pit_plot.upper()=="TRUE":
        f = plt.figure(3)   
        PHZ_PdfHandling.PdfPlot.plotPIT(pits, histo_bins=args.hist_bins)
        f.show()
    if args.crps_plot.upper()=="TRUE":
        f = plt.figure(4)   
        PHZ_PdfHandling.PdfPlot.plotCRPS(crps, histo_bins=args.hist_bins)
        f.show()
    plt.show()
    return Exit.Code["OK"]
