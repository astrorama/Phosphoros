/** 
 * @file LikelihoodAlgorithm.cpp
 * @date November 30, 2014
 * @author Nikolaos Apostolakos
 */

#include "PhzLikelihood/LikelihoodAlgorithm.h"

namespace Euclid {
namespace PhzLikelihood {

LikelihoodAlgorithm::LikelihoodAlgorithm(ScaleFactorCalc scale_factor_calc, LikelihoodCalc likelihood_calc)
      : m_scale_factor_calc{std::move(scale_factor_calc)}, m_likelihood_calc{std::move(likelihood_calc)} { }

} // end of namespace PhzLikelihood
} // end of namespace Euclid