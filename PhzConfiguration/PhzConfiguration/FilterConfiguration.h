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
#include "XYDataset/XYDatasetProvider.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class FilterConfiguration
 * @brief
 * This class defines the filter parameter options
 * @details
 * The parameters available are:
 * filter-root-path : string root path to the filters
 * The three following parameters can be used multiple times
 * filter-group     : string a filter group(only one) e.g. filter/MER
 * filter-name      : string a filter name (only one) e.g. MER/vis
 * filter-exclude   : string a filter name to be excluded (only one)
 * @throw
 * ElementException: Missing or unknown filter dataset provider options
 * ElementException: Empty filter list
 */
class FilterConfiguration {
  
public:
  
  /**
   * @brief
   * Get program options of the FilterConfiguration class
   * @return
   * A boost boost::program_options::options_description type
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief Constructor
   * The filter configuration class which defines parameters allowed for
   * the filter parameter options
   *
   * @details
   * The filter options are:
   * filter-root-path : string, the root path of the filters
   * filter-group     : vector of strings, goup name   e.g. filter/MER
   * filter-name      : vector of strings, filter name e.g. filter/MER/vis
   * filter-exclude   : vector of strings, filter name to be excluded
   * @param options
   * A map containing the options and their values.
   *
   */
  FilterConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : m_options{options} {};

  /**
   * @brief
   * This function provides a XYdatasetProvider object
   * @details
   * @throw ElementException
   * Missing  <filter-root-path> or unknown filter dataset provider options
   * @return
   * A unique pointer of XYDatasetProvider type
   */
  std::unique_ptr<XYDataset::XYDatasetProvider> getFilterDatasetProvider();
  
  /**
   * @brief
   * This function provides a filter list
   * @details
   * @throw ElementException
   * Missing  <filter-root-path> or unknown filter dataset provider options
   * @throw ElementException
   *  Empty filter list
   * @return
   * A vector of QualifiedName type
   */
  std::vector<XYDataset::QualifiedName> getFilterList();
  
private:
  /// Map containing all the filter options and their values
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

}
}

#endif	/* PHZCONFIGURATION_FILTERCONFIGURATION_H */

