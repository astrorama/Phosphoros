/** 
 * @file PhotometricCorrectionCalculator.cpp
 * @date January 20, 2015
 * @author Nikolaos Apostolakos
 */

#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

PhotometricCorrectionCalculator::PhotometricCorrectionCalculator(
                    FindBestFitModelsFunction find_best_fit_models,
                    CalculateScaleFactorsMapFunction calculate_scale_factors_map,
                    CalculatePhotometricCorrectionFunction calculate_photometric_correction)
        : m_find_best_fit_models{std::move(find_best_fit_models)},
          m_calculate_scale_factors_map{std::move(calculate_scale_factors_map)},
          m_calculate_photometric_correction{std::move(calculate_photometric_correction)} {
}
          
PhzDataModel::PhotometricCorrectionMap createInitialPhotCorr(const SourceCatalog::Catalog& catalog) {
  PhzDataModel::PhotometricCorrectionMap phot_corr {};
  auto source_phot = catalog.begin()->getAttribute<SourceCatalog::Photometry>();
  for (auto iter=source_phot->begin(); iter!=source_phot->end(); ++iter) {
    phot_corr.emplace(iter.filterName(), 1.);
  }
  return phot_corr;
}

PhzDataModel::PhotometricCorrectionMap PhotometricCorrectionCalculator::operator ()(
                              const SourceCatalog::Catalog& catalog,
                              const PhzDataModel::PhotometryGrid& model_phot_grid,
                              ProgressListener progress_listener,
                              StopCriteriaFunction stop_criteria_func) {
  PhzDataModel::PhotometricCorrectionMap phot_corr = createInitialPhotCorr(catalog);
  size_t counter {0};
  while(!stop_criteria_func(phot_corr)) {
    auto best_fit_model_map = m_find_best_fit_models(
                                            catalog, model_phot_grid, phot_corr);
    auto scale_factor_map = m_calculate_scale_factors_map(
                                            catalog.begin(), catalog.end(),
                                            best_fit_model_map);
    phot_corr = m_calculate_photometric_correction(
                                            catalog.begin(), catalog.end(),
                                            scale_factor_map, best_fit_model_map);
    if (progress_listener) {
      progress_listener(++counter, phot_corr);
    }
  }
  return phot_corr;
}

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid