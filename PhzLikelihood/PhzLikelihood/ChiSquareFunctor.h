/** 
 * @file LikelihoodFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_LIKELIHOODFUNCTOR_H
#define	PHZLIKELIHOOD_LIKELIHOODFUNCTOR_H

namespace Euclid {
namespace PhzLikelihood {

class ChiSquareFunctor {
  
public:
  
  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter source_begin, SourceIter source_end,
                    ModelIter model_begin, double scale) const;
  
};

} // end of namespce PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/ChiSquareFunctor.icpp"

#endif	/* PHZLIKELIHOOD_LIKELIHOODFUNCTOR_H */

