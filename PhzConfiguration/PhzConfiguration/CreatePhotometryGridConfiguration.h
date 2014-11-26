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

#include <boost/program_options.hpp>

#include "PhzDataModel/PhotometryGrid.h"
#include "PhzConfiguration/ParameterSpaceConfiguration.h"
#include "PhzConfiguration/FilterConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class Euclid::PhzConfiguration::CreatePhotometryGridConfiguration
 * @brief
 * This class defines the Photometry Grid parameter option, \b binary-photometry-grid
 * and inherits from the options of the ParameterSpaceConfiguration and FilterConfiguration
 * options. It also provides a function for writing a grid in a binary file.
 * @details
 * The parameter available is :
 * - \b binary-photometry-grid : string, output filename for storing the grid data
 * Before writing data to the disk, the constructor checks that it is
 * possible to write on the disk at the location specified by the
 * binary-photometry-grid option and throws an exception if any.
 * @throw Element::Exception
 * - IO error, can not write any file there
 */

class CreatePhotometryGridConfiguration : public ParameterSpaceConfiguration,
                                          public FilterConfiguration {
  
public:
  
  typedef std::function<void(const PhzDataModel::PhotometryGrid&)> OutputFunction;
  
  /**
   * @brief
   * Get program options of the CreatePhotometryGridConfiguration class
   * @return
   * A boost boost::program_options::options_description type
   */
  static boost::program_options::options_description getProgramOptions();
  
  /**
   * @brief Constructor
   * The CreatePhotometryGridConfiguration class which defines parameters allowed for
   * the CreatePhotometryGridConfiguration parameter options
   *
   * @details
   * The option is:
   *  - \b binary-photometry-grid : string, output filename for storing the grid data
   * @param options
   * A map containing the options and their values.
   *
   */
  CreatePhotometryGridConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
  /**
   * @brief
   * This function provides a function with PhzDataModel::PhotometryGrid
   * object as argument and this function stores this object in a binary file
   *  with the filename provided by the binary-photometry-grid option.
   * @details
   * Before writing the object to the disk, the constructor checks that it is
   * possible to write data at the location specified by the \b
   * binary-photometry-grid option and throws an exception if any.
   * @throw Element::Exception
   * - IO error, can not write any file there
   * @return
   * A void function.
   */
  OutputFunction getOutputFunction();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CREATEPHOTOMETRYGRIDCONFIGURATION_H */

