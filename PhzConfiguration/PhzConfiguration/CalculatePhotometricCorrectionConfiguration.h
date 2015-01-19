/** 
 * @file CalculatePhotometricCorrectionConfiguration.h
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_CALCULATEPHOTOMETRICCORRECTIONCONFIGURATION_H
#define	PHZCONFIGURATION_CALCULATEPHOTOMETRICCORRECTIONCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzConfiguration/PhotometryCatalogConfiguration.h"
#include "PhzConfiguration/SpectroscopicRedshiftCatalogConfiguration.h"
#include "PhzConfiguration/PhotometryGridConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class CalculatePhotometricCorrectionConfiguration : virtual public PhotometryCatalogConfiguration,
                                                    virtual public SpectroscopicRedshiftCatalogConfiguration,
                                                    public PhotometryGridConfiguration {
  
public:
  
  /// The type of the function which handles the output of the photometric
  /// correction map
  typedef std::function<void(const PhzDataModel::PhotometricCorrectionMap&)> OutputFunction;
  
  static boost::program_options::options_description getProgramOptions();
  
  CalculatePhotometricCorrectionConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
  OutputFunction getOutputFunction();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CALCULATEPHOTOMETRICCORRECTIONCONFIGURATION_H */

