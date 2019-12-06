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

from __future__ import absolute_import, division, print_function
import os
import numbers

import argparse
import astropy.table as table
import astropy.io.fits as fits
from ElementsKernel import Logging

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import PhzCLI.TableUtils as tut

# Astropy >= 2.0rc1
try:
    from astropy.samp import SAMPIntegratedClient
except ImportError:
    from astropy.vo.samp import SAMPIntegratedClient

logger = Logging.getLogger('PhosphorosPlotSpecZComparison')

#
#-------------------------------------------------------------------------------
#

def read_specz_catalog(filename, id_col, specz_col):
    specz_cat = tut.read_table(filename)
    if id_col not in specz_cat.colnames:
        raise ValueError('ERROR : Spec-z catalog does not have column with name {}'.format(id_col))
    if not id_col == 'ID':
        specz_cat.rename_column(id_col, 'ID')
    if specz_col not in specz_cat.colnames:
        raise ValueError('ERROR : Spec-z catalog does not have column with name {}'.format(specz_col))
    if not specz_col == 'SPECZ':
        specz_cat.rename_column(specz_col, 'SPECZ')
    return specz_cat[['ID', 'SPECZ']]
    
#
#-------------------------------------------------------------------------------
#

def read_phosphoros_catalog(out_dir, out_cat, phz_col):
    if not out_cat:
        out_cat = os.path.join(out_dir, 'phz_cat.fits')
        if not os.path.exists(out_cat):
            out_cat = os.path.join(out_dir, 'phz_cat.txt')
    if out_dir and out_cat[0] != '/':
        out_cat = os.path.join(out_dir, out_cat)

    phos_cat = tut.read_table(out_cat)
    phos_cat.add_column(table.Column(np.arange(len(phos_cat)), name='Index'))
    if phz_col not in phos_cat.colnames:
        raise ValueError('ERROR : Photo-z catalog does not have column with name {}'.format(phz_col))
    phos_cat.rename_column(phz_col, 'PHZ')
    return phos_cat[['ID', 'PHZ']]
    
#
#-------------------------------------------------------------------------------
#

def get_pdf_bins_from_comment(comment_lines, parameter):
    type_map = {'SED': str, 'REDDENING-CURVE': str, 'EBV': float, 'Z': float,
                'LIKELIHOOD-SED': str, 'LIKELIHOOD-REDDENING-CURVE': str, 'LIKELIHOOD-EBV': float,
                'LIKELIHOOD-Z': float}
    bins_str = ''.join(comment_lines)
    bins_str = bins_str[bins_str.index(parameter+'-BINS :'):]
    bins_str = bins_str[bins_str.index('{')+1: bins_str.index('}')]
    bins = [type_map[parameter](value) for value in bins_str.split(',')]
    return bins
    
#
#-------------------------------------------------------------------------------
#

def read_pdfs(catalog, out_dir):
    logger.info('Loading the 1D-PDFs...')
    filename_map = {'SED': 'pdf_sed.fits', 'REDDENING-CURVE': 'pdf_red_curve.fits', 'EBV': 'pdf_ebv.fits',
                    'Z': 'pdf_z.fits',
                    'LIKELIHOOD-SED': 'likelihood_pdf_sed.fits',
                    'LIKELIHOOD-REDDENING-CURVE': 'likelihood_pdf_red_curve.fits',
                    'LIKELIHOOD-EBV': 'likelihood_pdf_ebv.fits', 'LIKELIHOOD-Z': 'likelihood_pdf_z.fits'}
    
    def read(parameter):
        if parameter+'-1D-PDF' in catalog.colnames:
            print('    ' + parameter + ': Using catlog column ' + parameter+'-1D-PDF')
            
            key_comment = 'COMMENT'
            if key_comment not in catalog.meta.keys():
                key_comment = 'comments'
            return parameter, get_pdf_bins_from_comment(catalog.meta[key_comment], parameter), catalog[parameter+'-1D-PDF']

        elif out_dir and os.path.exists(out_dir+'/'+filename_map[parameter]):
            logger.info('    ' + parameter + ': Reading file ' + out_dir+'/'+filename_map[parameter])
            hdus = fits.open(out_dir+'/'+filename_map[parameter])
            return parameter, hdus[1].data[hdus[1].columns[0].name], [hdus[s['Index']+1].data['Probability'] for s in catalog]
        else:
            logger.error('    ' + parameter + ': No 1D PDF found')
            return None
    
    result = [read(p) for p in ['SED', 'REDDENING-CURVE', 'EBV', 'Z',
                                'LIKELIHOOD-SED', 'LIKELIHOOD-REDDENING-CURVE', 'LIKELIHOOD-EBV', 'LIKELIHOOD-Z']]
    result = [r for r in result if r is not None]
    return result
    
