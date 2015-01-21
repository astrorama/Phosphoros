/** 
 * @file PhotometricCorrectionCalculator.h
 * @date January 20, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZPHOTOMETRICCORRECTION_PHOTOMETRICCORRECTIONCALCULATOR_H
#define	PHZPHOTOMETRICCORRECTION_PHOTOMETRICCORRECTIONCALCULATOR_H

#include <map>
#include <functional>
#include "SourceCatalog/Catalog.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzPhotometricCorrection/DefaultStopCriteria.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

class PhotometricCorrectionCalculator {

public:
  
  typedef std::function<void(size_t iter_no, const PhzDataModel::PhotometricCorrectionMap& phot_corr)> ProgressListener;
  
  typedef std::function<std::map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>(
                  const SourceCatalog::Catalog& calibration_catalog,
                  const PhzDataModel::PhotometryGrid& model_photometric_grid,
                  const PhzDataModel::PhotometricCorrectionMap& photometric_correction
            )> FindBestFitModelsFunction;
  
  typedef std::function<std::map<int64_t, double>(
                  SourceCatalog::Catalog::const_iterator source_begin,
                  SourceCatalog::Catalog::const_iterator source_end,
                  const std::map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>& model_phot_map
            )> CalculateScaleFactorsMapFunction;
  
  typedef std::function<PhzDataModel::PhotometricCorrectionMap(
                  SourceCatalog::Catalog::const_iterator source_begin,
                  SourceCatalog::Catalog::const_iterator source_end,
                  const std::map<int64_t, double>& scale_factor_map,
                  const std::map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>& model_phot_map
            )> CalculatePhotometricCorrectionFunction;
  
  typedef std::function<bool(const PhzDataModel::PhotometricCorrectionMap& phot_corr)> StopCriteriaFunction;
  
  PhotometricCorrectionCalculator(FindBestFitModelsFunction find_best_fit_models,
                                  CalculateScaleFactorsMapFunction calculate_scale_factors_map,
                                  CalculatePhotometricCorrectionFunction calculate_photometric_correction);
  
  PhzDataModel::PhotometricCorrectionMap operator()(
          const SourceCatalog::Catalog& catalog,
          const PhzDataModel::PhotometryGrid& model_phot_grid,
          StopCriteriaFunction stop_criteria_func,
          ProgressListener progress_listener=ProgressListener{});
  
private:
  
  FindBestFitModelsFunction m_find_best_fit_models;
  CalculateScaleFactorsMapFunction m_calculate_scale_factors_map;
  CalculatePhotometricCorrectionFunction m_calculate_photometric_correction;
  
};

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid

#endif	/* PHZPHOTOMETRICCORRECTION_PHOTOMETRICCORRECTIONCALCULATOR_H */

