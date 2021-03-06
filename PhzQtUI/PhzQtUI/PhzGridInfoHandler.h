#ifndef PHZGRIDINFOHANDLER_H
#define PHZGRIDINFOHANDLER_H

#include "PhzDataModel/PhzModel.h"
#include "PhzQtUI/ModelSet.h"
#include <list>
#include <map>
#include <string>

namespace boost {
namespace program_options {
class variable_value;
}
}  // namespace boost

namespace Euclid {
namespace PhzQtUI {

enum GridType { PhotometryGrid = 1, GalacticReddeningCorrectionGrid = 2, FilterShiftCorrectionGrid = 3 };

/**
 * @brief The PhzGridInfoHandler class
 */
class PhzGridInfoHandler {
public:
  static bool checkGridFileCompatibility(QString                                                    file_path,
                                         const std::map<std::string, PhzDataModel::ModelAxesTuple>& axes,
                                         const std::list<std::string>& selected_filters, const std::string igm_type,
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
  static std::list<std::string> getCompatibleGridFile(std::string catalog,
                                                      const std::map<std::string, PhzDataModel::ModelAxesTuple>& axes,
                                                      const std::list<std::string>& selected_filters,
                                                      std::string igm_type, const std::string luminosity_filter,
                                                      GridType grid_type = PhotometryGrid);

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
  static std::map<std::string, boost::program_options::variable_value>
  GetConfigurationMap(std::string catalog, std::string output_file, ModelSet model,
                      const std::list<std::string>& selected_filters, std::string luminosity_filter,
                      std::string igm_type);
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // PHZGRIDINFOHANDLER_H
