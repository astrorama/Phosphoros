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

from scipy.stats import gaussian_kde

# Astropy >= 2.0rc1
try:
    from astropy.samp import SAMPIntegratedClient
except ImportError:
    from astropy.vo.samp import SAMPIntegratedClient

logger = Logging.getLogger('PhosphorosPlotSpecZComparison')


#
# -------------------------------------------------------------------------------
#

def read_specz_catalog(filename, id_col, specz_col):
    specz_cat = tut.read_table(filename)
    if id_col not in specz_cat.colnames:
        raise ValueError('ERROR : Spec-z catalog does not have column with name {}'.format(id_col))
    if not id_col == 'ID':
        specz_cat.rename_column(id_col, 'ID')
    if specz_col not in specz_cat.colnames:
        raise ValueError(
            'ERROR : Spec-z catalog does not have column with name {}'.format(specz_col))
    if not specz_col == 'SPECZ':
        specz_cat.rename_column(specz_col, 'SPECZ')
    return specz_cat[['ID', 'SPECZ']]


#
# -------------------------------------------------------------------------------
#

def read_phosphoros_catalog(out_cat, id_col, phz_col):
    phos_cat = tut.read_table(out_cat)
    phos_cat.add_column(table.Column(np.arange(len(phos_cat)), name='Index'))
    if phz_col not in phos_cat.colnames:
        raise ValueError(
            'ERROR : Photo-z catalog does not have column with name {}'.format(phz_col))
    if id_col not in phos_cat.colnames:
        raise ValueError(
            'ERROR : Photo-z catalog does not have ID column with name {}'.format(id_col))
    if id_col != 'ID':
        phos_cat.rename_column(id_col, 'ID')
    phos_cat.rename_column(phz_col, 'PHZ')

    cols = ['ID', 'PHZ']
    for col in phos_cat.colnames:
        if col.endswith('-1D-PDF'):
            cols.append(col)

    return phos_cat[cols]


#
# -------------------------------------------------------------------------------
#

def get_pdf_bins_from_comment(comment_lines, parameter):
    type_map = {'SED': str, 'REDDENING-CURVE': str, 'EBV': float, 'Z': float,
                'LIKELIHOOD-SED': str, 'LIKELIHOOD-REDDENING-CURVE': str, 'LIKELIHOOD-EBV': float,
                'LIKELIHOOD-Z': float}
    bins_str = ''.join(comment_lines)
    bins_str = bins_str[bins_str.index(parameter + '-BINS :'):]
    bins_str = bins_str[bins_str.index('{') + 1: bins_str.index('}')]
    bins = [type_map[parameter](value) for value in bins_str.split(',')]
    return bins


#
# -------------------------------------------------------------------------------
#

def read_pdfs(catalog, out_dir):
    logger.info('Loading the 1D-PDFs...')
    logger.info(catalog.colnames)
    filename_map = {'SED': 'pdf_sed.fits', 'REDDENING-CURVE': 'pdf_red_curve.fits',
                    'EBV': 'pdf_ebv.fits',
                    'Z': 'pdf_z.fits',
                    'LIKELIHOOD-SED': 'likelihood_pdf_sed.fits',
                    'LIKELIHOOD-REDDENING-CURVE': 'likelihood_pdf_red_curve.fits',
                    'LIKELIHOOD-EBV': 'likelihood_pdf_ebv.fits',
                    'LIKELIHOOD-Z': 'likelihood_pdf_z.fits'}

    def read(parameter):
        if parameter + '-1D-PDF' in catalog.colnames:
            print('    ' + parameter + ': Using catalog column ' + parameter + '-1D-PDF')

            key_comment = 'COMMENT'
            if key_comment not in catalog.meta.keys():
                key_comment = 'comments'
            return parameter, get_pdf_bins_from_comment(catalog.meta[key_comment], parameter), \
                   catalog[parameter + '-1D-PDF']

        elif out_dir and os.path.exists(out_dir + '/' + filename_map[parameter]):
            logger.info(
                '    ' + parameter + ': Reading file ' + out_dir + '/' + filename_map[parameter])
            hdus = fits.open(out_dir + '/' + filename_map[parameter])
            return parameter, hdus[1].data[hdus[1].columns[0].name], [
                hdus[s['Index'] + 1].data['Probability'] for s in catalog]
        else:
            logger.error('    ' + parameter + ': No 1D PDF found')
            return None

    result = [read(p) for p in ['SED', 'REDDENING-CURVE', 'EBV', 'Z',
                                'LIKELIHOOD-SED', 'LIKELIHOOD-REDDENING-CURVE', 'LIKELIHOOD-EBV',
                                'LIKELIHOOD-Z']]
    result = [r for r in result if r is not None]
    return result


