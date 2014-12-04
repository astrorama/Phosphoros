/** 
 * @file SourcePhzFunctor.cpp
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#include <algorithm>
#include <tuple>
#include "ElementsKernel/Exception.h"
#include "GridContainer/_impl/TemplateLoopCounter.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzLikelihood/LikelihoodAlgorithm.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"
#include "PhzLikelihood/ChiSquareFunctor.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

SourcePhzFunctor::SourcePhzFunctor(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                                   PhzDataModel::PhotometryGrid phot_grid)
        : m_phot_corr_map{std::move(phot_corr_map)}, m_phot_grid{std::move(phot_grid)},
          m_likelihood_func{LikelihoodAlgorithm{ScaleFactorFunctor{}, ChiSquareFunctor{}}},
          m_best_fit_search_func{std::max_element<PhzDataModel::LikelihoodGrid::iterator>} {
}

SourceCatalog::Photometry applyPhotCorr(const PhzDataModel::PhotometricCorrectionMap& pc_map,
                                        const SourceCatalog::Photometry source_phot) {
  std::shared_ptr<std::vector<std::string>> filter_names_ptr {new std::vector<std::string>{}};
  std::vector<SourceCatalog::FluxErrorPair> fluxes {};
  for (auto iter = source_phot.begin(); iter != source_phot.end(); ++iter) {
    auto filter_name = iter.filterName();
    auto pc = pc_map.find(filter_name);
    if (pc == pc_map.end()) {
      throw Elements::Exception() << "Source does not contain photometry for " << filter_name;
    }
    filter_names_ptr->push_back(std::move(filter_name));
    fluxes.emplace_back((*pc).second * (*iter).flux, (*iter).error);
  }
  return SourceCatalog::Photometry{filter_names_ptr, std::move(fluxes)};
}

template<typename IterFrom, typename IterTo>
class GridIterHelper {
  
public:

  template<int I>
  void fixSameAxis(IterFrom& from, IterTo& to, const GridContainer::TemplateLoopCounter<I>&) {
    to.template fixAxisByIndex<I>(from.template axisIndex<I>());
    fixSameAxis(from, to, GridContainer::TemplateLoopCounter<I-1>{});
  }

  void fixSameAxis(IterFrom&, IterTo&, const GridContainer::TemplateLoopCounter<-1>&) {
  }
//  
};

template<typename IterFrom, typename IterTo>
void fixSameAxis(IterFrom& from, IterTo& to) {
  GridIterHelper< IterFrom, IterTo> helper {};
  helper.fixSameAxis(from, to, GridContainer::TemplateLoopCounter<std::tuple_size<PhzDataModel::ModelAxesTuple>::value - 1>{});
}

PhzDataModel::PhotometryGrid::const_iterator SourcePhzFunctor::operator()(
                          const SourceCatalog::Photometry& source_phot) const {
  // Apply the photometric correction to the given source photometry
  auto cor_source_phot = applyPhotCorr(m_phot_corr_map, source_phot);
  // Create a new likelihood grid, with all cells set to 0
  PhzDataModel::LikelihoodGrid likelihood_grid {m_phot_grid.getAxesTuple()};
  // Calculate the likelihood over the grid
  m_likelihood_func(cor_source_phot, m_phot_grid.begin(), m_phot_grid.end(), likelihood_grid.begin());
  // Select the best fitted model
  auto best_fit = m_best_fit_search_func(likelihood_grid.begin(), likelihood_grid.end());
  // Create an iterator of PhotometryGrid instead of the LikelihoodGrid that we have
  auto result = m_phot_grid.begin();
  fixSameAxis(best_fit, result);
  return result;
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid