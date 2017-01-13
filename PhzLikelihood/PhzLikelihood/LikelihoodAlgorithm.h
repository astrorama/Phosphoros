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
 * @class LikelihoodAlgorithm
 * 
 * @brief Calculates the likelihood of a source over a set of models
 * 
 * @details
 * The Likelihood Algorithm component is an STL-like algorithm responsible for
 * calculating the likelihood of a source over a set of models, based on the
 * source and models photometries. The assumption of the algorithm is that all
 * the models provide photometries for the same filters and in the same order.
 * For the source the only restriction is that it contains photometries
 * for all the filters that the models have (order doesn't matter and it might
 * contain extra photometries).
 * 
 * Note that the magnitude of the source and the models is not taken in
 * consideration. A scale factor is used to scale the model photometries to best
 * fit the given source photometry. The way of calculating this factor is by
 * using a function of type ScaleFactorCalc given at construction time.
 * 
 * The second way the algorithm can be parameterised is the likelihood
 * calculation method. For this is used the
 * function LikelihoodCalc, as given by the constructor.
 */
class LikelihoodAlgorithm {

public:

  /**
   * Definition of the function for calculating the factor with which a model
   * photometry must be scaled to best fit the source. It is a function which
   * gets the iterators over the source and the model photometries and it returns
   * the scale factor as a double. It makes the assumption that the iterators
   * will iterate over the photometries of the same filters and in the same
   * order.
   */
  typedef std::function<double(SourceCatalog::Photometry::const_iterator source_begin,
                               SourceCatalog::Photometry::const_iterator source_end,
                               SourceCatalog::Photometry::const_iterator model_begin)
                       > ScaleFactorCalc;

  /**
   * Definition of the function for calculating the likelihood a source to be a
   * like a model. It is a function which gets the iterators over the source and
   * the model photometries, as well as a factor to scale the model photometry
   * with, and returns the likelihood as a double. It makes the assumption that
   * the iterators will iterate over the photometries of the same filters and in
   * the same order.
   */
  typedef std::function<double(SourceCatalog::Photometry::const_iterator source_begin,
                               SourceCatalog::Photometry::const_iterator source_end,
                               SourceCatalog::Photometry::const_iterator model_begin,
                               double scale_factor)
                       > LikelihoodCalc;

  /**
   * Constructs a new instance of LikelihoodAlgorithm.
   * @param scale_factor_calc The function to use for calculating the scale
   *        factor of each model 
   * @param likelihood_calc The function to use for calculating the likelihood
   *        of a source to fit a model
   */
  LikelihoodAlgorithm(ScaleFactorCalc scale_factor_calc, LikelihoodCalc likelihood_calc);

  /**
   * Calculates the likelihood of a given source to fit a set of models. The
   * models are iterated by using the given iterator. They are all assumed to
   * provide photometry for the same filters and in the same order. The source
   * photometry is required to contain information for these filters (but the
   * order does not matter and there might be extra filters). The output
   * likelihood is stored using the likelihood_begin iterator.
   * 
   * @tparam ModelIter
   *    The type of the iterator over the model photometries. It
   *    must be an iterator over SourceCatalog::Photometry objects.
   * @tparam LikelihoodIter
   *    The type of the iterator which is used for returning
   *    the likelihood values. It must be an iterator over a collection of
   *    doubles, which has the same size as the number of models.
   * @param source_photometry 
   *    The photometry of the source to calculate the likelihood for
   * @param model_begin
   *    The iterator pointing at the first of the models
   * @param model_end
   *    The iterator pointing at one after the last of the models
   * @param likelihood_begin
   *    The iterator pointing at the first element of the collection which will
   *    be used as the likelihood output
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

