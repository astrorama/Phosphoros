/** 
 * @file CreatePhzCatalogConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include "PhzOutput/BestModelCatalog.h"
#include "PhzConfiguration/CreatePhzCatalogConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description CreatePhzCatalogConfiguration::getProgramOptions() {
  po::options_description options {"Create PHZ Catalog options"};

  options.add_options()
  ("output-catalog-file", po::value<std::string>(),
      "The filename of the file to export the PHZ catalog file");

  options.add(PhotometricCorrectionConfiguration::getProgramOptions());
  options.add(PhotometryCatalogConfiguration::getProgramOptions());
  options.add(PhotometryGridConfiguration::getProgramOptions());

  return options;
}

CreatePhzCatalogConfiguration::CreatePhzCatalogConfiguration(const std::map<std::string, po::variable_value>& options)
          : CatalogConfiguration(options), PhotometricCorrectionConfiguration(options),
            PhotometryCatalogConfiguration(options), PhotometryGridConfiguration(options) {
  m_options = options;
}

std::unique_ptr<PhzOutput::OutputHandler> CreatePhzCatalogConfiguration::getOutputHandler() {
  std::string out_file = m_options["output-catalog-file"].as<std::string>();
  return std::unique_ptr<PhzOutput::OutputHandler> {new PhzOutput::BestModelCatalog{out_file}};
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid