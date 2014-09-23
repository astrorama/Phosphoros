/** 
 * @file PhzDataModel/PhzModel.h
 * @date May 20, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PHZMODEL_H
#define	PHZDATAMODEL_PHZMODEL_H

#include <tuple>
#include <vector>
#include <utility>
#include <istream>
#include "GridContainer/GridAxis.h"
#include "GridContainer/GridContainer.h"
#include "GridContainer/serialize.h"
#include "XYDataset/QualifiedName.h"
#include "PhzDataModel/serialization/QualifiedName.h"

namespace Euclid {
namespace PhzDataModel {

struct ModelParameter {
  enum {
    Z = 0,
    EBV = 1,
    REDDENING_CURVE = 2,
    SED = 3
  };
};

typedef std::tuple<Euclid::GridContainer::GridAxis<double>, Euclid::GridContainer::GridAxis<double>,
    Euclid::GridContainer::GridAxis<Euclid::XYDataset::QualifiedName>, Euclid::GridContainer::GridAxis<Euclid::XYDataset::QualifiedName>> ModelAxesTuple;

ModelAxesTuple createAxesTuple(std::vector<double> zs, std::vector<double> ebvs,
                               std::vector<Euclid::XYDataset::QualifiedName> reddening_curves,
                               std::vector<Euclid::XYDataset::QualifiedName> seds) ;

template<typename GridCellManager>
using PhzGrid = typename Euclid::GridContainer::GridContainer<GridCellManager, double, double, Euclid::XYDataset::QualifiedName, Euclid::XYDataset::QualifiedName>;

template<typename GridCellManager>
PhzGrid<GridCellManager> phzGridBinaryImport(std::istream& in) {
  return Euclid::GridContainer::gridBinaryImport<GridCellManager, double, double, Euclid::XYDataset::QualifiedName, Euclid::XYDataset::QualifiedName>(in);
}

}
} // end of namespace Euclid

#endif	/* PHZDATAMODEL_PHZMODEL_H */

