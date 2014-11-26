/*
 * RedshiftConfiguration.h
 *
 *  Created on: Oct 20, 2014
 *      Author: Nicolas Morisset
 */

#ifndef READSHIFTCONFIGURATION_H_
#define READSHIFTCONFIGURATION_H_


#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "XYDataset/XYDatasetProvider.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class Euclid::PhzConfiguration::RedshiftConfiguration
 * @brief
 * This class defines the redshift parameter options
 * @details
 * The parameters available are:
 * - \b z-range : double type, min max step e.g. "0. 2. 0.5"
 *          - min as minimum redshift value
 *          - max as maximum redshift value and max > min
 *          - step, step to be used for the interval
 *  Note that the space character is the separator.
 *  You can add several z-range parameters but they should not overlap. The
 *  following example is not allowed:
 *  - z-range = 0. 2. 0.5
 *  - z-range = 1.5 3. 1.
 *  as 1.5 is less than 2 of the first range. \n
 * A regex is applied for z-range: `(((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)){3}`
 *
 * - \b z-value : double, redshift value e.g. 4. \n
 * You can add as many z-value parameters as you want but each z-value
 * parameter must contain only one value.\n
 * A regex is applied for z-value : `((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)`
 * @throw Element::Exception
 * - Invalid range(s) for the z-range option!
 * - Invalid character(s) for the z-range option
 * - Invalid character(s) for the z-value option
 */

class RedshiftConfiguration {

public:

  /**
   * @brief
   * Get program options of the RedshiftConfiguration class
   * @return
   * A boost boost::program_options::options_description object
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief Constructor
   * The redshift configuration class which defines parameters allowed for
   * the redshift parameter options(z-range and z-value)
   *
   * @details
   * The redshift options are:
   * - \b z-range     : string, minimum z, maximum z, z step e.g. 0 2 0.1
   * - \b z-value     : string, a single value e.g. 1.5
   * @param options
   * A map containing the options and their values.
   *
   */
  RedshiftConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : m_options{options} {};


  /**
   * @brief
   * This function provides a redshift list
   * @details
   * @throw Element::Exception
   * - Missing  <filter-root-path> or unknown filter dataset provider options
   * - Invalid range(s) for the z-range option!
   * - Invalid character(s) for the z-range option
   * - Invalid character(s) for the z-value option
   * - Empty filter list
   * @return
   * A vector of QualifiedName type
   */
   std::vector<double> getZList();

private:
  /// Map containing all the filter options and their values
  std::map<std::string, boost::program_options::variable_value> m_options;

};

}
}


#endif /* READSHIFTCONFIGURATION_H_ */
