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
  
  static boost::program_options::options_description getProgramOptions() {
    boost::program_options::options_description options {"Create Photometry Grid options"};
    
    options.add_options()
    ("binary-photometry-grid", boost::program_options::value<std::string>(),
        "The file to export in binary format the photometry grid");
    
    options.add(ParameterSpaceConfiguration::getProgramOptions());
    options.add(FilterConfiguration::getProgramOptions());
    
    return options;
  }
  
  CreatePhotometryGridConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
          : ParameterSpaceConfiguration(options), FilterConfiguration(options) {
    m_options = options;
  }
  
  OutputFunction getOutputFunction() {
    return [this](const PhzDataModel::PhotometryGrid& grid) {
      std::ofstream out {m_options["binary-photometry-grid"].as<std::string>()};
      GridContainer::gridBinaryExport(out, grid);
    };
  }
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CREATEPHOTOMETRYGRIDCONFIGURATION_H */

