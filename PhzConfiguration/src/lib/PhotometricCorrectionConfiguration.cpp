/** 
 * @file PhotometricCorrectionConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <vector>
#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include "PhzConfiguration/PhotometricCorrectionConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description PhotometricCorrectionConfiguration::getProgramOptions() {
  return PhotometryCatalogConfiguration::getProgramOptions();
}

PhotometricCorrectionConfiguration::PhotometricCorrectionConfiguration(
                      const std::map<std::string, po::variable_value>& options)
        : CatalogConfiguration(options), PhotometryCatalogConfiguration(options) {
  m_options = options;
}

PhzDataModel::PhotometricCorrectionMap PhotometricCorrectionConfiguration::getPhotometricCorrectionMap() {
  std::vector<std::string> filter_names {};
  auto mapping_iter = m_options.find("filter-name-mapping");
  if (mapping_iter != m_options.end()) {
    for (auto& filter_mapping_option : mapping_iter->second.as<std::vector<std::string>>()) {
      smatch match_res;
      regex expr {"\\s*([^\\s]+)\\s+[^\\s]+\\s+[^\\s]+\\s*"};
      if (regex_match(filter_mapping_option, match_res, expr)) {
        filter_names.emplace_back(match_res.str(1));
      }
    }
  }
  PhzDataModel::PhotometricCorrectionMap result {};
  for (auto& filter : filter_names) {
    result[filter] = 1.;
  }
  return result;
}

} // end of namespace PhzConfiguration
} // end of namespace Eucild