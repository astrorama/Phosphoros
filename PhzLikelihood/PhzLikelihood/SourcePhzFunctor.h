/** 
 * @file SourcePhzFunctor.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H
#define	PHZLIKELIHOOD_SOURCEPHZFUNCTOR_H

#include <functional>
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzLikelihood {

class SourcePhzFunctor {
  
public:
  
  typedef std::function<void(const SourceCatalog::Photometry& source_photometry,
                             PhzDataModel::PhotometryGrid::const_iterator model_begin,
                             PhzDataModel::PhotometryGrid::const_iterator model_end,
                             PhzDataModel::LikelihoodGrid::iterator likelihood_begin)
                       > LikelihoodFunction;
  
  typedef std::function<PhzDataModel::LikelihoodGrid::iterator(
                              PhzDataModel::LikelihoodGrid::iterator likelihood_begin,
                              PhzDataModel::LikelihoodGrid::iterator likelihood_end)
                       > BestFitSearchFunction;
  
  SourcePhzFunctor(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                   PhzDataModel::PhotometryGrid phot_grid);
  
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

