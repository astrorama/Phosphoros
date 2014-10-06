/** 
 * @file PhzConfiguration/FilterConfiguration.h
 * @date October 6, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_FILTERCONFIGURATION_H
#define	PHZCONFIGURATION_FILTERCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include "XYDataset/XYDatasetProvider.h"

namespace Euclid {
namespace PhzConfiguration {

class FilterConfiguration {
  
public:
  
  FilterConfiguration(const std::map<std::string, po::variable_value>& options);
  
  static po::options_description getProgramOptions();
  
  std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> filterDatasetProvider();
  
  std::vector<Euclid::XYDataset::QualifiedName> filterList();
  
private:
  
  std::map<std::string, po::variable_value> m_options;
  
};

}
}

#endif	/* PHZCONFIGURATION_FILTERCONFIGURATION_H */

