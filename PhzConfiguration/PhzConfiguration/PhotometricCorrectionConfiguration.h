/** 
 * @file PhotometricCorrectionConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_PHOTOMETRICCORRECTIONCONFIGURATION_H
#define	PHZCONFIGURATION_PHOTOMETRICCORRECTIONCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzConfiguration/PhotometryCatalogConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class PhotometricCorrectionConfiguration : public PhotometryCatalogConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  PhotometricCorrectionConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
  PhzDataModel::PhotometricCorrectionMap getPhotometricCorrectionMap();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHOTOMETRICCORRECTIONCONFIGURATION_H */

