/** 
 * @file LsAux.cpp
 * @date January 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include <set>
#include <boost/program_options/options_description.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzCLI/LsAuxConfiguration.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosLsAux");

class LsAux : public Elements::Program {
  
  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::LsAuxConfiguration::getProgramOptions();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    
    PhzConfiguration::LsAuxConfiguration conf {args};
    
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