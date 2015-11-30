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

         map[catalog].insert(
             std::make_pair(line_split[1].toStdString(),
                            line_split[2].toStdString()));
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


std::map<std::string, boost::program_options::variable_value> PreferencesUtils::getThreadOverrideConfiguration(){
  std::map<std::string, boost::program_options::variable_value> options{};
  int value = getThreadNumberOverride();
  if (value>0){
    options["thread-no"].value() = boost::any(value);
  }

  return options;
}


}
}
