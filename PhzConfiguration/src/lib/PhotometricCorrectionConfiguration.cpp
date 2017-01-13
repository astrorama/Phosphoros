/** 
 * @file PhotometricCorrectionConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzConfiguration/PhotometricCorrectionConfiguration.h"

using boost::regex;
using boost::regex_match;
using boost::smatch;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description PhotometricCorrectionConfiguration::getProgramOptions() {
  po::options_description options {"Photometric Correction options"};
  options.add_options()
    ("photometric-correction-file", po::value<std::string>(), "The full path of the photometric correction file");
  return options;
}

PhzDataModel::PhotometricCorrectionMap PhotometricCorrectionConfiguration::getPhotometricCorrectionMap() {

 PhzDataModel::PhotometricCorrectionMap result {};
 std::string file_option{"photometric-correction-file"};

 // Read correction map from an ASCII file otherwise set default values
 if (!m_options[file_option].empty()) {
	 // Check the file exist
	 auto correction_file = m_options[file_option].as<std::string>();
	 if (!fs::exists(correction_file)) {
	   logger.error() << "File " << correction_file << " not found";
	   throw Elements::Exception() << "Photometric Correction file " << correction_file << " does not exist";
	 }
	 // Read the correction file(ASCII type)
	 std::ifstream in {correction_file};
	 result = PhzDataModel::readPhotometricCorrectionMap(in);
 }
 else {
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
  for (auto& filter : filter_names) {
    result[filter] = 1.;
  }

 } //EndOfoption
  return result;
}

} // end of namespace PhzConfiguration
} // end of namespace Eucild
