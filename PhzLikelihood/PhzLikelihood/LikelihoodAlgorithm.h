/** 
 * @file LikelihoodAlgorithm.h
 * @date November 30, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_LIKELIHOODALGORITHM_H
#define	PHZLIKELIHOOD_LIKELIHOODALGORITHM_H

#include <functional>
#include "SourceCatalog/SourceAttributes/Photometry.h"

namespace Euclid {
namespace PhzLikelihood {

class LikelihoodAlgorithm {
  
public:
  
  typedef std::function<double(SourceCatalog::Photometry::const_iterator source_begin,
                               SourceCatalog::Photometry::const_iterator source_end,
                               SourceCatalog::Photometry::const_iterator model_begin)
                       > ScaleFactorCalc;
  
  typedef std::function<double(SourceCatalog::Photometry::const_iterator source_begin,
                               SourceCatalog::Photometry::const_iterator source_end,
                               SourceCatalog::Photometry::const_iterator model_begin,
                               double scale_factor)
                       > LikelihoodCalc;
  
  LikelihoodAlgorithm(ScaleFactorCalc scale_factor_calc, LikelihoodCalc likelihood_calc);
  
  template<typename ModelIter, typename LikelihoodIter>
  void operator()(const SourceCatalog::Photometry& source_photometry,
                  ModelIter model_begin, ModelIter model_end,
                  LikelihoodIter likelihood_begin) const;
  
private:
  
  ScaleFactorCalc m_scale_factor_calc;
  LikelihoodCalc m_likelihood_calc;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/LikelihoodAlgorithm.icpp"

#endif	/* PHZLIKELIHOOD_LIKELIHOODALGORITHM_H */

