/** 
 * @file FilterConfiguration.cpp
 * @date October 6, 2014
 * @author Nikolaos Apostolakos
 */

#include <string>
#include <vector>
#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/FilterConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

FilterConfiguration::FilterConfiguration(const std::map<std::string, po::variable_value>& options) : m_options{options} {
  if (m_options["filter-root-path"].empty()) {
    throw Elements::Exception("Missing filter-root-path parameter");
  }
}


po::options_description FilterConfiguration::getProgramOptions() {
  po::options_description options {"Photometric filter options"};
  options.add_options()
    ("filter-root-path", po::value<std::string>(),
        "The directory containing the Filter datasets, organized in folders")
    ("filter-group", po::value<std::vector<std::string>>(),
        "Use all the Filters in the given group and subgroups")
    ("filter-list", po::value<std::vector<std::string>>(),
        "Use all the given Filters");
  return options;
}

std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> FilterConfiguration::filterDatasetProvider() {
  if (!m_options["filter-root-path"].empty()) {
    std::string path = m_options["filter-root-path"].as<std::string>();
    std::unique_ptr<Euclid::XYDataset::FileParser> file_parser {new Euclid::XYDataset::AsciiParser{}};
    return std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> {
      new Euclid::XYDataset::FileSystemProvider{path, std::move(file_parser)}
    };
  }
  throw Elements::Exception {"Missing or unknown Filter dataset provider options"};
}

std::vector<Euclid::XYDataset::QualifiedName> FilterConfiguration::filterList() {
  // We use a set to avoid duplicate entries
  std::set<Euclid::XYDataset::QualifiedName> selected {};
  if (!m_options["filter-group"].empty()) {
    auto provider = filterDatasetProvider();
    auto group_list = m_options["filter-group"].as<std::vector<std::string>>();
    for (auto& group : group_list) {
      for (auto& name : provider->listContents(group)) {
        selected.insert(name);
      }
    }
  }
  if (!m_options["filter-list"].empty()) {
    auto name_list = m_options["filter-list"].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.insert(Euclid::XYDataset::QualifiedName{name});
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty Filter list";
  }
  return std::vector<Euclid::XYDataset::QualifiedName> {selected.begin(), selected.end()};
}

}
}