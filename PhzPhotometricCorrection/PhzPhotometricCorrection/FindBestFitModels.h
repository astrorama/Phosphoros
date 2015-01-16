/**
 * @file PhzPhotometricCorrection/FindBestFitModels.h
 * @date Jan 15, 2015
 * @author Florian Dubath
 */

#ifndef PHZPHOTOMETRICCORRECTION_FINDBESTFITMODELS_H
#define PHZPHOTOMETRICCORRECTION_FINDBESTFITMODELS_H

#include <map>
#include "SourceCatalog/Catalog.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

/**
 * @class FindBestFitModels
 *
 * @brief
 */
template<typename sourceCalculatorFunctor>
class FindBestFitModels {
public:
  std::map<int64_t, PhzDataModel::PhotometryGrid::const_iterator> operator()(
      const SourceCatalog::Catalog& calibration_catalog,
      const PhzDataModel::PhotometryGrid& model_photometric_grid,
      const PhzDataModel::PhotometricCorrectionMap& photometric_correction);
};

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid

#include "PhzPhotometricCorrection/_impl/FindBestFitModels.icpp"

#endif    /* PHZPHOTOMETRICCORRECTION_FINDBESTFITMODELS_H */
