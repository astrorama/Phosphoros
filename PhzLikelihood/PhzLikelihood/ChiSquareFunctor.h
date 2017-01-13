/**
 * @file LikelihoodFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_LIKELIHOODFUNCTOR_H
#define	PHZLIKELIHOOD_LIKELIHOODFUNCTOR_H

namespace Euclid {
namespace PhzLikelihood {
/**
 * @class Euclid::PhzLikelihood::ChiSquareFunctor
 * @brief
 * This functor is in charge of computing the Likelihood of a source with respect
 * to a given model. This implementation use the Chi^2 pathway.
 * @details
 * The Likelihood is computed as the exp(-Chi^2/2). The the Chi^2 is computed as
 * the weighted sum over the filters of the square of the difference between the model and the source flux.
 * The weight being the inverse of the source flux error. The model normalization
 * is provided as the "Scale Factor" and this value is computed to minimize the Chi^2.
 */
class ChiSquareFunctor {

public:
  /**
   * @brief Function Call Operator
   * @details
   * Compute the Likelihood for a source with respect to the model using the Chi^2 pathway.
   *
   * @param source_begin
   * An iterator over the Filters of the source exposing an object having "flux" and "error" fields.
   *
   * @param source_end
   * End iterator for the Filters of the source.
   *
   * @param model_begin
   * An iterator over the Filters of the model exposing an object containing the model "flux".
   *
   * @param scale
   * The scale factor, a double.
   * In order to get a meaningful value for the Likelihood, the scale factor
   * must be chosen as the value minimizing the Chi^2 (an analytical formula is available).
   *
   * @return
   * The computed Likelihood as a double.
   */
  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter source_begin, SourceIter source_end,
                    ModelIter model_begin, double scale) const;

};

} // end of namespce PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/ChiSquareFunctor.icpp"

#endif	/* PHZLIKELIHOOD_LIKELIHOODFUNCTOR_H */

