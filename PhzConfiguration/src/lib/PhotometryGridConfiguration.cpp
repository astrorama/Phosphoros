/*
 * PhotometryGridConfiguration.cpp
 *
 *  Created on: Dec 3, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <vector>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "PhzConfiguration/PhotometryGridConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description PhotometryGridConfiguration::getProgramOptions() {
  po::options_description options {"Photometric Grid options"};
  options.add_options()
    ("photometry-grid-file", po::value<std::string>(),
        "The path and filename of the grid file");
  return options;
}

PhzDataModel::PhotometryGrid PhotometryGridConfiguration::getPhotometryGrid() {
  std::string option_name{"photometry-grid-file"};
  if (m_options[option_name].empty()) {
    throw Elements::Exception() << "Empty parameter option : \"" << option_name << "\"";
  }
  std::ifstream in{m_options[option_name].as<std::string>()};
  // Read binary file
  return (PhzDataModel::phzGridBinaryImport<PhzDataModel::PhotometryCellManager>(in));
}

}
}
