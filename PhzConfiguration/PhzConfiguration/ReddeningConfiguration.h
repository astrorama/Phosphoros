/** 
 * @file PhzConfiguration/ReddeningConfiguration.h
 * @date October 6, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_REDDENINGCONFIGURATION_H
#define	PHZCONFIGURATION_REDDENINGCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "XYDataset/XYDatasetProvider.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class Euclid::PhzConfiguration::ReddeningConfiguration
 * @brief
 * This class defines the Reddening parameter options
 * @details
 * The parameters available are:
 * reddening-root-path : string, a root path to the reddening curves
 * The three following parameters can be used multiple times
 * reddening-group   : string, a reddening group(only one) e.g. reddening/CAL
 * reddening-name    : string, a reddening name (only one) e.g. CAL/calzetti
 * reddening-exclude : string, a reddening name to be excluded (only one)
 * ebv-range : double type, min max step e.g. "0. 2. 0.5"
 *            - min as minimum E(B-V) value
 *            - max as maximum E(B-V) value and max > min
 *            - step, step to be used for the interval
 * Note that the space character is the separator.
 * You can add several z-range parameters but they should not overlap. The
 * the following example is forbidden:
 *  ebv-range = 0. 2. 0.5
 *  ebv-range = 1.5 3. 1.
 *  1.5 is less than 2 of the first range and this is not allowed!
 * A regex is applied for ebv-range: (((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)){3}
 *
 * ebv-value : double, E(B-V) value e.g. 4.
 * You can add as many ebv-value parameters as you want but each ebv-value
 * parameter must contain only one value.
 * A regex is applied for ebv-value : ((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)
 * @throw
 * Elements::Exception: Missing or unknown reddening dataset provider options:
 *                      <reddening-curve-root-path>
 * Elements::Exception: Invalid character(s) for the ebv-value option
 * Elements::Exception: Invalid range(s) for ebv-range option
 * Elements::Exception: Empty ebv list (check the options ebv-range and ebv-value)
 */

class ReddeningConfiguration {
  
public:
  
  /**
   * @brief
   * Get program options of the ReddeningConfiguration class
   * @return
   * A boost boost::program_options::options_description type
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief Constructor
   * The reddening configuration class which defines parameters allowed for
   * the reddening parameter options
   *
   * @details
   * The reddening options are provided through a STL map which contains the
   * following informations:
   * reddening-root-path : string, the root path of the reddening
   * reddening-group     : vector of strings, goup name
   * reddening-name      : vector of strings, reddening name
   * reddening-exclude   : vector of strings, reddening name to be excluded
   * ebv-range           : vector of strings, range as: min max step
   * ebv-value           : vector of strings, only one E(B-V) value per string
   * @param options
   * A map containing the options and their values.
   *
   */
  ReddeningConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : m_options{options} {};

  /**
   * @brief
   * This function provides a XYdatasetProvider object
   * @details
   * @throw ElementException
   * Missing  <reddening-root-path> or unknown reddening dataset provider options
   * @return
   * A unique pointer of XYDatasetProvider type
   */
  std::unique_ptr<XYDataset::XYDatasetProvider> getReddeningDatasetProvider();
  
  /**
   * @brief
   * This function provides a reddening curve list
   * @details
   * @throw ElementException
   * Missing  <reddening-root-path> or unknown reddening dataset provider options
   * @throw ElementException
   *  Empty reddening list
   * @return
   * A vector of QualifiedName types
   */
  std::vector<XYDataset::QualifiedName> getReddeningCurveList();

  /**
   * @brief
   * This function provides an E(B-V) list as a vector of doubles
   * @details
   * A regex is applied for the ebv-range and another for the ebv-value
   * in order to make sure the string given got valid characters.
   * regex for the ebv-range : (((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)){3}
   * regex for the ebv-value : ((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)
   *
   * @throw ElementException
   * Missing  <filter-root-path> or unknown filter dataset provider options
   * @throw ElementException
   * ElementException: Invalid range(s) for the ebv-range option!
   * ElementException: Invalid character(s) for the ebv-range option!
   * ElementException: Invalid character(s) for the ebv-value option!
   *  Empty filter list
   * @return
   * A vector of doubles
   */
   std::vector<double> getEbvList();

private:
  /// Map containing all the reddening options and their value
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

}
}

#endif	/* PHZCONFIGURATION_FILTERCONFIGURATION_H */

