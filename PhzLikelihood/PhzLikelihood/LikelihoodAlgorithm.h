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
/**
 * @class Euclid::PhzLikelihood::LikelihoodAlgorithm
 * @brief
 * This algorithm compute the likelihood for a source with respect to a set of models.
 */
class LikelihoodAlgorithm {

public:

  /**
   * @brief
   * Alias of the interface of the Scale Factor function.
   */
  typedef std::function<double(SourceCatalog::Photometry::const_iterator source_begin,
                               SourceCatalog::Photometry::const_iterator source_end,
                               SourceCatalog::Photometry::const_iterator model_begin)
                       > ScaleFactorCalc;

  /**
   * @brief
   * Alias of the interface of the Likelihood function.
   */
  typedef std::function<double(SourceCatalog::Photometry::const_iterator source_begin,
                               SourceCatalog::Photometry::const_iterator source_end,
                               SourceCatalog::Photometry::const_iterator model_begin,
                               double scale_factor)
                       > LikelihoodCalc;

  /**
     * @brief Constructor
     * @details
     * Instantiate a LikelihoodAlgorithm with the provided scale factor and likelihood function..
     *
     * @param scale_factor_calc
     * A ScaleFactorCalc function containing the logic for computing the Scale factor of the source
     * with respect to a model.
     *
     * @param likelihood_calc
     * A LikelihoodCalc function containing the logic for computing the Likelihood of the source
     * with respect to a model.
     */
  LikelihoodAlgorithm(ScaleFactorCalc scale_factor_calc, LikelihoodCalc likelihood_calc);

  /**
    * @brief Function Call Operator
    * @details
    * Performs the computation of the likelihood of the source with respect to the set of models.
    *
    * @param source_photometry
    * The source photometry
    *
    * @param model_begin
    * An iterator over the models to be matched with the source
    *
    * @param model_end
    * The end iterator for the models
    *
    * @param likelihood_begin
    * A Likelihood iterator into which the computed Likelihood will be stored.
    *
    */
  template<typename ModelIter, typename LikelihoodIter>
  void operator()(const SourceCatalog::Photometry& source_photometry,
                  ModelIter model_begin, ModelIter model_end,
                  LikelihoodIter likelihood_begin) const;

private:
  /**
    * @brief
    * The Scale Factor function.
    */
  ScaleFactorCalc m_scale_factor_calc;
  /**
    * @brief
    * The Likelihood function.
    */
  LikelihoodCalc m_likelihood_calc;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/LikelihoodAlgorithm.icpp"

#endif	/* PHZLIKELIHOOD_LIKELIHOODALGORITHM_H */

