/** 
 * @file LsData.cpp
 * @date January 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include <set>
#include <boost/program_options/options_description.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("LsData");

class LsData : public Elements::Program {
  
  po::options_description defineSpecificProgramOptions() override {
    po::options_description options {};
    options.add_options()
    ("data-root-path", po::value<std::string>(),
        "The directory containing the data files, organized in folders")
    ("group", po::value<std::string>(),
        "List the contents of the given group")
    ("data", po::value<std::string>(),
        "Print the data of the given dataset");
    return options;
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    
    // Get the dataset provider to use. If the user didn't gave the data-root-path
    // use the current directory
    string path = args["data-root-path"].empty() ? "." : args["data-root-path"].as<std::string>();
    std::unique_ptr<XYDataset::FileParser> file_parser {new XYDataset::AsciiParser{}};
    XYDataset::FileSystemProvider provider {path, std::move(file_parser)};
    
    if (args["data"].empty()) {
      string group = args["group"].empty() ? "" : args["group"].as<std::string>();
      // Order the results
      auto unordered = provider.listContents(group);
      set<XYDataset::QualifiedName, XYDataset::QualifiedName::AlphabeticalComparator>
          ordered_set {unordered.begin(), unordered.end()};
      for (auto& name : ordered_set) {
        cout << name.qualifiedName() << '\n';
      }
    } else {
      auto name = args["data"].as<string>();
      auto dataset = provider.getDataset(name);
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

MAIN_FOR(LsData)