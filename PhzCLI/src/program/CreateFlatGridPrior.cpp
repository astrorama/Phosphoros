/*  
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */  
/**
 * @file src/program/CreateFlatGridPrior.cpp
 * @date 01/22/16
 * @author nikoapos
 */
#include <map>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "GridContainer/serialize.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzCLI/CreateFlatGridPriorConfig.h"
using namespace std;
using namespace Euclid;
namespace po = boost::program_options;
namespace fs = boost::filesystem;
static Elements::Logging logger = Elements::Logging::getLogger("CreateFlatGridPrior");
static long config_manager_id = Euclid::Configuration::getUniqueManagerId();
class CreateFlatGridPrior : public Elements::Program {
public:
  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<PhzCLI::CreateFlatGridPriorConfig>();
    config_manager.registerConfiguration<PhzConfiguration::PhotometryGridConfig>();
    return config_manager.closeRegistration();
  }
  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {
    
    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);
    
    fs::path out_file = config_manager.getConfiguration<PhzCLI::CreateFlatGridPriorConfig>()
                                                  .getGridPriorFilename();
    fs::remove(out_file);
    fs::create_directories(out_file.parent_path());
    
    auto& regions = config_manager.getConfiguration<PhzConfiguration::PhotometryGridConfig>()
                                                    .getPhotometryGridInfo().region_axes_map;
    for (auto& pair : regions) {
      PhzDataModel::DoubleGrid grid {pair.second};
      for (auto& cell : grid) {
        cell = 1;
      }
      GridContainer::gridFitsExport(out_file, pair.first, grid);
    }
    
    logger.info() << "Created file " << out_file.string();
    
    return Elements::ExitCode::OK;
  }
};
MAIN_FOR(CreateFlatGridPrior)
