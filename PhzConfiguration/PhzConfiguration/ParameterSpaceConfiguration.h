/*
 * ParameterSpaceConfiguration.h
 *
 *  Created on: Oct 29, 2014
 *      Author: Nicolas Morisset
 */

#ifndef PARAMETERSPACECONFIGURATION_H_
#define PARAMETERSPACECONFIGURATION_H_

#include <map>
#include <string>

#include <boost/program_options.hpp>

#include "XYDataset/XYDatasetProvider.h"
#include "PhzConfiguration/SedConfiguration.h"
#include "PhzConfiguration/RedshiftConfiguration.h"
#include "PhzConfiguration/ReddeningConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class ParameterSpaceConfiguration
 * @brief
 * This class combines the parameter options of the SedConfiguration,
 * RedshiftConfiguration and ReddeningConfiguration classes
 * @details
 * The parameters available are the ones defined by the SedConfiguration,
 * RedshiftConfiguration and ReddeningConfiguration classes, so have a look at
 *  these classes for more details. The options available are:
 * - sed-root-path
 * - sed-group
 * - sed-exclude
 * - sed-name
 * - z-range
 * - z-value
 * - reddening-curve-root-path
 * - reddening-curve-group
 * - reddening-curve-exclude
 * - reddening-curve-name
 * - ebv-range
 * - ebv-value
 */
class ParameterSpaceConfiguration : public SedConfiguration,
                                    public RedshiftConfiguration,
                                    public ReddeningConfiguration {

public:

  /**
   * @brief
   * Get program options of the ParameterSpaceConfiguration class
   * @return
   * A boost::program_options::options_description boost type
   */
  static boost::program_options::options_description getProgramOptions()
  {
    boost::program_options::options_description options {"Photometric combined options"};

    options.add(SedConfiguration::getProgramOptions());
    options.add(RedshiftConfiguration::getProgramOptions());
    options.add(ReddeningConfiguration::getProgramOptions());

    return options;
  }

  /**
   * @brief Constructor
   * @param options
   * A map containing the options and their values.
   *
   */
  ParameterSpaceConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
            : SedConfiguration(options), RedshiftConfiguration(options), ReddeningConfiguration(options) {};
  
   /**
   * @brief destructor.
   */
   virtual ~ParameterSpaceConfiguration()=default;


};

}
}



#endif /* PARAMETERSPACECONFIGURATION_H_ */
