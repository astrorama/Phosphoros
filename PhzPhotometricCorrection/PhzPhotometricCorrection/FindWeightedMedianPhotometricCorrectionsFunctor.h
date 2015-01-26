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

class FindWeightedMedianPhotometricCorrectionsFunctor {
  
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


#include "PhzPhotometricCorrection/_impl/FindWeightedMedianPhotometricCorrectionsFunctor.icpp"

#endif	/* PHZPHOTOMETRICCORRECTION_FINDWEIGHTEDMEDIANPHOTOMETRICCORRECTIONSFUNCTOR_H */