#
#-------------------------------------------------------------------------------
#
    
def compute_stats(specz, phz):
    """
    Computes mean, median, sigma and outliers 
    
    Returns outliers array
    """
    
    diffArr = phz - specz
    plusArr = 1 + specz  
    dataArr = diffArr / plusArr
    
    mean     = np.average(dataArr)
    median   = np.median(dataArr)
    sigma    = np.std(dataArr)
    
    # Mean absolute deviation
    mad = np.median(abs(dataArr - median)) 
    
    absDataArr = abs(dataArr)
    outliers = [i for i in absDataArr if i > 0.15]
    outliersPercent =  len(outliers)*100. / len(phz)
 
    # Without outliers
    noOutliersArr   = [i for i in absDataArr if i <= 0.15]
    sigmaNoOutliers = np.std(noOutliersArr)
    meanNoOutliers  = np.average(noOutliersArr)

    print('--> Mean                : ', mean)     
    print('--> Median              : ', median)     
    print('--> Sigma               : ', sigma)     
    print('--> Mad                 : ', mad)     
    print('--> Outliers            : ', outliersPercent, '%')
    print('--> Sigma (no outliers) : ', sigmaNoOutliers)     
   
    return dataArr, mean, median, sigma, mad, outliersPercent, sigmaNoOutliers, meanNoOutliers

#
#-------------------------------------------------------------------------------
#

class SpeczPhotozPlot(object):
    
    def _createSpeczPhotozPlot(self):
        self.spzPhzAx = plt.subplot2grid((4,3), (0,0), rowspan=3, colspan=3)
        self.spzPhzAx.set_ylabel('PhotoZ')
        # Plot the diagonal
        self.spzPhzAx.plot([-0.1, self.z_max], [-0.1, self.z_max], c='k', alpha=0.2)
        # Plot the outlier limits
        x_r = [v/10. for v in range(0,int(self.z_max*10+1))]
        y1 = [0.15+1.15*x for x in x_r]
        y2 = [-0.15+.85*x for x in x_r]
        self.spzPhzAx.plot(x_r, y1, "r", alpha=.3)
        self.spzPhzAx.plot(x_r, y2, "r", alpha=.3)
        # Plot the data
        self.spzPhzAx.scatter(self.specz, self.photz, c='b', marker='o', s=10, edgecolors='none', alpha=0.6, picker=True)
        self.spzPhzAx.set_xlim([-0.1, self.z_max])
        self.spzPhzAx.set_ylim([-0.1, self.z_max])
    
    def _createDeltaZPlot(self):
        self.deltaZAx = plt.subplot2grid((4,3), (3,0), colspan=3)
        self.deltaZAx.set_ylabel('$\\Delta$z/(1+z)')
        self.deltaZAx.set_xlabel('SpecZ')
        # Plot the zero line
        self.deltaZAx.plot ([-0.1, self.z_max], [0, 0], c='k', alpha=0.2)
        # Plot the outlier limits
        self.deltaZAx.plot ([-0.1, self.z_max], [0.15, 0.15], c='r', alpha=0.3)
        self.deltaZAx.plot ([-0.1, self.z_max], [-0.15, -0.15], c='r', alpha=0.3)
        self.deltaZAx.scatter(self.specz, self.data, c='b', marker='o', s=10, edgecolors='none', alpha=0.6, picker=True)
        self.deltaZAx.set_xlim([-0.1, self.z_max])
        self.deltaZAx.set_ylim([-self.data_max, self.data_max])
        
    def _createSelected(self):
        self.id_text = self.spzPhzAx.text(0.05, 0.95, 'selected ID:', transform=self.spzPhzAx.transAxes, va='top')
        self.selected1 = self.spzPhzAx.scatter(0., 0., c='none', marker='o', s=60., edgecolors='#00EE00', linewidth=2., visible=False)
        self.selected2 = self.deltaZAx.scatter(0., 0., c='none', marker='o', s=60., edgecolors='#00EE00', linewidth=2., visible=False)
    
    def _updatePlots(self):
        if self.selected_index is None:
            return []
        self.id_text.set_text('selected ID: %d' % self.ids[self.selected_index])
        self.selected1.set_visible(True)
        self.selected1.set_offsets([self.specz[self.selected_index], self.photz[self.selected_index]])
        self.selected2.set_visible(True)
        self.selected2.set_offsets([self.specz[self.selected_index], self.data[self.selected_index]])
        return self.id_text, self.selected1, self.selected2
    
    def _makeTightLayout(self):
        self.fig.tight_layout()
        self.fig.subplots_adjust(hspace=0)

    def __init__(self, ids, specz, photz, data):
        self.ids = ids
        self.specz = specz
        self.photz = photz
        self.data = data
        
        self.fig = plt.figure(figsize=(7, 8))
        self.z_max = max(max(specz), max(photz)) + 0.1
        self.data_max = max(max(data), abs(min(data))) + 0.1
        
        self._createSpeczPhotozPlot()
        self._createDeltaZPlot()
        self._createSelected()
        
        self._makeTightLayout()
        self.fig.canvas.mpl_connect('resize_event', lambda x:self._makeTightLayout())
        self.canvas = self.fig.canvas
        
        self.selected_index = None
        self.anim = animation.FuncAnimation(self.fig, lambda n:self._updatePlots(), interval=100, blit=True, repeat=True)
    
    def updateSelectedRow(self, index):
        self.selected_index = index