#
# -------------------------------------------------------------------------------
#

def compute_stats(specz, phz):
    """
    Computes mean, median, sigma and outliers 
    
    Returns outliers array
    """

    diffArr = phz - specz
    plusArr = 1 + specz
    dataArr = diffArr / plusArr

    mean = np.average(dataArr)
    median = np.median(dataArr)
    sigma = np.std(dataArr)

    # Mean absolute deviation
    mad = np.median(abs(dataArr - median))

    absDataArr = abs(dataArr)
    outliers = [i for i in absDataArr if i > 0.15]
    outliersPercent = len(outliers) * 100. / len(phz)

    # Without outliers
    noOutliersArr = [i for i in absDataArr if i <= 0.15]
    sigmaNoOutliers = np.std(noOutliersArr)
    meanNoOutliers = np.average(noOutliersArr)

    print('--> Mean                : ', mean)
    print('--> Median              : ', median)
    print('--> Sigma               : ', sigma)
    print('--> Mad                 : ', mad)
    print('--> Outliers            : ', outliersPercent, '%')
    print('--> Sigma (no outliers) : ', sigmaNoOutliers)

    return dataArr, mean, median, sigma, mad, outliersPercent, sigmaNoOutliers, meanNoOutliers


#
# -------------------------------------------------------------------------------
#

class SpeczPhotozPlot(object):

    def _customPicker(self, point, mouseevent):
        if mouseevent.xdata is None:
            return False, {}
        inside, info = point.contains(mouseevent)
        if inside and np.any(info['ind']):
            info['obj_id'] = self.ids[info['ind'][0]]
        return inside, info

    def _createSpeczPhotozPlot(self):
        self.spzPhzAx = plt.subplot2grid((4, 3), (0, 0), rowspan=3, colspan=3)
        self.spzPhzAx.set_ylabel('PhotoZ')
        # Plot the data
        self.spzPhzAx.scatter(self.specz, self.photz, c=self.density, marker='o', s=10,
                              cmap='jet',
                              picker=self._customPicker if not self.embedded else False,
                              rasterized=self.embedded)
        self.spzPhzAx.set_xlim([-0.1, self.z_max])
        self.spzPhzAx.set_ylim([-0.1, self.z_max])
        # Plot the diagonal
        self.spzPhzAx.plot([-0.1, self.z_max], [-0.1, self.z_max], c='k', alpha=0.2)
        # Plot the outlier limits
        x_r = [v / 10. for v in range(0, int(self.z_max * 10 + 1))]
        y1 = [0.15 + 1.15 * x for x in x_r]
        y2 = [-0.15 + .85 * x for x in x_r]
        self.spzPhzAx.plot(x_r, y1, "r", alpha=.3)
        self.spzPhzAx.plot(x_r, y2, "r", alpha=.3)

    def _createDeltaZPlot(self):
        self.deltaZAx = plt.subplot2grid((4, 3), (3, 0), colspan=3)
        self.deltaZAx.set_ylabel('$\\Delta$z/(1+z)')
        self.deltaZAx.set_xlabel('SpecZ')
        self.deltaZAx.scatter(self.specz, self.data, c=self.density, marker='o', s=10, cmap='jet',
                              picker=self._customPicker if not self.embedded else False,
                              rasterized=self.embedded)
        # Plot the zero line
        self.deltaZAx.plot([-0.1, self.z_max], [0, 0], c='k', alpha=0.2)
        # Plot the outlier limits
        self.deltaZAx.plot([-0.1, self.z_max], [0.15, 0.15], c='r', alpha=0.3)
        self.deltaZAx.plot([-0.1, self.z_max], [-0.15, -0.15], c='r', alpha=0.3)
        self.deltaZAx.set_xlim([-0.1, self.z_max])
        self.deltaZAx.set_ylim([-self.data_max, self.data_max])

    def _createSelected(self):
        self.id_text = self.spzPhzAx.text(0.05, 0.95, 'selected ID:',
                                          transform=self.spzPhzAx.transAxes, va='top')
        self.selected1 = self.spzPhzAx.scatter(0., 0., c='none', marker='o', s=60.,
                                               edgecolors='#00EE00', linewidth=2., visible=False)
        self.selected2 = self.deltaZAx.scatter(0., 0., c='none', marker='o', s=60.,
                                               edgecolors='#00EE00', linewidth=2., visible=False)

    def _updatePlots(self):
        if self.selected_index is None:
            return []
        self.id_text.set_text('selected ID: {}'.format(self.ids[self.selected_index]))
        self.selected1.set_visible(True)
        self.selected1.set_offsets(
            [self.specz[self.selected_index], self.photz[self.selected_index]])
        self.selected2.set_visible(True)
        self.selected2.set_offsets(
            [self.specz[self.selected_index], self.data[self.selected_index]])
        return self.id_text, self.selected1, self.selected2

    def _makeTightLayout(self):
        self.fig.tight_layout()
        self.fig.subplots_adjust(hspace=0)

    def __init__(self, ids, specz, photz, data, figsize=None, embedded=True):
        self.data = data
        self.embedded = embedded

        self.fig = plt.figure(figsize=figsize)
        self.z_max = max(max(specz), max(photz)) + 0.1
        self.data_max = max(max(data), abs(min(data))) + 0.1
        xy = np.vstack([specz, photz])
        density = gaussian_kde(xy)(xy)
        z_order = np.argsort(density)
        self.specz, self.photz, self.density = specz[z_order], photz[z_order], density[z_order]
        self.data = self.data[z_order]
        self.ids = ids[z_order]

        self._createSpeczPhotozPlot()
        self._createDeltaZPlot()

        if not self.embedded:
            self._createSelected()

        self._makeTightLayout()
        if not self.embedded:
            self.fig.canvas.mpl_connect('resize_event', lambda x: self._makeTightLayout())
        self.canvas = self.fig.canvas

        self.selected_index = None

        if not self.embedded:
            self.anim = animation.FuncAnimation(self.fig, lambda n: self._updatePlots(),
                                                interval=100, blit=True, repeat=True)

    def updateSelectedObject(self, obj_id):
        self.selected_index = np.nonzero(self.ids == obj_id)[0][0]


