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

/**
 * @struct Euclid::PhzDataModel::ModelParameter
 * @brief Define the axis (and their order) of the Phz grids.
 *
 * @details
 * The order of the axis is of grate importance as this will determine the model walk-through and therefore which intermediary result can be re-used.
 *
 */
struct ModelParameter {
  enum {
    Z = 0,
    EBV = 1,
    REDDENING_CURVE = 2,
    SED = 3
  };
};

/**
 * @def Euclid::PhzDataModel::ModelAxesTuple
 * @brief Alias on the Axes tuple specifying the actual type of axis.
 *
 */
typedef std::tuple<GridContainer::GridAxis<double>, GridContainer::GridAxis<double>,
    GridContainer::GridAxis<XYDataset::QualifiedName>, GridContainer::GridAxis<XYDataset::QualifiedName>> ModelAxesTuple;

/**
 * @brief Define the Phz model parameter space.
 *
 * @param zs
 * A vector containing the red-shifts values for the model.
 *
 * @param ebvs
 * A vector containing the color excess ( E(B-V) ) values for the model.
 *
 * @param reddening_curves
 * A vector containing the reddening curves for the model.
 *
 * @param seds
 * A vector containing the original spectral energy density (SED) for the model.
 *
 * @return The ModelAxesTuple encoding the model parameter space.
 */
ModelAxesTuple createAxesTuple(std::vector<double> zs, std::vector<double> ebvs,
                               std::vector<XYDataset::QualifiedName> reddening_curves,
                               std::vector<XYDataset::QualifiedName> seds) ;
/**
 * @def Euclid::PhzDataModel::PhzGrid
 * @brief Alias on the Phz grid.
 *
 * @details
 * This grid has the the Phz model parameter space dimension build-in (Definition of the axes), however it is not fully specialized keeping the versatility on its GridCellManager.
 *
 */
template<typename GridCellManager>
using PhzGrid = typename GridContainer::GridContainer<GridCellManager, double, double, XYDataset::QualifiedName, XYDataset::QualifiedName>;

/**
 * @brief Retrieves a PhzGrid from a binary stream.
 *
 * @param in
 * The istream containing the serialized PhzGrid.
 *
 * @return
 * The re-constructed PhzGrid.
 */
template<typename GridCellManager>
PhzGrid<GridCellManager> phzGridBinaryImport(std::istream& in) {
  return GridContainer::gridBinaryImport<GridCellManager, double, double, XYDataset::QualifiedName, XYDataset::QualifiedName>(in);
}

}
} // end of namespace Euclid

#endif	/* PHZDATAMODEL_PHZMODEL_H */

