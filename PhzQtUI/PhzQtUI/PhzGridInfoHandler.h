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
namespace PhosphorosUiDm {


/**
 * @brief The PhzGridInfoHandler class
 */
class PhzGridInfoHandler
{
public:

  static PhzDataModel::ModelAxesTuple getAxesTuple(const ModelSet& model_set);

  static std::list<std::string> getCompatibleGridFile(
      const PhzDataModel::ModelAxesTuple& axes,
      const std::list<std::string> & selected_filters);

  static std::map<std::string, po::variable_value> GetConfigurationMap(
      std::string output_file,
      const PhzDataModel::ModelAxesTuple& axes,
      const std::list<std::string>& selected_filters);
};

}
}

#endif // PHZGRIDINFOHANDLER_H