#
#-------------------------------------------------------------------------------
#

def displayHistogram(data, mean, median, mad, sigma, outliersPercent, sigmaNoOutliers, meanNoOutliers):

    f, ax = plt.subplots(figsize=(10, 4)) 

    plt.hist(data, bins=100)
    ax.axvline(x=0.15, c='r', alpha=.3)
    ax.axvline(x=-0.15, c='r', alpha = .3)
    ax.set_xlabel("Value")
    ax.set_ylabel("Frequency")
    ax.set_title('Distribution of : (PhotoZ - SpecZ)/(1 + SpecZ)')
              
    # Write information
    txt = '\n  Mean : %2.5f\n  Median : %2.5f\n  Mad : %2.5f\n  Sigma : %2.5f\n  Outliers : %2.5f%%\n  Sigma(no outliers) : %2.5f\n  Mean((no outliers) : %2.5f ' \
        % (mean, median, mad, sigma, outliersPercent, sigmaNoOutliers, meanNoOutliers)
    ax.text(ax.get_xlim()[0], ax.get_ylim()[1], txt, fontsize=10, family='sans-serif', style='italic', ha='left', va='top', alpha=.5)
    
    f.tight_layout()
    f.canvas.mpl_connect('resize_event', lambda x: f.tight_layout())
    return f

#
#-------------------------------------------------------------------------------
#

