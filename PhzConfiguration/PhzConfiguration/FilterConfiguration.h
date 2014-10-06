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
  
  static void addProgramOptions(po::options_description& options);
  
  std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> filterDatasetProvider();
  
private:
  
  std::map<std::string, po::variable_value> m_options;
  
};

}
}

#endif	/* PHZCONFIGURATION_FILTERCONFIGURATION_H */

