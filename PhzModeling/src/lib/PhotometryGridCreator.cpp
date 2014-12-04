/**
 * @file PhzModeling/PhotometryGridCreator.cpp
 * @date Oct 20 2014
 * @author Florian Dubath
 */

#include <future>
#include <atomic>
#include <thread>
#include <atomic>
#include "ElementsKernel/Logging.h"
#include "MathUtils/interpolation/interpolation.h"

#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/PhotometryGrid.h"

#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/CalculateFluxFunctor.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "PhzModeling/PhotometryAlgorithm.h"

#include "PhzModeling/PhotometryGridCreator.h"


namespace Euclid {
namespace PhzModeling {

static Elements::Logging logger = Elements::Logging::getLogger("PhotometryGridCreator");

template<typename NameIter>
std::map<XYDataset::QualifiedName, XYDataset::XYDataset> buildMap(
          XYDataset::XYDatasetProvider& provider, NameIter begin, NameIter end) {
  std::map<XYDataset::QualifiedName, XYDataset::XYDataset> result {};
  while (begin != end) {
    auto dataset_ptr = provider.getDataset(*begin);
    if (!dataset_ptr) {
      throw Elements::Exception() << "Failed to find dataset: " << begin->qualifiedName();
    }
    result.insert(std::make_pair(*begin, std::move(*dataset_ptr)));
    ++begin;
  }
  return result;
}

std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>>
    convertToFunction(const std::map<XYDataset::QualifiedName, XYDataset::XYDataset>& dataset_map) {
  std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>> result {};
  for (auto& pair : dataset_map) {
    auto function_ptr = MathUtils::interpolate(pair.second, MathUtils::InterpolationType::LINEAR);
    result.emplace(pair.first, std::move(function_ptr));
  }
  return result;
}

PhotometryGridCreator::PhotometryGridCreator(
              std::unique_ptr<XYDataset::XYDatasetProvider> sed_provider,
              std::unique_ptr<XYDataset::XYDatasetProvider> reddening_curve_provider,
              std::unique_ptr<XYDataset::XYDatasetProvider> filter_provider)
      : m_sed_provider{std::move(sed_provider)},
        m_reddening_curve_provider{std::move(reddening_curve_provider)},
        m_filter_provider(std::move(filter_provider)) {
}
        
class ParallelJob {
  
public:

  ParallelJob(PhotometryAlgorithm<ModelFluxAlgorithm>& m_photometry_algo,
              ModelDatasetGrid::iterator model_begin, ModelDatasetGrid::iterator model_end, 
              PhzDataModel::PhotometryGrid::iterator photometry_begin, std::atomic<size_t>& m_progress)
        : m_photometry_algo(m_photometry_algo), m_model_begin(model_begin), m_model_end(model_end),
          m_photometry_begin(photometry_begin), m_progress(m_progress) { }
        
  void operator()() {
    m_photometry_algo(m_model_begin, m_model_end, m_photometry_begin, m_progress);
  }

private:
  
  PhotometryAlgorithm<ModelFluxAlgorithm>& m_photometry_algo;
  ModelDatasetGrid::iterator m_model_begin;
  ModelDatasetGrid::iterator m_model_end;
  PhzDataModel::PhotometryGrid::iterator m_photometry_begin;
  std::atomic<size_t>& m_progress;
  
};

PhzDataModel::PhotometryGrid PhotometryGridCreator::createGrid(
            const PhzDataModel::ModelAxesTuple& parameter_space,
            const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list) {

  // Create the maps
  auto filter_map = buildMap(*m_filter_provider, filter_name_list.begin(), filter_name_list.end());
  auto sed_name_list = std::get<PhzDataModel::ModelParameter::SED>(parameter_space);
  auto sed_map = buildMap(*m_sed_provider, sed_name_list.begin(), sed_name_list.end());
  auto reddening_curve_list = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(parameter_space);
  auto reddening_curve_map = convertToFunction(buildMap(*m_reddening_curve_provider,
                                            reddening_curve_list.begin(), reddening_curve_list.end()));

  // Define the functions and the algorithms based on the Functors
  ModelDatasetGrid::ReddeningFunction reddening_function {ExtinctionFunctor{}};
  ModelDatasetGrid::RedshiftFunction redshift_function {RedshiftFunctor{}};
  ModelFluxAlgorithm::ApplyFilterFunction apply_filter_function {ApplyFilterFunctor{}};
  ModelFluxAlgorithm::CalculateFluxFunction flux_function {CalculateFluxFunctor{MathUtils::InterpolationType::LINEAR}};
  ModelFluxAlgorithm flux_model_algo {std::move(apply_filter_function), std::move(flux_function)};

  // Create the model grid
  auto model_grid= ModelDatasetGrid(parameter_space, std::move(sed_map),std::move(reddening_curve_map),
                                    reddening_function, redshift_function);

  // Create the photometry Grid
  auto photometry_grid = PhzDataModel::PhotometryGrid(parameter_space);

  // Define the algo

  auto photometry_algo = createPhotometryAlgorithm(std::move(flux_model_algo),std::move(filter_map), filter_name_list);

  // Here we keep the futures for the threads we start so we can wait for them
  std::vector<std::future<void>> futures;
  std::atomic<size_t> progress {0};
  size_t total_models = model_grid.size();
  logger.info() << "Creating photometries for " << total_models << " models";
  for (auto& sed : std::get<PhzDataModel::ModelParameter::SED>(parameter_space)) {
    // We start a new thread to handle this SED
    auto model_iter = model_grid.begin();
    model_iter.fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);
    auto photometry_iter = photometry_grid.begin();
    photometry_iter.fixAxisByValue<PhzDataModel::ModelParameter::SED>(sed);

    futures.push_back(std::async(std::launch::async, ParallelJob{photometry_algo, model_iter, model_grid.end(), photometry_iter, progress}));
  }
  while (progress < total_models) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
      int percentage_done = 100. * progress / total_models;
      logger.info() << "Progress: " << percentage_done << " %";
  }
  // Wait for all threads to finish
  for (auto& f : futures) {
    f.get();
  }

  return std::move(photometry_grid);
}


}
}