#
# -------------------------------------------------------------------------------
#

def displayHistogram(data, mean, median, mad, sigma, outliersPercent, sigmaNoOutliers,
                     meanNoOutliers, figsize=None):
    f, ax = plt.subplots(figsize=figsize)

    plt.hist(data, bins=100)
    ax.axvline(x=0.15, c='r', alpha=.3)
    ax.axvline(x=-0.15, c='r', alpha=.3)
    ax.set_xlabel("Value")
    ax.set_ylabel("Frequency")
    ax.set_title('Distribution of : (PhotoZ - SpecZ)/(1 + SpecZ)')

    # Write information
    txt = '\n  Mean : %2.5f\n  Median : %2.5f\n  Mad : %2.5f\n  Sigma : %2.5f\n  Outliers : %2.5f%%\n  Sigma(no outliers) : %2.5f\n  Mean((no outliers) : %2.5f ' \
          % (mean, median, mad, sigma, outliersPercent, sigmaNoOutliers, meanNoOutliers)
    ax.text(ax.get_xlim()[0], ax.get_ylim()[1], txt, fontsize=10, family='sans-serif',
            style='italic', ha='left', va='top', alpha=.5)

    f.tight_layout()
    f.canvas.mpl_connect('resize_event', lambda x: f.tight_layout())
    return f


#
# -------------------------------------------------------------------------------
#

def boxPlot(x, deltaz, bins=10, figsize=None):
    # Compute binning
    edges = np.histogram_bin_edges(x, bins=bins)
    centers = (edges[1:] + edges[:-1]) / 2
    bins = np.digitize(x, bins=edges)
    # List of list of data points (one list of data points per bin)
    data_per_bin = []
    for b in np.arange(1, 1 + len(centers)):
        data_per_bin.append(deltaz[bins == b])
    # Plot
    fig = plt.figure(figsize=figsize)
    # Whiskers on the outlier limit
    plt.boxplot(data_per_bin, labels=centers)
    # 0
    plt.axhline(0, 0, 1, linestyle='--', color='r')
    # Outlier limit
    plt.axhline(0.15, 0, 1, linestyle=':', color='gray')
    plt.axhline(-0.15, 0, 1, linestyle=':', color='gray')
    # Labels
    if hasattr(x, 'name'):
        plt.xlabel(x.name)
    plt.ylabel('$\\Delta$z/(1+z)')
    return fig


#
# -------------------------------------------------------------------------------
#

