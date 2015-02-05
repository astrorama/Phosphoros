/**
 * @file SourcePhzFunctor.cpp
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#include <tuple>
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

SourcePhzFunctor::SourcePhzFunctor(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                                   const PhzDataModel::PhotometryGrid& phot_grid,
                                   MarginalizationFunction marginalization_func,
                                   LikelihoodFunction likelihood_func,
                                   BestFitSearchFunction best_fit_search_func)
        : m_phot_corr_map{std::move(phot_corr_map)}, m_phot_grid(phot_grid),
          m_marginalization_func{std::move(marginalization_func)},
          m_likelihood_func{std::move(likelihood_func)},
          m_best_fit_search_func{std::move(best_fit_search_func)} {
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

auto SourcePhzFunctor::operator()(const SourceCatalog::Photometry& source_phot) const -> result_type {
  // Apply the photometric correction to the given source photometry
  auto cor_source_phot = applyPhotCorr(m_phot_corr_map, source_phot);
  // Create a new likelihood grid, with all cells set to 0
  PhzDataModel::LikelihoodGrid likelihood_grid {m_phot_grid.getAxesTuple()};
  // Calculate the likelihood over the grid
  m_likelihood_func(cor_source_phot, m_phot_grid.begin(), m_phot_grid.end(), likelihood_grid.begin());
  // Select the best fitted model
  auto best_fit = m_best_fit_search_func(likelihood_grid.begin(), likelihood_grid.end());
  // Create an iterator of PhotometryGrid instead of the LikelihoodGrid that we have
  auto best_fit_result = m_phot_grid.begin();
  best_fit_result.fixAllAxes(best_fit);

  // Calculate the 1D PDF
  auto pdf_1D = m_marginalization_func(likelihood_grid);
  // Return the result
  return result_type{best_fit_result, std::move(pdf_1D)};
}

} // end of namespace PhzLikelihood
} // end of namespace Euclid
