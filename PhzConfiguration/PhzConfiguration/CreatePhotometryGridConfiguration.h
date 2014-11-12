/** 
 * @file CreatePhotometryGridConfiguration.h
 * @date November 4, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_CREATEPHOTOMETRYGRIDCONFIGURATION_H
#define	PHZCONFIGURATION_CREATEPHOTOMETRYGRIDCONFIGURATION_H

#include <functional>
#include <map>
#include <string>
#include <fstream>
#include <boost/program_options.hpp>
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzConfiguration/ParameterSpaceConfiguration.h"
#include "PhzConfiguration/FilterConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class CreatePhotometryGridConfiguration : public ParameterSpaceConfiguration,
                                          public FilterConfiguration {
  
public:
  
  typedef std::function<void(const PhzDataModel::PhotometryGrid&)> OutputFunction;
  
  static boost::program_options::options_description getProgramOptions();
  
  CreatePhotometryGridConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
  OutputFunction getOutputFunction();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CREATEPHOTOMETRYGRIDCONFIGURATION_H */

