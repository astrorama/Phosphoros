#include <chrono>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QTextStream>
#include "Configuration/ConfigManager.h"
#include "PhzConfiguration/PhosphorosRootDirConfig.h"
#include "PhzConfiguration/CatalogDirConfig.h"
#include "PhzConfiguration/AuxDataDirConfig.h"
#include "PhzConfiguration/IntermediateDirConfig.h"
#include "PhzConfiguration/ResultsDirConfig.h"
#include "FileUtils.h"
#include "PreferencesUtils.h"
#include "DefaultOptionsCompleter.h"
#include "Configuration/Utils.h"

namespace Euclid {
namespace PhzQtUI {

PreferencesUtils::PreferencesUtils()
{
}



std::map<std::string,std::map<std::string,std::string>> PreferencesUtils::readUserPreferences(){
  QString path = QString::fromStdString(FileUtils::getGUIConfigPath())+QDir::separator() +"pref.txt";
  QFile file(path);

  auto map = std::map<std::string,std::map<std::string,std::string>>{};
  if (file.open(QIODevice::ReadOnly)){
    while (!file.atEnd())
    {
       QString line = file.readLine();
       auto line_split = line.split(" ");
       if (line_split.length()>2){
         auto catalog = line_split[0].toStdString();
         if (map.find(catalog) == map.end()){
           auto new_cat_map= std::map<std::string,std::string>{};
           map.insert(std::make_pair(catalog,new_cat_map));
         }

         QString val ="";
         for (int i=2;i<line_split.length();++i){
           if (val.length()>0){
             val=val+" ";
           }
           val = val + line_split[i];
         }

         val = val.trimmed();


         map[catalog].insert(
             std::make_pair(line_split[1].toStdString(), val.toStdString()));
       }
    }
  }

  return map;
}

void PreferencesUtils::writeUserPreferences(std::map<std::string,std::map<std::string,std::string>> preferences){
   QString path = QString::fromStdString(FileUtils::getGUIConfigPath())+QDir::separator() +"pref.txt";
   QFile file(path);
   file.open(QIODevice::WriteOnly );
   QTextStream stream(&file);

   for (auto& item : preferences) {
     for (auto& sub_item :item.second){
       stream << QString::fromStdString(item.first) << " " ;
       stream << QString::fromStdString(sub_item.first) <<  " " ;
       stream << QString::fromStdString(sub_item.second) << " \n";
     }
   }

   file.close();
}

void PreferencesUtils::setUserPreference(const std::string& catalog, const std::string& key, const std::string& value ){
  auto map = PreferencesUtils::readUserPreferences();
  if (map.find(catalog) == map.end()){
    auto new_cat_map= std::map<std::string,std::string>{};
    map.insert(std::make_pair(catalog,new_cat_map));
  }

  if (map[catalog].find(key) == map[catalog].end()){
    map[catalog].insert(std::make_pair(key,value));
  } else {
    map[catalog][key] = value;
  }

  PreferencesUtils::writeUserPreferences(map);

}

std::string PreferencesUtils::getUserPreference(const std::string& catalog, const std::string& key){
  auto map = PreferencesUtils::readUserPreferences();
  if (map.find(catalog) != map.end()){
    if (map[catalog].find(key) != map[catalog].end()){
      return map[catalog][key];
    }
  }

  return "";
}

void PreferencesUtils::clearUserPreference(const std::string& catalog, const std::string& key){
  auto map = PreferencesUtils::readUserPreferences();
   if (map.find(catalog) != map.end() && map[catalog].find(key) != map[catalog].end()){
     map[catalog].erase(key);
     PreferencesUtils::writeUserPreferences(map);
   }
}

int PreferencesUtils::getThreadNumberOverride(){
  auto value = getUserPreference("_global_preferences_","Thread-Number");
  if (value.length()==0){
    return 0;
  } else {
    return std::stoi(value);
  }
}

void PreferencesUtils::setThreadNumberOverride(int value){
  if (value<=0) value=0;
  setUserPreference("_global_preferences_","Thread-Number",std::to_string(value));
}


PhysicsUtils::CosmologicalParameters PreferencesUtils::getCosmologicalParameters(){
  auto value_omega_m = getUserPreference("_global_preferences_", "Cosmological-Parameter-Omega-Matter");
  auto value_omega_l = getUserPreference("_global_preferences_", "Cosmological-Parameter-Omega-Lambda");
  auto value_hubble = getUserPreference("_global_preferences_", "Cosmological-Parameter-Hubble");
  if (value_omega_l.length()==0 || value_omega_m.length()==0 || value_hubble.length()==0){
    return PhysicsUtils::CosmologicalParameters{};
  }

  return PhysicsUtils::CosmologicalParameters{ std::stod(value_omega_m),
                                               std::stod(value_omega_l),
                                               std::stod(value_hubble)
                                             };
}

void PreferencesUtils::setCosmologicalParameters(const PhysicsUtils::CosmologicalParameters& parameters){
  setUserPreference("_global_preferences_",
                    "Cosmological-Parameter-Omega-Matter",
                    QString::number(parameters.getOmegaM(),'g',20).toStdString());

  setUserPreference("_global_preferences_",
                    "Cosmological-Parameter-Omega-Lambda",
                    QString::number(parameters.getOmegaLambda(),'g',20).toStdString());

  setUserPreference("_global_preferences_",
                    "Cosmological-Parameter-Hubble",
                    QString::number(parameters.getHubbleConstant(),'g',20).toStdString());
}



std::map<std::string, boost::program_options::variable_value> PreferencesUtils::getThreadConfigurations(){
  std::map<std::string, boost::program_options::variable_value> options{};
  int value = getThreadNumberOverride();
  if (value>0){
    options["thread-no"].value() = boost::any(value);
  }



  return options;
}

std::map<std::string, boost::program_options::variable_value> PreferencesUtils::getCosmologyConfigurations(){
  std::map<std::string, boost::program_options::variable_value> options{};
  auto cosmology = getCosmologicalParameters();
  options["cosmology-omega-m"].value() = boost::any(cosmology.getOmegaM());
  options["cosmology-omega-lambda"].value() = boost::any(cosmology.getOmegaLambda());
  options["cosmology-hubble-constant"].value() = boost::any(cosmology.getHubbleConstant());

  return options;
}

}
}