class PdfPlot(object):
    
    def _updatePlot(self):
        if self.selected_index is None:
            return []
        
        self.lines.set_ydata(self.pdf_list[self.selected_index])
        self.ax.set_ylim(0, max(self.pdf_list[self.selected_index])*1.05)
        
        if self.parameter == 'Z':
            y_lim = self.ax.get_ylim()
            y_size = y_lim[1]-y_lim[0]
            specz = self.catalog[self.selected_index]['SPECZ']
            self.speczLine.set_visible(True)
            self.speczLine.set_xdata([specz, specz])
            self.speczLine.set_ydata(y_lim)
            self.speczText.set_visible(True)
            self.speczText.set_x(specz)
            self.speczText.set_y(y_lim[0]+.9*y_size)
            phz = self.catalog[self.selected_index][self.phz_col]
            self.phzLine.set_visible(True)
            self.phzLine.set_xdata([phz, phz])
            self.phzLine.set_ydata(y_lim)
            self.phzText.set_visible(True)
            self.phzText.set_x(phz)
            self.phzText.set_y(y_lim[0]+.9*y_size)

        return [self.ax, self.lines, self.speczLine, self.speczText, self.phzLine, self.phzText]

    def _initFigure(self):
        self.fig, self.ax = plt.subplots(figsize=(6, 3))
        self.fig.canvas.set_window_title(self.parameter+' 1D PDF')
        xs = self.bins if self.is_numerical else range(len(self.bins))
        self.lines, = self.ax.plot(xs, [0]*len(xs))
        self.ax.set_xlim(0, max(xs))
        if not self.is_numerical:
            labels = []
            for l in self.bins:
                if '/' in l:
                    labels.append(l[l.rfind('/')+1:])
                else:
                    labels.append(l)
            self.ax.set_xticks(range(len(self.bins)))
            self.ax.set_xticklabels(labels, rotation='vertical')
        self.fig.tight_layout()
        self.fig.canvas.mpl_connect('resize_event', lambda x:self.fig.tight_layout())
    
    def _initZLines(self):
        self.speczLine, = self.ax.plot([0, 0], [0, 0], c='k', alpha=.5, visible=False)
        self.speczText = self.ax.text(0, 0, 'SpecZ', color='k', alpha=.5, visible=False)
        self.phzLine, = self.ax.plot([0, 0], [0, 0], c='r', alpha=.5, visible=False)
        self.phzText = self.ax.text(0, 0, 'PHZ', color='r', alpha=.5, visible=False)
    
    def __init__(self, parameter, bins, pdf_list, catalog, phz_col):
        self.parameter = parameter
        self.bins = bins
        self.pdf_list = pdf_list
        self.catalog = catalog
        self.phz_col = phz_col
        
        self.is_numerical = isinstance(self.bins[0], numbers.Number)
        self.pdf_title_map = {'SED':'SED', 'REDDENING-CURVE':'Reddening Curve', 'EBV':'E(B-V)', 'Z':'Z'}
        self._initFigure()
        self._initZLines()
        
        self.selected_index = None
        self.anim = animation.FuncAnimation(self.fig, lambda n:self._updatePlot(), interval=100, blit=False, repeat=True)
       
    def updateSelectedRow(self, index):
        self.selected_index = index
        

#
#-------------------------------------------------------------------------------
#

class Selector(object):
    
    def __init__(self, obj_list, catalog):
        for obj in obj_list:
            if hasattr(obj, 'canvas'):
                obj.canvas.mpl_connect('pick_event', self.onpick)
        self.obj_list = obj_list
        self.catalog = catalog
    
    def _print_cols(self, index):
        row = self.catalog[index]
        print('\nFull info for source with ID',row['ID'],':')
        for c in row.colnames:
            print('   ', c, '=', row[c])
    
    def onpick(self, event):
        if event.mouseevent.button != 1:
            return
        
        index = event.ind[0]
        
        for obj in self.obj_list:
            if hasattr(obj, 'updateSelectedRow'):
                obj.updateSelectedRow(index)
                
        if not event.mouseevent.dblclick:
            return
        
        self._print_cols(index)
            

#
#-------------------------------------------------------------------------------
#

class SampUpdater(object):

    def __init__(self, specz_file, specz_id, phos_out_dir, catalog, update_listeners):
        self.topcat = None
        self.update_listeners = update_listeners
        try:
            self.client = SAMPIntegratedClient(name='PhosphorosPlotSpecZComparison')
            self.client.connect()
            for c in self.client.get_registered_clients():
                meta = self.client.get_metadata(c)
                if 'samp.name' in meta and meta['samp.name'] == 'topcat':
                    self.topcat = c
        except:
            pass
        if self.topcat:
            self.client.bind_receive_notification('table.highlight.row', self.rowNotification)
            self.table_urls = []
            self.table_ids = []
            self.global_ids = catalog['ID']
            self.global_ids_reverse = {}
            for i in range(len(self.global_ids)):
                self.global_ids_reverse[self.global_ids[i]] = i
            
            self.id_col = []
            self.orig_ids = []
            tablenames = []
            try:
                tablenames.append(specz_file)
                self.id_col.append(specz_id)
            except:
                pass
            if os.path.exists(phos_out_dir+'/phz_cat.fits'):
                tablenames.append(phos_out_dir+'/phz_cat.fits')
                self.id_col.append('ID')
            
            for ti, name in enumerate(tablenames):
                data = table.Table.read(name, format='fits')
                self.orig_ids.append(data[self.id_col[ti]])
                id_map = {}
                for i in range(len(data)):
                    id_map[data[self.id_col[ti]][i]] = i
                self.table_ids.append(id_map)
                url = 'file:' + os.path.abspath(name)
                table_name = os.path.basename(name)
                self.table_urls.append(url)
                message = {'samp.mtype' : 'table.load.fits',
                           'samp.params' : { 'url' : url,
                                             'name' : table_name }}
                self.client.notify(self.topcat, message)

    def updateSelectedRow(self, i):
        if self.topcat:
            for url, ids in zip(self.table_urls, self.table_ids):
                message = {'samp.mtype': 'table.highlight.row',
                           'samp.params': {'url': url,
                                           'row': str(ids[self.global_ids[i]])}}
                self.client.notify_all(message)

    def rowNotification(self, private_key, sender_id, mtype, params, extra):
        url = params['url']
        if url not in self.table_urls:
            return
        i = self.table_urls.index(url)
        row = int(params['row'])
        id = self.orig_ids[i][row]
        global_row = self.global_ids_reverse[id]
        for l in self.update_listeners:
            if getattr(l, 'updateSelectedRow', None) is not None:
                l.updateSelectedRow(global_row)


