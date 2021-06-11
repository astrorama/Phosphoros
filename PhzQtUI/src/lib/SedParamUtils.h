#ifndef SED_PARAM_UTILS_H
#define SED_PARAM_UTILS_H

#include <string>
#include <set>
#include "PhzQtUI/ModelSet.h"

/**
 * @brief The SedParamUtils class
 */

namespace Euclid {
namespace PhzQtUI {

class SedParamUtils {
public:
  SedParamUtils();

  static std::string getFile(const XYDataset::QualifiedName& sed);

  static std::set<std::string> getParameterList(const std::string& file);

  static std::string getParameter(const std::string& file, const std::string& key_word);

  static std::string getName(const std::string& file);


  static std::set<std::string> listAvailableParam(const ModelSet& model);

  static bool createPPConfig(const ModelSet& model, std::set<std::string> params, std::string out_path);

};

}
}

#endif // SED_PARAM_UTILS_H
