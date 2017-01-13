/**
 * @file FindWeightedMeanPhotometricCorrectionsFunctor.h
 * @date January 23, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHOTOMETRICCORRECTION_FINDWEIGHTEDMEANPHOTOMETRICCORRECTIONSFUNCTOR_H
#define	PHOTOMETRICCORRECTION_FINDWEIGHTEDMEANPHOTOMETRICCORRECTIONSFUNCTOR_H

#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzPhotometricCorrection {
/**
 * @class FindWeightedMeanPhotometricCorrectionsFunctor
 *
 * @brief
 * This functor extract the weighted mean values for the Photometric Corrections
 * (one for each filter) based on the individual source's corrections. The weights
 * are obtained at the inverse of the relative error 1/(Delta Flux/Flux)
 */
class FindWeightedMeanPhotometricCorrectionsFunctor {

public:

  /**
    * @brief Compute the global photometric corrections by taking the  weighted
    * mean over the sources.
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


#include "PhzPhotometricCorrection/_impl/FindWeightedMeanPhotometricCorrectionsFunctor.icpp"

#endif	/* PHOTOMETRICCORRECTION_FINDWEIGHTEDMEANPHOTOMETRICCORRECTIONSFUNCTOR_H */

