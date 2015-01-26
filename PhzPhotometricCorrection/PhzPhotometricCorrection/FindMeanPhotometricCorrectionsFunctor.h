/** 
 * @file FindMeanPhotometricCorrectionsFunctor.h
 * @date January 23, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHOTOMETRICCORRECTION_FINDMEANPHOTOMETRICCORRECTIONSFUNCTOR_H
#define	PHOTOMETRICCORRECTION_FINDMEANPHOTOMETRICCORRECTIONSFUNCTOR_H

#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

class FindMeanPhotometricCorrectionsFunctor {
  
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


#include "PhzPhotometricCorrection/_impl/FindMeanPhotometricCorrectionsFunctor.icpp"

#endif	/* PHOTOMETRICCORRECTION_FINDMEANPHOTOMETRICCORRECTIONSFUNCTOR_H */