################## MAIN ###########

def defineSpecificProgramOptions():
    """
    @brief Allows to define the (command line and configuration file) options
    specific to this program

    @details
        See the Elements documentation for more details.
    @return
        An  ArgumentParser.
    """
    description = """
            Computes the standard deviation of [ (photZ - specZ) / 1+specZ ] and
            the outliers fraction as:
                outliers =  (photZ - specZ) / 1+specZ > 0.15  
            and the mean, median and displays an histogram of outliers
            """
    parser = argparse.ArgumentParser(description=description)

    parser.add_argument('-scat', '--specz-catalog', type=str, required=True, help='Catalog file containing the spec-z')
    parser.add_argument('-sid', '--specz-cat-id', type=str, default='ID', help='Spec-z catalog ID column')
    parser.add_argument('-scol', '--specz-column', type=str, default='ZSPEC', help='Spec-z column name')
    parser.add_argument('-pod', '--phosphoros-output-dir', required=False, type=str, help='Directory to read Phosphoros outputs from')
    parser.add_argument('-pcol', '--phz-column', type=str, default='Z', help='Photo-z column name')
    parser.add_argument('-pcat', '--phz-catalog', type=str, default=None, help='Photo-z catalog')
    parser.add_argument("-nd", "--no-display", action="store_true", default=False, help="Disables the plot window")
    parser.add_argument("-samp", "--samp", action="store_true", default=False, help="Enables communication with other SAMP applications")

    return parser


def mainMethod(args):
    """
    @brief The "main" method.
    @details
        This method is the entry point to the program. In this sense, it is
        similar to a main (and it is why it is called mainMethod()).
    """
    if not args.phosphoros_output_dir and not args.phz_catalog:
        raise ValueError('At least one of --phz-catalog or --phosphoros-output-dir must be specified')

    specz_cat = read_specz_catalog(args.specz_catalog, args.specz_cat_id, args.specz_column)
    phos_cat = read_phosphoros_catalog(args.phosphoros_output_dir, args.phz_catalog, args.phz_column)

    # merge the catalogs
    logger.info('Merging the catalogs')
    catalog = table.join(specz_cat, phos_cat, keys='ID')

    specz = catalog['SPECZ']
    phz = catalog['PHZ']
    data, mean, median, sigma, mad, outliersPercent,sigmaNoOutliers, meanNoOutliers = compute_stats(specz, phz)
    
    if args.no_display:
        exit()

    fig1 = SpeczPhotozPlot(catalog['ID'], specz, phz, data)
    fig2 = displayHistogram(data, mean, median, mad, sigma, outliersPercent, sigmaNoOutliers, meanNoOutliers)
    
    pdfs = read_pdfs(catalog, args.phosphoros_output_dir)
    pdf_plots = [
        PdfPlot(param, bins, pdf_list, catalog, 'PHZ') for param, bins, pdf_list in pdfs if len(bins) > 1
    ]
    
    samp = None
    if args.samp:
        samp = SampUpdater(args.specz_catalog, args.specz_cat_id, args.phosphoros_output_dir, catalog, [fig1] + pdf_plots)
    
    selector = Selector([fig1, samp] + pdf_plots, catalog)

    try:
        plt.show()
    finally:
        if samp:
            samp.client.disconnect()
