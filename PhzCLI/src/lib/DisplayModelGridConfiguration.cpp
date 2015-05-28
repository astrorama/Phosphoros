/**
 * @file DisplayModelGridConfiguration.cpp
 * @date January 26, 2015
 * @author Nikolaos Apostolakos
 */

#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include "ElementsKernel/Logging.h"
#include "PhzCLI/DisplayModelGridConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description DisplayModelGridConfiguration::getProgramOptions() {

  po::options_description options {"Display Model Grid options"};

  auto phot_options = PhotometryGridConfiguration::getProgramOptions();
  for (auto o : phot_options.options()) {
    options.add(o);
  }

  options.add_options()
  ("sed", po::bool_switch()->default_value(false),
      "Show the SED axis values")
  ("redcurve", po::bool_switch()->default_value(false),
      "Show the reddening curve axis values")
  ("ebv", po::bool_switch()->default_value(false),
      "Show the E(B-V) axis values")
  ("z", po::bool_switch()->default_value(false),
      "Show the redshift axis values")
  ("phot", po::value<std::string>(),
      "Show the photometry of the cell (SED,REDCURVE,EBV,Z) (zero based indices)");

  return options;
}

DisplayModelGridConfiguration::DisplayModelGridConfiguration(
            const std::map<std::string, po::variable_value>& options)
      : PhosphorosPathConfiguration(options), CatalogNameConfiguration(options),
        PhotometryGridConfiguration(options) {

  m_options = options;

}

bool DisplayModelGridConfiguration::showGeneric() {
  bool result = true;
  if (m_options["sed"].as<bool>() || m_options["redcurve"].as<bool>()
      || m_options["ebv"].as<bool>() || m_options["z"].as<bool>()
      || !m_options["phot"].empty()) {
    return false;
  }
  return result;
}

bool DisplayModelGridConfiguration::showSedAxis() {
  return m_options["sed"].as<bool>();
}

bool DisplayModelGridConfiguration::showReddeningCurveAxis() {
  return m_options["redcurve"].as<bool>();
}

bool DisplayModelGridConfiguration::showEbvAxis() {
  return m_options["ebv"].as<bool>();
}

bool DisplayModelGridConfiguration::showRedshiftAxis() {
  return m_options["z"].as<bool>();
}

std::unique_ptr<std::tuple<size_t,size_t,size_t,size_t>> DisplayModelGridConfiguration::getCellPhotCoords() {
  std::unique_ptr<std::tuple<size_t,size_t,size_t,size_t>> result {};
  if (!m_options["phot"].empty()) {
    std::string coords = m_options["phot"].as<std::string>();
    smatch match_res;
    regex expr {"\\s*\\(?\\s*" // Skip any opening parenthesis if exists and any spaces
                "(\\d+)" // Get the SED index
                "\\s*[ ,]" // Separate with comma or space. Ignore other spaces
                "(\\d+)" // Get the REDCURVE index
                "\\s*[ ,]" // Separate with comma or space. Ignore other spaces
                "(\\d+)" // Get the EBV index
                "\\s*[ ,]" // Separate with comma or space. Ignore other spaces
                "(\\d+)" // Get the Z index
                "\\s*\\)?\\s*"}; // Skip any closing parentesis if exists and any spaces
    if (!regex_match(coords, match_res, expr)) {
      logger.error() << "Malformed coordinates: " << coords;
      throw Elements::Exception() << "Malformed coordinates: " << coords;
    }
    std::stringstream coords_stream {};
    coords_stream << match_res.str(1) << " " << match_res.str(2) << " "
                  << match_res.str(3) << " " << match_res.str(4) << " ";
    size_t c1, c2, c3, c4;
    coords_stream >> c1 >> c2 >> c3 >> c4;
    result.reset(new std::tuple<size_t,size_t,size_t,size_t>(c1, c2, c3, c4));
  }
  return result;
}

}
}