class PdfPlot(object):

    def _updatePlot(self):
        if self.selected_index is None:
            return []

        self.lines.set_ydata(self.pdf_list[self.selected_index])
        self.ax.set_title('selected ID: {}'.format(self.catalog['ID'][self.selected_index]))
        self.ax.set_ylim(0, max(self.pdf_list[self.selected_index]) * 1.05)

        if self.parameter == 'Z':
            y_lim = self.ax.get_ylim()
            y_size = y_lim[1] - y_lim[0]
            specz = self.catalog[self.selected_index]['SPECZ']
            self.speczLine.set_visible(True)
            self.speczLine.set_xdata([specz, specz])
            self.speczLine.set_ydata(y_lim)
            self.speczText.set_visible(True)
            self.speczText.set_x(specz)
            self.speczText.set_y(y_lim[0] + .9 * y_size)
            phz = self.catalog[self.selected_index][self.phz_col]
            self.phzLine.set_visible(True)
            self.phzLine.set_xdata([phz, phz])
            self.phzLine.set_ydata(y_lim)
            self.phzText.set_visible(True)
            self.phzText.set_x(phz)
            self.phzText.set_y(y_lim[0] + .9 * y_size)

        return [self.ax, self.lines, self.speczLine, self.speczText, self.phzLine, self.phzText]

    def _initFigure(self):
        self.fig, self.ax = plt.subplots(figsize=(6, 3))
        self.fig.canvas.set_window_title(self.parameter + ' 1D PDF')
        xs = self.bins if self.is_numerical else range(len(self.bins))
        self.lines, = self.ax.plot(xs, [0] * len(xs))
        self.ax.set_xlim(0, max(xs))
        if not self.is_numerical:
            labels = []
            for l in self.bins:
                if '/' in l:
                    labels.append(l[l.rfind('/') + 1:])
                else:
                    labels.append(l)
            self.ax.set_xticks(range(len(self.bins)))
            self.ax.set_xticklabels(labels, rotation='vertical')
        self.ax.set_title(' ')
        self.fig.tight_layout()
        self.fig.canvas.mpl_connect('resize_event', lambda x: self.fig.tight_layout())

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
        self.pdf_title_map = {'SED': 'SED', 'REDDENING-CURVE': 'Reddening Curve', 'EBV': 'E(B-V)',
                              'Z': 'Z'}
        self._initFigure()
        self._initZLines()

        self.selected_index = None
        self.anim = animation.FuncAnimation(self.fig, lambda n: self._updatePlot(), interval=100,
                                            blit=False, repeat=True)

    def updateSelectedObject(self, obj_id):
        self.selected_index = np.argwhere(self.catalog['ID'] == obj_id)[0][0]


#
# -------------------------------------------------------------------------------
#

class Selector(object):

    def __init__(self, obj_list, catalog):
        for obj in obj_list:
            if hasattr(obj, 'canvas'):
                obj.canvas.mpl_connect('pick_event', self.onpick)
        self.obj_list = obj_list
        self.catalog = catalog

    def _print_cols(self, obj_id):
        row = self.catalog[self.catalog['ID'] == obj_id][0]
        print('\nFull info for source with ID', obj_id, ':')
        for c in row.colnames:
            print('   ', c, '=', row[c])

    def onpick(self, event):
        if event.mouseevent.button != 1 or not hasattr(event, 'obj_id'):
            return

        obj_id = event.obj_id

        for obj in self.obj_list:
            if hasattr(obj, 'updateSelectedObject'):
                obj.updateSelectedObject(obj_id)

        if not event.mouseevent.dblclick:
            return

        self._print_cols(obj_id)


#
# -------------------------------------------------------------------------------
#

