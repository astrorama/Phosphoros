/**
 * @file PhzDataModel/PhotometryGrid.h
 * @date May 19, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PHOTOMETRYGRID_H
#define	PHZDATAMODEL_PHOTOMETRYGRID_H

#include <memory>
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/PhzModel.h"

namespace Euclid {
namespace PhzDataModel {

typedef std::vector<Euclid::SourceCatalog::Photometry> PhotometryCellManager;

typedef PhzGrid<PhotometryCellManager> PhotometryGrid;

} // end of namespace PhzDataModel
} // end of namespace Euclid


namespace Euclid {
namespace GridContainer {

/**
 * @struct Euclid::GridContainer::GridCellManagerTraits<PhotometryCellManager>
 * @brief Specialization of the GridCellManagerTraits template.
 *
 * @details
 * We define the GridCellManagerTraits for a vector of Photometries to redefine the
 * factory method because the Photometry does not have default constructor *
 */
template<>
struct GridCellManagerTraits<Euclid::PhzDataModel::PhotometryCellManager> {
  typedef Euclid::SourceCatalog::Photometry data_type;

  typedef typename Euclid::PhzDataModel::PhotometryCellManager::iterator iterator;

  /**
   * @brief Factory to build PhotometryCellManager which has no default constructor
   *
   * @param size
   * The size of the photometry storage
   *
   * @return
   * A unique_ptr on the new PhotometryCellManager.
   *
   */
  static std::unique_ptr<Euclid::PhzDataModel::PhotometryCellManager> factory(size_t size);

  /**
   * @brief return the size of the PhotometryCellManager
   */
  static size_t size(const Euclid::PhzDataModel::PhotometryCellManager& vector);

  /**
   * @brief static iterator on the PhotometryCellManager
   */
  static iterator begin(Euclid::PhzDataModel::PhotometryCellManager& vector);

  /**
   * @brief static iterator on the PhotometryCellManager
   */
  static iterator end(Euclid::PhzDataModel::PhotometryCellManager& vector);

  static const bool enable_boost_serialize = true;
}; // end of GridCellManagerTraits

} // end of namespace GridContainer
} // end of namespace Euclid

// Here we include the serialization of the photometry grid. This is done here
// to avoid having the default grid serialization applied to the PhotometryGrid
// (which would happen if the user would forget to include this file)
#include "PhzDataModel/serialization/PhotometryGrid.h"

#endif	/* PHZDATAMODEL_PHOTOMETRYGRID_H */

