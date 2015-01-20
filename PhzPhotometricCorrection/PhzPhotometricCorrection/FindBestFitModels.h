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
 * @brief This class is in charge of getting the best fitted models at a given
 * redshift for each sources of a catalog.
 * The redshift to be chosen is provided for each sources by the
 * catalog along with the photometry.
 *
 * @tparam sourceCalculatorFunctor A type of functor encoding the algorithm for
 * finding the best fitted model.
 * See SourcePhzFunctor::SourcePhzFunctor for the signatures.
 */
template<typename sourceCalculatorFunctor>
class FindBestFitModels {
public:

  /**
     * @brief Map each input source to the model which is the best match,
     * according their photometries. The results are selected from the models
     * which correspond to the known spectroscopic redshift of the source and
     * the models of other reshdifts are ignored.
     *
     * @param calibration_catalog An object of type SourceCatalog::Catalog,
     * which contains the sources. All the sources are assumed to contain both
     * Photometry and Spec-Z information.
     *
     * @param model_photometric_grid  An object of type PhzDataModel::PhotometryGrid,
     * which contain the photometries of the models.
     *
     * @param photometric_correction An object of type
     * PhzDataModel::PhotometricCorrectionMap, containing the photometric corrections
     * for all filters.
     *
     * @return An object of type std::map, with keys of type int64 t,
     * representing the IDs of the sources, and values of type
     * PhzDataModel::PhotometryGrid::const iterator, pointing to a cell of the
     * input Model Photometry Grid, representing the best fitted model for the source.
     *
     * @throws ElementsException
     *    if any of the source is missing the Spec-Z information
     * @throws ElementsException
     *    if any of the source is missing the Photometry information
     */
  std::map<int64_t, PhzDataModel::PhotometryGrid::const_iterator> operator()(
      const SourceCatalog::Catalog& calibration_catalog,
      const PhzDataModel::PhotometryGrid& model_photometric_grid,
      const PhzDataModel::PhotometricCorrectionMap& photometric_correction);
};

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid

#include "PhzPhotometricCorrection/_impl/FindBestFitModels.icpp"

#endif    /* PHZPHOTOMETRICCORRECTION_FINDBESTFITMODELS_H */
