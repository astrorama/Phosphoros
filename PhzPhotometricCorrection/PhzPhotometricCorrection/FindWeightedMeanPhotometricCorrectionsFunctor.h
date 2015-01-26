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

class FindWeightedMeanPhotometricCorrectionsFunctor {
  
public:
  
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

