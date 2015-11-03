/*
 * LuminosityPriorConfig.h
 *
 *  Created on: Sep 8, 2015
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_LUMINOSITYPRIORCONFIG_H_
#define PHZQTUI_PHZQTUI_LUMINOSITYPRIORCONFIG_H_

#include <vector>
#include <map>
#include "PhzQtUI/LuminosityFunctionInfo.h"
#include <QDomDocument>


namespace boost{
namespace program_options{
 class variable_value;
}
}




namespace Euclid {
namespace PhzQtUI {

class LuminosityPriorConfig {

public:
  
  using SedGroup = std::pair<std::string, std::vector<std::string>>;
  
   std::string getName() const;
   void setName(std::string name);

  bool getInMag() const;
  void setInMag(bool in_mag);

  bool getReddened() const;
  void setReddened(bool reddened);

  std::string getFilterName() const;
  void setFilterName(std::string filter_name);

  std::string getLuminosityModelGridName() const;
  void setLuminosityModelGridName(std::string grid_name);


  std::vector<SedGroup> getSedGRoups() const;
  void setSedGroups(std::vector<SedGroup> sed_groups);

  std::vector<double> getZs() const;
  void setZs(std::vector<double>  zs);

  std::vector<LuminosityFunctionInfo> getLuminosityFunctionList() const;
  void setLuminosityFunctionList(std::vector<LuminosityFunctionInfo>  functions);

  std::vector<std::vector<LuminosityFunctionInfo>> getLuminosityFunctionArray() const;

  static LuminosityPriorConfig deserialize(QDomDocument& doc);

  QDomDocument serialize() const;

  static std::map<std::string, LuminosityPriorConfig> readFolder(std::string folder);
  static void  writeFolder(std::string folder, std::map<std::string, LuminosityPriorConfig> configs);

  std::map<std::string, boost::program_options::variable_value> getBasicConfigOptions(bool input=true) const;
  std::map<std::string,  boost::program_options::variable_value> getConfigOptions() const;


private:
    std::string m_name;
    bool m_in_mag = true;
    bool m_reddened = true;
    std::string m_filter_name;
    std::string m_luminosity_model_grid_name;
    std::vector<SedGroup> m_sed_groups;
    std::vector<double> m_zs;
    std::vector<LuminosityFunctionInfo> m_luminosity_function_list{};

};

}
}

#endif /* PHZQTUI_PHZQTUI_LUMINOSITYPRIORCONFIG_H_ */
