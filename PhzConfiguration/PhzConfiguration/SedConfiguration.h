/*
 * SedConfiguration.h
 *
 *  Created on: Oct 17, 2014
 *      Author: Nicolas Morisset
 */

#ifndef SEDCONFIGURATION_H_
#define SEDCONFIGURATION_H_


#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "XYDataset/XYDatasetProvider.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class Euclid::PhzConfiguration::SedConfiguration
 * @brief
 * This class defines the sed parameter options
 * @details
 * The parameters available are:
 * sed-root-path : string root path to the seds
 * The three following parameters can be used multiple times
 * sed-group     : string a sed group(only one) e.g. sed/MER
 * sed-name      : string a sed name (only one) e.g. MER/vis
 * sed-exclude   : string a sed name to be excluded (only one)
 * @throw
 * ElementException: Missing or unknown sed dataset provider options
 * ElementException: Empty sed list
 */

class SedConfiguration {

public:

  /**
   * @brief
   * Get program options of the SedConfiguration class
   * @return
   * A boost boost::program_options::options_description type
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief Constructor
   * The sed configuration class which defines parameters allowed for
   * the sed parameter options
   *
   * @details
   * The sed options are:
   * sed-root-path : string, the root path of the seds
   * sed-group     : vector of strings, goup name   e.g. sed/MER
   * sed-name      : vector of strings, sed name e.g. sed/MER/vis
   * sed-exclude   : vector of strings, sed name to be excluded
   * @param options
   * A map containing the options and their value.
   *
   */
  SedConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : m_options{options} {};

  /**
   * @brief
   * This function provides a XYdatasetProvider object
   * @details
   * @throw ElementException
   * Missing  <sed-root-path> or unknown sed dataset provider options
   * @return
   * A unique boost::program_optionsinter of XYDatasetProvider type
   */
  std::unique_ptr<XYDataset::XYDatasetProvider> getSedDatasetProvider();

  /**
   * @brief
   * This function provides a sed list
   * @details
   * @throw ElementException
   * Missing  <sed-root-path> or unknown sed dataset provider options
   * @throw ElementException
   *  Empty sed list
   * @return
   * A vector of QualifiedName type
   */
  std::vector<XYDataset::QualifiedName> getSedList();

private:
  /// Map containing all the sed options and their value
  std::map<std::string, boost::program_options::variable_value> m_options;

};

}
}


#endif /* SEDCONFIGURATION_H_ */
