/*
 * FindMedianPhotometricCorrectionsFunctor.h
 *
 *  Created on: Jan 22, 2015
 *      Author: fdubath
 */

#ifndef FINDMEDIANPHOTOMETRICCORRECTIONSFUNCTOR_H_
#define FINDMEDIANPHOTOMETRICCORRECTIONSFUNCTOR_H_

#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzPhotometricCorrection {




class FindMedianPhotometricCorrectionsFunctor{
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


#include "PhzPhotometricCorrection/_impl/FindMedianPhotometricCorrectionsFunctor.icpp"

#endif /* FINDMEDIANPHOTOMETRICCORRECTIONSFUNCTOR_H_ */
