/** 
 * @file ScaleFactorFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SCALEFACTORFUNCTOR_H
#define	PHZLIKELIHOOD_SCALEFACTORFUNCTOR_H

namespace Euclid {
namespace PhzLikelihood {

class ScaleFactorFunctor {
  
public:
  
  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter source_begin, SourceIter source_end,
                    ModelIter model_begin) const;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/ScaleFactorFunctor.icpp"

#endif	/* PHZLIKELIHOOD_SCALEFACTORFUNCTOR_H */

