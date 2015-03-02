#ifndef PHOTOMETRICCORRECTIONHANDLER_H
#define PHOTOMETRICCORRECTIONHANDLER_H

#include <string>
#include <list>
#include <map>
#include <boost/program_options.hpp>
#include "PhzDataModel/PhotometricCorrectionMap.h"


namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {

/**
 * @brief The PhotometricCorrectionHandler class
 */
class PhotometricCorrectionHandler
{
public:

  /**
   *  @brief Scan the Photometric Corrections files and returns the list of those
   * having the right filters.
   *
   * @param selected_filters
   * A map containing the filter of the Survey and a flag indicating if they are
   * selected. Only the selected filters will be taken into account.
   *
   * @return the list of file name corresponding to Corrections files with the
   *  same filters as those seleted.
   */
   static std::list<std::string> getCompatibleCorrectionFiles(std::list<std::string> selected_filters);

  /**
   * @brief read the PhotometricCorrections out of the file which name is provided.
   *
   * @param file
   * the name of the file (relative to the PhotometricCorrections folder)
   *
   * @return the Photometric Corrections as a map where the keys are the filters
   * and the values the correction.
   */
   static PhzDataModel::PhotometricCorrectionMap getCorrections(std::string file);

  /**
   * @brief write the PhotometricCorrections onto the file which name is provided.
   *
   * @param map
   * the PhotometricCorrectionMap
   *
   * @param file
   * the name of the file (relative to the PhotometricCorrections folder)
   */
   static void writeCorrections(PhzDataModel::PhotometricCorrectionMap map, std::string file);

   /**
    * Get the photometric corrections computation executable configuration map
    */
   static std::map<std::string, po::variable_value> GetConfigurationMap(
     std::string output_file_name,
     int iteration_number,
     double tolerance,
     std::string method,
     std::string photometric_grid_file,
     std::string training_catalog_file,
     std::string id_column,
     std::string z_column,
     std::vector<std::string> filter_mappings
   );

};

}
}

#endif // PHOTOMETRICCORRECTIONHANDLER_H


