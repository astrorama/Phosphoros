#ifndef PHZGRIDINFOHANDLER_H
#define PHZGRIDINFOHANDLER_H

#include <list>
#include <map>
#include <string>
#include "PhzDataModel/PhzModel.h"
#include "PhzQtUI/ModelSet.h"

namespace boost{
namespace program_options{
 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {


/**
 * @brief The PhzGridInfoHandler class
 */
class PhzGridInfoHandler
{
public:

  static bool checkGridFileCompatibility(QString file_path,
      const std::map<std::string,PhzDataModel::ModelAxesTuple>& axes,
      const std::list<std::string> & selected_filters,
      const std::string igm_type,
      const std::string luminosity_filter);

  /**
   * @brief Get the name of the file containing a grid with the same axes and
   * filters as provided.
   *
   * @param axes The ModelAxesTuple containing the axes of the grid.
   *
   * @param selected_filters A list of the filters names.
   *
   * @param igm_type The type of inter-galactic medium absorption.
   */
  static std::list<std::string> getCompatibleGridFile(
      std::string catalog,
      const std::map<std::string,PhzDataModel::ModelAxesTuple>& axes,
      const std::list<std::string> & selected_filters,
      std::string igm_type,
      const std::string luminosity_filter,
      int grid_type = 1);

  /**
   * @breif Build a boost configuration map out of the selected parameters.
   *
   * @param output_file The name of the file to which the Grid has to be written.
   *
   * @param axes The ModelAxesTuple containing the axes of the grid.
   *
   * @param selected_filters A list of the filters names.
   *
   * @param igm_type The type of inter-galactic medium absorption.
   */
  static std::map<std::string, boost::program_options::variable_value> GetConfigurationMap(
      std::string catalog,
      std::string output_file,
      ModelSet model,
      const std::list<std::string>& selected_filters,
      std::string luminosity_filter,
      std::string igm_type);
};

}
}

#endif // PHZGRIDINFOHANDLER_H

