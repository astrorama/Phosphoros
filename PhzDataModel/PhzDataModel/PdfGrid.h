/** 
 * @file PdfGrid.h
 * @date January 12, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PDFGRID_H
#define	PHZDATAMODEL_PDFGRID_H

#include <vector>
#include "PhzDataModel/PhzModel.h"

namespace Euclid {
namespace PhzDataModel {

typedef std::vector<double> PdfCellManager;

typedef PhzGrid<PdfCellManager> PdfGrid;

}
} // end of namespace Euclid

#endif	/* PHZDATAMODEL_PDFGRID_H */

