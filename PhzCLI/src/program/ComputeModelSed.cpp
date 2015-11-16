/**
 * @file ComputeModelSed.cpp
 * @date January 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include <chrono>
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"

#include "PhzDataModel/PhzModel.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzCLI/ComputeModelSedConfig.h"

using namespace std;
using namespace Euclid::PhzCLI;
using namespace Euclid::Configuration;
using namespace Euclid::PhzConfiguration;
using namespace Euclid::PhzModeling;
using namespace Euclid::XYDataset;
using namespace Euclid::MathUtils;
using namespace Euclid::PhzDataModel;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("ComputeModelSed");

static long config_manager_id = std::chrono::duration_cast<std::chrono::microseconds>(
                                    std::chrono::system_clock::now().time_since_epoch()).count();

class ComputeModelSed : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeModelSedConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);
    
    auto& grid_axes = config_manager.getConfiguration<ComputeModelSedConfig>().getGridAxes();
    std::map<QualifiedName, XYDataset> sed_map {};
    for (auto& pair : config_manager.getConfiguration<ComputeModelSedConfig>().getSedMap()) {
      std::vector<std::pair<double, double>> values {pair.second.begin(), pair.second.end()};
      sed_map.emplace(pair.first, values);
    }
    std::map<QualifiedName, std::unique_ptr<Function>> red_curve_map {};
    for (auto& pair : config_manager.getConfiguration<ComputeModelSedConfig>().getReddeningCurveMap()) {
      red_curve_map.emplace(pair.first, pair.second->clone());
    }
    auto& igm_function = config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionFunction();

    ModelDatasetGrid grid {grid_axes, std::move(sed_map), std::move(red_curve_map),
                               ExtinctionFunctor{}, RedshiftFunctor{}, igm_function};

    for (auto iter=grid.begin(); iter!=grid.end(); ++iter) {
      cout << "\nDataset for model with:\n";
      cout << "SED      " << iter.axisValue<ModelParameter::SED>().qualifiedName() << '\n';
      cout << "REDCURVE " << iter.axisValue<ModelParameter::REDDENING_CURVE>().qualifiedName() << '\n';
      cout << "EBV      " << iter.axisValue<ModelParameter::EBV>() << '\n';
      cout << "Z        " << iter.axisValue<ModelParameter::Z>() << '\n';
      cout << "IGM      " << config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionType() << '\n';
      cout << "\nData:\n";
      for (auto& pair : *iter) {
        cout << pair.first << '\t' << pair.second << '\n';
      }
    }
    cout << '\n';

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputeModelSed)
