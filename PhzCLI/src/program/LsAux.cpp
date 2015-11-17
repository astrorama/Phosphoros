/** 
 * @file LsAux.cpp
 * @date January 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include <set>
#include <chrono>
#include <boost/program_options/options_description.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzCLI/LsAuxDirConfig.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PhzCLI;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosLsAux");

static long config_manager_id = std::chrono::duration_cast<std::chrono::microseconds>(
                                    std::chrono::system_clock::now().time_since_epoch()).count();

class LsAux : public Elements::Program {
  
  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<LsAuxDirConfig>();
    return config_manager.closeRegistration();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);
    
    auto& conf = config_manager.getConfiguration<LsAuxDirConfig>();
    
    auto provider = conf.getDatasetProvider();
    
    if (!conf.showData()) {
      string group = conf.getGroup();
      // Order the results
      auto unordered = provider->listContents(group);
      set<XYDataset::QualifiedName, XYDataset::QualifiedName::AlphabeticalComparator>
          ordered_set {unordered.begin(), unordered.end()};
      for (auto& name : ordered_set) {
        cout << name.qualifiedName() << '\n';
      }
    } else {
      auto name = conf.getDatasetToShow();
      auto dataset = provider->getDataset(name);
      if (dataset) {
        for (auto& pair : *dataset) {
          cout << pair.first << '\t' << pair.second << '\n';
        }
      } else {
        cout << "Dataset \"" << name << "\" not found\n";
      }
    }
    
    return Elements::ExitCode::OK;
  }
  
};

MAIN_FOR(LsAux)