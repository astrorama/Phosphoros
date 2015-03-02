#ifndef PHZGRIDINFOHANDLER_H
#define PHZGRIDINFOHANDLER_H

#include <list>
#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "PhzDataModel/PhzModel.h"
#include "PhzQtUI/ModelSet.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {


/**
 * @brief The PhzGridInfoHandler class
 */
class PhzGridInfoHandler
{
public:

  /**
   *  @brief Build the Axes tuple from the ModelSet.
   *
   *  @param model_set The selected ModelSet.
   *
   *  @return the ModelAxesTuple matching the ModelSet.
   */
  static PhzDataModel::ModelAxesTuple getAxesTuple(const ModelSet& model_set);

  /**
   * @brief Get the name of the file containing a grid with the same axes and
   * filters as provided.
   *
   * @param axes The ModelAxesTuple containing the axes of the grid.
   *
   * @param selected_filters A list of the filters names.
   */
  static std::list<std::string> getCompatibleGridFile(
      const PhzDataModel::ModelAxesTuple& axes,
      const std::list<std::string> & selected_filters);

  /**
   * @breif Build a boost configuration map out of the selected parameters.
   *
   * @param output_file The name of the file to which the Grid has to be written.
   *
   * @param axes The ModelAxesTuple containing the axes of the grid.
   *
   * @param selected_filters A list of the filters names.
   */
  static std::map<std::string, po::variable_value> GetConfigurationMap(
      std::string output_file,
      const PhzDataModel::ModelAxesTuple& axes,
      const std::list<std::string>& selected_filters);
};

}
}

#endif // PHZGRIDINFOHANDLER_H

