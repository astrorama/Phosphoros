/*
 * PhotometryGridConfguration.h
 *
 *  Created on: Dec 3, 2014
 *      Author: Nicolas Morisset
 */

#ifndef PHOTOMETRYGRIDCONFGURATION_H_
#define PHOTOMETRYGRIDCONFGURATION_H_

#include <map>
#include <string>

#include <boost/program_options.hpp>

#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/PhotometryGrid.h"

namespace Euclid {
namespace PhzConfiguration {

class PhotometryGridConfiguration {

public:

   static boost::program_options::options_description getProgramOptions();

  /**
   * @brief Constructor
   * Construtor which defines parameters allowed for this class
   *
   * @details
   * The options are:
   * photometry-grid-file : string, filename and path of the photometry grid file
   * @param options
   * A map containing the options and their values.
   *
   */
   PhotometryGridConfiguration(const std::map<std::string, boost::program_options::variable_value>& options)
                     : m_options{options} {};


  /**
   * @brief
   * @details
   * @throw ElementException
   * @throw ElementException
   * @return
   * A vector of QualifiedName type
   */
   PhzDataModel::PhotometryGrid getPhotometryGrid();

private:
  /// Map containing all the filter options and their values
  std::map<std::string, boost::program_options::variable_value> m_options;

};

}
}




#endif /* PHOTOMETRYGRIDCONFGURATION_H_ */
