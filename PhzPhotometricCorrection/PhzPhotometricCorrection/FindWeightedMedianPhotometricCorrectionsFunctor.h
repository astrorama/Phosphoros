/**
 * @file FindWeightedMedianPhotometricCorrectionsFunctor.h
 * @date January 23, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZPHOTOMETRICCORRECTION_FINDWEIGHTEDMEDIANPHOTOMETRICCORRECTIONSFUNCTOR_H
#define	PHZPHOTOMETRICCORRECTION_FINDWEIGHTEDMEDIANPHOTOMETRICCORRECTIONSFUNCTOR_H

#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzPhotometricCorrection {
/**
 * @class FindWeightedMedianPhotometricCorrectionsFunctor
 *
 * @brief
 * This functor extract the weighted median values for the Photometric Corrections
 * (one for each filter) based on the individual source's corrections. The weights
 * are obtained at the inverse of the relative error 1/(Delta Flux/Flux)
 */
class FindWeightedMedianPhotometricCorrectionsFunctor {

public:
  /**
     * @brief Compute the global photometric corrections by taking the  weighted
     * median over the sources.
     *
     * @tparam SourceIter The type of the iterator over the source objects
     *
     * @param source_phot_corr_map A map associating the source id to the photometric
     * correction map for this source.
     *
     * @param source_begin An iterator to the first of the sources.
     * Sources records must have the Photometry attribute defined with non-zero errors.
     *
     * @param source_end An iterator to one after the last of the sources
     *
     * @return The calcualted photometric corrections
     */
  template <typename SourceIter>
  PhzDataModel::PhotometricCorrectionMap operator()(
      const std::map<int64_t, PhzDataModel::PhotometricCorrectionMap>& source_phot_corr_map,
      SourceIter source_begin,
      SourceIter source_end
  );

};

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid


#include "PhzPhotometricCorrection/_impl/FindWeightedMedianPhotometricCorrectionsFunctor.icpp"

#endif	/* PHZPHOTOMETRICCORRECTION_FINDWEIGHTEDMEDIANPHOTOMETRICCORRECTIONSFUNCTOR_H */

