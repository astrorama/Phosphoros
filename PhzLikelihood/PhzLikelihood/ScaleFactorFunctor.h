/**
 * @file ScaleFactorFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SCALEFACTORFUNCTOR_H
#define	PHZLIKELIHOOD_SCALEFACTORFUNCTOR_H

namespace Euclid {
namespace PhzLikelihood {
/**
 * @class Euclid::PhzLikelihood::ScaleFactorFunctor
 * @brief
 * This functor is in charge of computing the Scale Factor of the source
 * @details
 * For a single Filter the Scale Factor is the ratio between the source flux and the model flux.
 * When multiple filter are available the Scale Factor is computed as the ratio between the
 * weighted sum of the source flux and the weighted sum of the model flux.
 * The applied weight being the model flux over the (source) error squared.
 * The model is assumed to be error free.
 */
class ScaleFactorFunctor {

public:

  /**
    * @brief Function Call Operator
    * @details
    * Compute the Scale Factor of a source
    *
    * @param source_begin
    * An iterator over the Filters of the source exposing an object having "flux" and "error" fields.

    * @param source_end
    * End iterator for the Filters of the source.
    *
    * @param model_begin
    * An iterator over the Filters of the model exposing an object containing the model "flux".
    *
    * @return
    * The computed Scale Factor as a double.
    */
  template<typename SourceIter, typename ModelIter>
  double operator()(SourceIter source_begin, SourceIter source_end,
                    ModelIter model_begin) const;

};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/ScaleFactorFunctor.icpp"

#endif	/* PHZLIKELIHOOD_SCALEFACTORFUNCTOR_H */

