#ifndef PREFERENCESUTILS_H
#define PREFERENCESUTILS_H
#include <QString>
#include <string>
#include <map>
#include <boost/program_options.hpp>
#include "Configuration/ConfigManager.h"

/**
 * @brief The PreferencesUtils class
 */

namespace Euclid {
namespace PhzQtUI {

class PreferencesUtils
{
public:
  PreferencesUtils();

    /////////////////////////////////////////////////////////
    //// Preferences
    static std::map<std::string,std::map<std::string,std::string>> readUserPreferences();

    static void writeUserPreferences(std::map<std::string,std::map<std::string,std::string>> preferences);

    static void setUserPreference(const std::string& catalog, const std::string& key, const std::string& value );

    static void clearUserPreference(const std::string& catalog, const std::string& key);

    static std::string getUserPreference(const std::string& catalog, const std::string& key);

    static int getThreadNumberOverride();

    static void setThreadNumberOverride(int value);

    static std::map<std::string, boost::program_options::variable_value> getThreadOverrideConfiguration();


};

}
}
#endif // PREFERENCESUTILS_H