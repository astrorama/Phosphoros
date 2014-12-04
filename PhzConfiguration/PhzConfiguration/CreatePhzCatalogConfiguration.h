/** 
 * @file CreatePhzCatalogConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_CREATEPHZCATALOGCONFIGURATION_H
#define	PHZCONFIGURATION_CREATEPHZCATALOGCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "PhzOutput/OutputHandler.h"
#include "PhzConfiguration/PhotometricCorrectionConfiguration.h"
#include "PhzConfiguration/PhotometryGridConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class CreatePhzCatalogConfiguration : public PhotometricCorrectionConfiguration,
                                      public PhotometryGridConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  CreatePhzCatalogConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
  std::unique_ptr<PhzOutput::OutputHandler> getOutputHandler();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CREATEPHZCATALOGCONFIGURATION_H */

