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


void FilterConfiguration::addProgramOptions(po::options_description& options) {
  options.add_options()
    ("filter-root-path", po::value<std::string>(),
        "The directory containing the Filter datasets, organized in folders")
    ("filter-group", po::value<std::vector<std::string>>(),
        "Use all the Filters in the given group and subgroups")
    ("filter-list", po::value<std::vector<std::string>>(),
        "Use all the given Filters");
}

std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> FilterConfiguration::filterDatasetProvider() {
  auto filter_path = m_options["filter-root-path"].as<std::string>();
  std::unique_ptr<Euclid::XYDataset::FileParser> file_parser {new Euclid::XYDataset::AsciiParser{}};
  return std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> {
    new Euclid::XYDataset::FileSystemProvider {filter_path, std::move(file_parser)}};
}

}
}