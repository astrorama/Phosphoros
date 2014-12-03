/** 
 * @file PhotometryCatalogConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_PHOTOMETRYCATALOGCONFIGURATION_H
#define	PHZCONFIGURATION_PHOTOMETRYCATALOGCONFIGURATION_H

#include "PhzConfiguration/CatalogConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class PhotometryCatalogConfiguration : virtual public CatalogConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  PhotometryCatalogConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHOTOMETRYCATALOGCONFIGURATION_H */

