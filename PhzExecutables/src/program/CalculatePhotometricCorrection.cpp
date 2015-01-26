/**
 * @file CalculatePhotometricCorrection.cpp
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "ElementsKernel/Program.h"
#include "ElementsKernel/Logging.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzConfiguration/CalculatePhotometricCorrectionConfiguration.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"
#include "PhzPhotometricCorrection/FindBestFitModels.h"
#include "PhzPhotometricCorrection/CalculateScaleFactorMap.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionAlgorithm.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::PhzPhotometricCorrection;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("CalculatePhotometricCorrection");

class CalculatePhotometricCorrection : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::CalculatePhotometricCorrectionConfiguration::getProgramOptions();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    PhzConfiguration::CalculatePhotometricCorrectionConfiguration conf {args};
    auto catalog = conf.getCatalog();
    auto model_phot_grid = conf.getPhotometryGrid();
    auto output_func = conf.getOutputFunction();
    auto stop_criteria = conf.getStopCriteria();

    FindBestFitModels<PhzLikelihood::SourcePhzFunctor> find_best_fit_models {};
    CalculateScaleFactorMap calculate_scale_factor_map {};
    PhotometricCorrectionAlgorithm phot_corr_algorighm {};
    auto selector = conf.getPhotometricCorrectionSelector();

    PhotometricCorrectionCalculator calculator {find_best_fit_models,
                                calculate_scale_factor_map, phot_corr_algorighm};

    auto progress_logger = [](size_t iter_no, const PhzDataModel::PhotometricCorrectionMap& phot_corr) {
      logger.info() << "Iteration no: " << iter_no;
      for (auto& pair : phot_corr) {
        logger.info() << pair.first.qualifiedName() << " : " << pair.second;
      }
    };
    auto phot_corr_map = calculator(catalog, model_phot_grid, stop_criteria, selector, progress_logger);

    output_func(phot_corr_map);

    logger.info() << "Done!";
    return Elements::ExitCode::OK;
  }

  string getVersion() override {
    return "0.1";
  }

};

MAIN_FOR(CalculatePhotometricCorrection)
