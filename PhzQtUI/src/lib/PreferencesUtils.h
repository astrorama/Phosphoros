#ifndef PREFERENCESUTILS_H
#define PREFERENCESUTILS_H
#include "Configuration/ConfigManager.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include <QString>
#include <boost/program_options.hpp>
#include <map>
#include <string>

/**
 * @brief The PreferencesUtils class
 */

namespace Euclid {
namespace PhzQtUI {

class PreferencesUtils {
public:
  PreferencesUtils();

  /////////////////////////////////////////////////////////
  //// Preferences
  static std::map<std::string, std::map<std::string, std::string>> readUserPreferences();

  static void writeUserPreferences(std::map<std::string, std::map<std::string, std::string>> preferences);

  static void setUserPreference(const std::string& catalog, const std::string& key, const std::string& value);

  static void clearUserPreference(const std::string& catalog, const std::string& key);

  static std::string getUserPreference(const std::string& catalog, const std::string& key);

  static int getThreadNumberOverride();

  static void setThreadNumberOverride(int value);

  static double getMaxMemory();

  static void setMaxMemory(double value);

  static std::string getLogLevel();

  static void setLogLevel(std::string value);

  static int getBufferSize();

  static int getCappedBufferSize(long models, long sampling);

  static void setBufferSize(int value);

  static PhysicsUtils::CosmologicalParameters getCosmologicalParameters();

  static void setCosmologicalParameters(const PhysicsUtils::CosmologicalParameters& parameters);

  static std::map<std::string, boost::program_options::variable_value> getThreadConfigurations();

  static std::map<std::string, boost::program_options::variable_value> getLogLevelConfigurations();

  static std::map<std::string, boost::program_options::variable_value> getCosmologyConfigurations();
};

}  // namespace PhzQtUI
}  // namespace Euclid
#endif  // PREFERENCESUTILS_H