class SampUpdater(object):

    def __init__(self, specz_file, specz_id, photoz_file, photoz_id, catalog, update_listeners):
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
            self.table_names = []
            self.table_ids = []
            self.table_reverse_ids = []
            table_paths = []
            table_id_cols = []

            if os.path.exists(specz_file):
                table_paths.append(specz_file)
                table_id_cols.append(specz_id)
            if os.path.exists(photoz_file):
                table_paths.append(photoz_file)
                table_id_cols.append(photoz_id)

            self.reverse_lookup = []
            for table_path, idcol in zip(table_paths, table_id_cols):
                self.table_urls.append('file:' + os.path.abspath(table_path))
                self.table_names.append(os.path.basename(table_path))

                ids = table.Table.read(table_path, format='fits')[idcol]
                reverse_map = dict()
                for i, obj_id in enumerate(ids):
                    reverse_map[obj_id] = i
                self.table_reverse_ids.append(reverse_map)
                self.table_ids.append(ids)

                message = {'samp.mtype': 'table.load.fits',
                           'samp.params': {'url': self.table_urls[-1],
                                           'name': self.table_names[-1]}}
                self.client.notify(self.topcat, message)

    def updateSelectedObject(self, obj_id):
        if self.topcat:
            for url, reverse_id in zip(self.table_urls, self.table_reverse_ids):
                message = {'samp.mtype': 'table.highlight.row',
                           'samp.params': {'url': url,
                                           'row': str(reverse_id[obj_id])}}
                self.client.notify_all(message)

    def rowNotification(self, private_key, sender_id, mtype, params, extra):
        url = params['url']
        if url not in self.table_paths:
            return
        i = self.table_urls.index(url)
        row = int(params['row'])
        id = self.table_ids[i][row]
        for l in self.update_listeners:
            if getattr(l, 'updateSelectedRow', None) is not None:
                l.updateSelectedObject(id)


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

    parser.add_argument('-scat', '--specz-catalog', type=str, required=True,
                        help='Catalog file containing the spec-z')
    parser.add_argument('-sid', '--specz-cat-id', type=str, default='ID',
                        help='Spec-z catalog ID column')
    parser.add_argument('-scol', '--specz-column', type=str, default='ZSPEC',
                        help='Spec-z column name')
    parser.add_argument('-pod', '--phosphoros-output-dir', required=False, type=str,
                        help='Directory to read Phosphoros outputs from')
    parser.add_argument('-pcat', '--phz-catalog', type=str, default=None, help='Photo-z catalog')
    parser.add_argument('-pid', '--phz_id', type=str, default='ID',
                        help='Photo-z catalog ID column')
    parser.add_argument('-pcol', '--phz-column', type=str, default='Z', help='Photo-z column name')
    parser.add_argument("-nd", "--no-display", action="store_true", default=False,
                        help="Disables the plot window")
    parser.add_argument("-n", "--sample_size", type=int,
                        help="Plot only a sample of the full catalog")
    parser.add_argument("-samp", "--samp", action="store_true", default=False,
                        help="Enables communication with other SAMP applications")
    return parser


def mainMethod(args):
    """
    @brief The "main" method.
    @details
        This method is the entry point to the program. In this sense, it is
        similar to a main (and it is why it is called mainMethod()).
    """
    if not args.phosphoros_output_dir and not args.phz_catalog:
        raise ValueError('At least one of phz-catalog or phosphoros-output-dir must be specified')

    if not args.phz_catalog:
        args.phz_catalog = os.path.join(args.phosphoros_output_dir, 'phz_cat.fits')
        if not os.path.exists(args.phz_catalog):
            args.phz_catalog = os.path.join(args.phosphoros_output_dir, 'phz_cat.txt')

    specz_cat = read_specz_catalog(args.specz_catalog, args.specz_cat_id, args.specz_column)

    phos_cat = read_phosphoros_catalog(args.phz_catalog, args.phz_id, args.phz_column)

    # Make sure the comments metadata are only picked from phos_cat, as the
    # bins are stored there. Otherwise, specz_cat may have COMMENT, and phos_cat comments, we end with both,
    # and later methods pick the first over the second
    for k in ['COMMENT', 'comments']:
        if k in specz_cat.meta:
            del specz_cat.meta[k]

    # merge the catalogs
    logger.info('Merging the catalogs')
    catalog = table.join(specz_cat, phos_cat, keys='ID')

    if len(catalog) == 0:
        logger.critical('No matching objects found between the SpecZ and the PhotoZ catalogs')
        logger.critical('Was the proper ID column chosen?')
        exit(1)

    specz = catalog['SPECZ']
    phz = catalog['PHZ']
    data, mean, median, sigma, mad, outliersPercent, sigmaNoOutliers, meanNoOutliers = compute_stats(
        specz, phz)

    if args.no_display:
        exit()

    if args.sample_size:
        idx = np.arange(len(specz))
        np.random.shuffle(idx)
        idx = idx[:args.sample_size]
        specz = specz[idx]
        phz = phz[idx]
        catalog = catalog[idx]
        data = data[idx]

    fig1 = SpeczPhotozPlot(catalog['ID'], specz, phz, data, figsize=(7, 8), embedded=False)
    fig2 = displayHistogram(data, mean, median, mad, sigma, outliersPercent, sigmaNoOutliers,
                            meanNoOutliers,
                            figsize=(10, 4))

    pdfs = read_pdfs(catalog, args.phosphoros_output_dir)
    pdf_plots = [
        PdfPlot(param, bins, pdf_list, catalog, 'PHZ') for param, bins, pdf_list in pdfs if
        len(bins) > 1
    ]

    samp = None
    if args.samp:
        samp = SampUpdater(args.specz_catalog, args.specz_cat_id,
                           args.phz_catalog, args.phz_id,
                           catalog,
                           [fig1] + pdf_plots)

    selector = Selector([fig1, samp] + pdf_plots, catalog)

    try:
        plt.show()
    finally:
        if samp:
            samp.client.disconnect()
