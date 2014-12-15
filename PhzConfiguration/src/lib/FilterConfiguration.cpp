/** 
 * @file FilterConfiguration.cpp
 * @date October 6, 2014
 * @author Nikolaos Apostolakos
 */

#include <string>
#include <iostream>
#include <vector>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/FilterConfiguration.h"
#include "CheckString.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description FilterConfiguration::getProgramOptions() {
  po::options_description options {"Photometric filter options"};
  options.add_options()
    ("filter-root-path", po::value<std::string>(),
        "The directory containing the Filter datasets, organized in folders")
    ("filter-group", po::value<std::vector<std::string>>(),
        "Use all the Filters in the given group and subgroups")
    ("filter-exclude", po::value<std::vector<std::string>>(),
        "Exclude a single filter name")
    ("filter-name", po::value<std::vector<std::string>>(),
        "Add a single filter name");
  return options;
}

std::unique_ptr<XYDataset::XYDatasetProvider> FilterConfiguration::getFilterDatasetProvider() {
  if (!m_options["filter-root-path"].empty()) {
    std::string path = m_options["filter-root-path"].as<std::string>();
    std::unique_ptr<XYDataset::FileParser> file_parser {new XYDataset::AsciiParser{}};
    return std::unique_ptr<XYDataset::XYDatasetProvider> {
      new XYDataset::FileSystemProvider{path, std::move(file_parser)}
    };
  }
  throw Elements::Exception {"Missing or unknown filter dataset provider options : <filter-root-path>"};
}

std::vector<XYDataset::QualifiedName> FilterConfiguration::getFilterList() {
  // We use a set to avoid duplicate entries
  std::set<XYDataset::QualifiedName, XYDataset::QualifiedName::AlphabeticalComparator> selected {};
  if (!m_options["filter-group"].empty()) {
    auto provider = getFilterDatasetProvider();
    auto group_list = m_options["filter-group"].as<std::vector<std::string>>();
    for (auto& group : group_list) {
      auto names_in_group = provider->listContents(group);
      if (names_in_group.empty()) {
        logger.warn() << "Filter group \"" << group << "\" is empty!";
      }
      for (auto& name : names_in_group) {
        selected.insert(name);
      }
    }
  }
  // Add filter-name if any
  if (!m_options["filter-name"].empty()) {
    auto name_list    = m_options["filter-name"].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.insert(XYDataset::QualifiedName{name});
    }
  }
  // Remove filter-exclude if any
  if (!m_options["filter-exclude"].empty()) {
    auto name_list = m_options["filter-exclude"].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      // Check the name exists before excluding
      auto foundInList = selected.find(name);
      if (foundInList != selected.end()) {
         selected.erase(XYDataset::QualifiedName{name});
      }
      else {
         logger.warn() << "filter-exclude: \"" << name << "\" not found in the list!";
      }
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty filter list";
  }
  return std::vector<XYDataset::QualifiedName> {selected.begin(), selected.end()};
}

}
}
