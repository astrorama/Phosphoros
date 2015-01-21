/** 
 * @file CalculatePhotometricCorrectionConfiguration.cpp
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PhzPhotometricCorrection/DefaultStopCriteria.h"
#include "PhzConfiguration/CalculatePhotometricCorrectionConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhzConfiguration");

po::options_description CalculatePhotometricCorrectionConfiguration::getProgramOptions() {

  po::options_description options {"Calculate Photometric Correction options"};

  options.add_options()
  ("output-phot-corr-file", boost::program_options::value<std::string>(),
      "The filename of the file to export the calculated photometric correction")
  ("phot-corr-iter-no", boost::program_options::value<int>()->default_value(5),
      "The maximum number of iterations to perform (negative=unlimited)")
  ("phot-corr-tolerance", boost::program_options::value<double>()->default_value(1E-3),
      "The tolerance which if achieved between two iteration steps the iteration stops");

  // We get all the catalog options. We are careful not to add an option twice.
  po::options_description catalog_options = PhotometryCatalogConfiguration::getProgramOptions();
  auto specz_options = SpectroscopicRedshiftCatalogConfiguration::getProgramOptions();
  for (auto specz_option : specz_options.options()) {
    if (catalog_options.find_nothrow(specz_option->long_name(), false) == nullptr) {
      catalog_options.add(specz_option);
    }
  }
  options.add(catalog_options);
  
  options.add(PhotometryGridConfiguration::getProgramOptions());

  return options;
}

CalculatePhotometricCorrectionConfiguration::CalculatePhotometricCorrectionConfiguration(
            const std::map<std::string, boost::program_options::variable_value>& options)
      : CatalogConfiguration(options), PhotometryCatalogConfiguration(options),
        SpectroscopicRedshiftCatalogConfiguration(options), PhotometryGridConfiguration(options) {
  
  m_options = options;

  //Extract file option
  if (m_options["output-phot-corr-file"].empty()) {
    throw Elements::Exception() << "Missing parameter output-phot-corr-file";
  }
  std::string filename = m_options["output-phot-corr-file"].as<std::string>();

  // The purpose here is to make sure we are able to
  // write the binary file on the disk
  std::fstream test_fstream;
  test_fstream.open(filename, std::fstream::out | std::fstream::binary);
  if ((test_fstream.rdstate() & std::fstream::failbit) != 0) {
    throw Elements::Exception() <<" IO error, can not write any file there : %s "
                                << filename << " (from option : output-phot-corr-file)";
  }
  test_fstream.close();
  // Remove file created
  if (std::remove(filename.c_str())) {
    logger.warn() << "Removing temporary file creation failed: \"" << filename << "\" !";
  }
  
}

auto CalculatePhotometricCorrectionConfiguration::getOutputFunction() -> OutputFunction {
  return [this](const PhzDataModel::PhotometricCorrectionMap& pc_map) {
    std::ofstream out {m_options["output-phot-corr-file"].as<std::string>()};
    PhzDataModel::writePhotometricCorrectionMap(out, pc_map);
  };
}

PhzPhotometricCorrection::PhotometricCorrectionCalculator::StopCriteriaFunction
                      CalculatePhotometricCorrectionConfiguration::getStopCriteria() {
  int iter_no = m_options["phot-corr-iter-no"].as<int>();
  double tolerance = m_options["phot-corr-tolerance"].as<double>();
  return PhzPhotometricCorrection::DefaultStopCriteria(iter_no, tolerance);
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid