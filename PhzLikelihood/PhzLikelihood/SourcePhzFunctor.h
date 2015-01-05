/** 
 * @file SourcePhzFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H
#define	PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H

#include <algorithm>
#include <functional>
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzLikelihood/LikelihoodAlgorithm.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"
#include "PhzLikelihood/ChiSquareFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

/**
 * @class SourcePhzFunctor
 * 
 * @brief Class responsible for producing the PHZ outputs for a single source
 * 
 * @details
 * This class operates as a functor, which recieves a source photometry and
 * returns the PHZ results. At the moment it only returns an iterator to the
 * model which is the best match (no PDF is returned at the moment).
 * 
 * The calculation of the PHZ results is performed in the following way. First,
 * the photometric correction is applied to the given source photometry. Then,
 * a grid containing the model photometries is used for creating a likelihood
 * grid of the corrected source photometry. This likelihood grid is used for
 * finding the best fitted model.
 */
class SourcePhzFunctor {
  
public:
  
  /**
   * Definition of the STL-like algorithm for calculating the likelihood grid.
   * It is a function which gets a source photometry and an iterator over the
   * model photometries and populates the likelihood results using a likelihood
   * iterator.
   */
  typedef std::function<void(const SourceCatalog::Photometry& source_photometry,
                             PhzDataModel::PhotometryGrid::const_iterator model_begin,
                             PhzDataModel::PhotometryGrid::const_iterator model_end,
                             PhzDataModel::LikelihoodGrid::iterator likelihood_begin)
                       > LikelihoodFunction;
  
  /**
   * Definition of the STL-like algorithm for finding the best fitted model. It
   * gets as parameters the iterator over the likelihood grid and it returns an
   * iterator pointing to the best fitted model.
   */
  typedef std::function<PhzDataModel::LikelihoodGrid::iterator(
                              PhzDataModel::LikelihoodGrid::iterator likelihood_begin,
                              PhzDataModel::LikelihoodGrid::iterator likelihood_end)
                       > BestFitSearchFunction;
  
  /**
   * Constructs a new SourcePhzFunctor instance. It gets as parameters a map
   * containing the photometric corrections, the grid with the model photometries,
   * the algorithm to use for calculating the likelihood grid and the algorithm
   * for finding the best fitted model. Note that the algorithms can be ommitted
   * which will result to the default chi^2 for the likelihood calculation and
   * the maximum likelihood value for the best fitted model.
   * 
   * @param phot_corr_map
   *    The map with the photometric corrections
   * @param phot_grid
   *    The grid with the model photometries
   * @param likelihood_func
   *    The STL-like algorithm for calculating the likelihood grid
   * @param best_fit_search_func
   *    The STL-like algorithm for finding the best fitted model
   */
  SourcePhzFunctor(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                   PhzDataModel::PhotometryGrid phot_grid,
                   LikelihoodFunction likelihood_func = LikelihoodAlgorithm{ScaleFactorFunctor{}, ChiSquareFunctor{}},
                   BestFitSearchFunction best_fit_search_func = std::max_element<PhzDataModel::LikelihoodGrid::iterator>);
  
  /**
   * Calculates the PHZ results for the given source photometry. The given
   * photometry is first being corrected by using the photometric corrections
   * and then its likelihood grid is calculated by using the likelihood algorithm
   * (as given at the constructor). The order of the filters of the source
   * photometry does not matter, but the SourcePhzFunctor instance must contain
   * photometric corrections for all of them, otherwise an ElementsException
   * will be thrown. The second requirement is that the source photometry must
   * contain information for all the filters of the model photometries.
   * 
   * The current implementation of the SourcePhzFunctor will only calculate
   * the best fitted model, which is returned as an iterator to the photometry
   * grid given at the constructor.
   * 
   * @param source_phot
   *    The photometry of the source
   * @return 
   *    An iterator pointing to the best fitted model
   * @throws ElementsException
   *    if the source photometry contains filters for which the SourcePhzFunctor
   *    has no photometric corrections
   * @throws ElementsException
   *    if the source photometry does not contain information for all the filters
   *    of the model photometries
   */
  PhzDataModel::PhotometryGrid::const_iterator operator()(
                            const SourceCatalog::Photometry& source_phot) const;
  
private:
  
  PhzDataModel::PhotometricCorrectionMap m_phot_corr_map;
  PhzDataModel::PhotometryGrid m_phot_grid;
  LikelihoodFunction m_likelihood_func;
  BestFitSearchFunction m_best_fit_search_func;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#endif	/* PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H */

