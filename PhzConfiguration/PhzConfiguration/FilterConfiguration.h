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

/**
 * @class Euclid::PhzConfiguration::FilterConfiguration
 * @brief
 * This class defines the filter parameter options
 * @details
 * The parameters available are:
 * filter-root-path : string, root path to the filters
 * The three following parameters can be used multiple times
 * filter-group     : string, a filter group(only one) e.g. filter/MER
 * filter-name      : string: a filter name (only one) e.g. MER/vis
 * filter-exclude   : string: a filter name to be excluded (only one)
 * @throw ElementException
 * Missing or unknown filter dataset provider options : <filter-root-path>
 */

class FilterConfiguration {
  
public:
  
  static po::options_description getProgramOptions();

  FilterConfiguration(const std::map<std::string, po::variable_value>& options)
                     : m_options{options} {};
  
  std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> getFilterDatasetProvider();
  
  std::vector<Euclid::XYDataset::QualifiedName> getFilterList();
  
private:
  
  std::map<std::string, po::variable_value> m_options;
  
};

}
}

#endif	/* PHZCONFIGURATION_FILTERCONFIGURATION_H */

