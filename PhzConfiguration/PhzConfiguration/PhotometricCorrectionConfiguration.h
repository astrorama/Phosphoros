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

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhotometricCorrectionConfiguration
 * 
 * @brief
 * Class to be used by executables which want to have as input the photometric
 * correction
 * 
 * @details
 * At the moment this class is very minimal, because the photometric correction
 * implementation is not yet defined. It provides a photometric
 * correction map with all the corrections set to 1, for all the filters defined
 * as photometry filters of the input catalog with the filter-name-mapping.
 * 
 * When the Photometric Correction is defined, this class will be modified
 * accordingly.
 */
class PhotometricCorrectionConfiguration {
  
public:
  
  /// Returns no options
  static boost::program_options::options_description getProgramOptions();
  
  /// Creates a new instance of PhotometricCorrectionConfiguration
  PhotometricCorrectionConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
  /// Returns a map with the photometric corrections for the filters
  PhzDataModel::PhotometricCorrectionMap getPhotometricCorrectionMap();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHOTOMETRICCORRECTIONCONFIGURATION_H */

