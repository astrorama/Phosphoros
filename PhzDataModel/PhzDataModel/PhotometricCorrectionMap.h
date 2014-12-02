/** 
 * @file PhotometricCorrectionMap.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PHOTOMETRICCORRECTIONMAP_H
#define	PHZDATAMODEL_PHOTOMETRICCORRECTIONMAP_H

#include <map>
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzDataModel {

typedef std::map<XYDataset::QualifiedName, double> PhotometricCorrectionMap;

} // end of namespace PhzDataModel
} // end of namespace Euclid

#endif	/* PHZDATAMODEL_PHOTOMETRICCORRECTIONMAP_H */

