/*
 * LuminosityPriorConfig.h
 *
 *  Created on: Sep 8, 2015
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_LUMINOSITYPRIORCONFIG_H_
#define PHZQTUI_PHZQTUI_LUMINOSITYPRIORCONFIG_H_

#include "PhzQtUI/LuminosityFunctionInfo.h"
#include <QDomDocument>
#include <map>
#include <vector>

namespace boost {
namespace program_options {
class variable_value;
}
}  // namespace boost

namespace Euclid {
namespace PhzQtUI {

class LuminosityPriorConfig {

public:
  using SedGroup = std::pair<std::string, std::vector<std::string>>;

  std::string getName() const;
  void        setName(std::string name);

  bool getInMag() const;
  void setInMag(bool in_mag);

  std::vector<SedGroup> getSedGRoups() const;
  void                  setSedGroups(std::vector<SedGroup> sed_groups);

  std::vector<double> getZs() const;
  void                setZs(std::vector<double> zs);

  std::vector<LuminosityFunctionInfo> getLuminosityFunctionList() const;
  void                                setLuminosityFunctionList(std::vector<LuminosityFunctionInfo> functions);

  std::vector<std::vector<LuminosityFunctionInfo>> getLuminosityFunctionArray() const;

  static LuminosityPriorConfig deserialize(QDomDocument& doc);

  QDomDocument serialize() const;

  static std::map<std::string, LuminosityPriorConfig> readFolder(std::string folder);
  static void writeFolder(std::string folder, std::map<std::string, LuminosityPriorConfig> configs);

  std::map<std::string, boost::program_options::variable_value> getBasicConfigOptions(bool input = true) const;
  std::map<std::string, boost::program_options::variable_value> getConfigOptions() const;

  bool isCompatibleWithParameterSpace(double z_min, double z_max, std::vector<std::string> seds) const;

  bool isCompatibleWithZ(double z_min, double z_max) const;
  // missing seds / new seds
  std::pair<std::vector<std::string>, std::vector<std::string>>
  isCompatibleWithSeds(std::vector<std::string> seds) const;

private:
  std::string                         m_name;
  bool                                m_in_mag = true;
  std::vector<SedGroup>               m_sed_groups;
  std::vector<double>                 m_zs;
  std::vector<LuminosityFunctionInfo> m_luminosity_function_list{};
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif /* PHZQTUI_PHZQTUI_LUMINOSITYPRIORCONFIG_H_ */
