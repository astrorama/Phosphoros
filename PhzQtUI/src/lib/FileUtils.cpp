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
#include "DefaultOptionsCompleter.h"
#include "Configuration/Utils.h"

#include "PhzUITools/CatalogColumnReader.h"

namespace Euclid {
namespace PhzQtUI {

FileUtils::FileUtils()
{
}

////// Directory and File manipulation
bool FileUtils::removeDir(const QString &dirName)
{

    bool result = true;

    QFileInfo rootInfo(dirName);
    if (rootInfo.isDir()){


        QDir dir(dirName);

        if (dir.exists()) {
            Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
                if (info.isDir()) {
                    result = removeDir(info.absoluteFilePath());
                }
                else {
                    result = QFile::remove(info.absoluteFilePath());
                }

                if (!result) {
                    return result;
                }
            }
            result = QDir().rmdir(dirName);
        }
     } else{
         result = QFile::remove(rootInfo.absoluteFilePath());
     }

    return result;
}

bool FileUtils::copyRecursively(const QString &srcFilePath,
                            const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {

        QDir targetDir(tgtFilePath);
        if (!targetDir.exists()){
            if (!targetDir.mkdir(tgtFilePath))
                     return false;
        }

        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath
                    = srcFilePath + QDir::separator() + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QDir::separator() + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath))
                return false;
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}

bool FileUtils::ends_with(const std::string  & value, const std::string  & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool FileUtils::starts_with(const std::string  & value, const std::string  & begining)
{
    if (begining.size() > value.size()) return false;
    return std::equal(begining.begin(), begining.end(), value.begin());
}

std::string FileUtils::removeExt(const std::string& name, const std::string& ext){
     if (FileUtils::ends_with(name,ext)){
         return name.substr(0,name.length()-ext.length());
     }

      return name;

}

std::string FileUtils::addExt(const std::string& name, const std::string& ext){
  if (FileUtils::ends_with(name,ext)){
           return name;
   }
   return name+ext;
}

std::string FileUtils::removeStart(const std::string& name, const std::string& start){
    if (FileUtils::starts_with(name,start)){
        return name.substr(start.length());
    }

     return name;
}


//// Application base paths as of configuration.
Configuration::ConfigManager& FileUtils::getRootPaths() {
  std::map<std::string, boost::program_options::variable_value> map {};
  completeWithDefaults<PhzConfiguration::PhosphorosRootDirConfig>(map);
  completeWithDefaults<PhzConfiguration::CatalogDirConfig>(map);
  completeWithDefaults<PhzConfiguration::AuxDataDirConfig>(map);
  completeWithDefaults<PhzConfiguration::IntermediateDirConfig>(map);
  completeWithDefaults<PhzConfiguration::ResultsDirConfig>(map);

  long config_manager_id = Configuration::getUniqueManagerId();
  auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
  config_manager.registerConfiguration<PhzConfiguration::PhosphorosRootDirConfig>();
  config_manager.registerConfiguration<PhzConfiguration::CatalogDirConfig>();
  config_manager.registerConfiguration<PhzConfiguration::AuxDataDirConfig>();
  config_manager.registerConfiguration<PhzConfiguration::IntermediateDirConfig>();
  config_manager.registerConfiguration<PhzConfiguration::ResultsDirConfig>();
  config_manager.closeRegistration();
  config_manager.initialize(map);

  return config_manager;
}

std::string FileUtils::getRootPath(bool with_separator) {
  QString path = QString::fromStdString(getRootPaths().getConfiguration<PhzConfiguration::PhosphorosRootDirConfig>().getPhosphorosRootDir().generic_string());
  if (with_separator){
    path = path + QDir::separator();
  }

  return path.toStdString();
}

///// GUI Specific path
std::string FileUtils::getGUIConfigPath(){
  auto path = QString::fromStdString(getRootPath(true)) + "config"+QDir::separator() +"GUI";
  QFileInfo info(path);

  if (!info.exists()){
      QDir().mkpath(path);
  }

  return path.toStdString();
}

std::string FileUtils::getGUILuminosityPriorConfig(bool check, const std::string & catalog_type, const std::string& model){
  QString path = QString::fromStdString(FileUtils::getGUIConfigPath())+QDir::separator()
  +"LuminosityPrior"+QDir::separator()+QString::fromStdString(catalog_type)
  +QDir::separator()+QString::fromStdString(model);
   QFileInfo info(path);

   if (check){
       if (!info.exists()){
           QDir().mkpath(path);
       }
   }

   return info.absoluteFilePath().toStdString();
}

//// Overriadable path

std::string FileUtils::getCatalogRootPath(bool check, const std::string& catalog_type){
  QString path = QString::fromStdString(readPath()["Catalogs"]);

  if (catalog_type.size()>0){
      path = path + QDir::separator()+ QString::fromStdString(catalog_type);
  }

  QFileInfo info(path);
  if (check){
     if (!info.exists()){
        QDir().mkpath(path);
     }
  }
  return info.absoluteFilePath().toStdString();
}

std::string FileUtils::getDefaultCatalogRootPath(){
    return getRootPath(true)+"Catalogs";
}

std::string FileUtils::getAuxRootPath(){
  auto aux_data_folder = QString::fromStdString(readPath()["AuxiliaryData"]);



  return aux_data_folder.toStdString();
}

std::string FileUtils::getDefaultAuxRootPath(){
  return getRootPath(true)+"AuxiliaryData";
}

std::string FileUtils::getIntermediaryProductRootPath(bool check, const std::string& catalog_type){
  QString path = QString::fromStdString(readPath()["IntermediateProducts"]);

  if (catalog_type.size()>0){
    path = path + QDir::separator()+ QString::fromStdString(catalog_type);
  }

  QFileInfo info(path);
  if (check){
     if (!info.exists()){
        QDir().mkpath(path);
     }
  }
  return info.absoluteFilePath().toStdString();
}

std::string FileUtils::getDefaultIntermediaryProductRootPath(){
  return getRootPath(true)+"IntermediateProducts";
}

std::string FileUtils::getResultRootPath(bool check, const std::string& catalog_type, const std::string& cat_file_name){
  QString path = QString::fromStdString(readPath()["Results"]);

  if (catalog_type.size()>0){
     path = path + QDir::separator()+ QString::fromStdString(catalog_type);
   }

  if (catalog_type.size()>0){
      path = path + QDir::separator()+ QString::fromStdString(cat_file_name);
  }

  QFileInfo info(path);
  if (check){
     if (!info.exists()){
        QDir().mkpath(path);
     }
  }
  return info.absoluteFilePath().toStdString();
}


std::string FileUtils::getDefaultResultsRootPath(){
  return getRootPath(true)+"Results";
}


std::map<std::string,std::string> removeDefault(const std::string& root_path,
                                                const std::string& default_path,
                                                const std::string& key,
                                                std::map<std::string,std::string> path_list){
  if (path_list.find(key)!= path_list.end()){
       std::string path = path_list[key];
       if (path==default_path){
             path_list.erase(key);
       }
   }

  return path_list;
}

void FileUtils::savePath(const std::map<std::string,std::string>& path_list){

  // replace the default paths
  std::string root_path = getRootPath(true);
  std::string default_path = getRootPaths().getConfiguration<PhzConfiguration::CatalogDirConfig>().getCatalogDir().generic_string();
  std::string key = "Catalogs";
  auto list = removeDefault(root_path,default_path,key,path_list);

  default_path = getRootPaths().getConfiguration<PhzConfiguration::AuxDataDirConfig>().getAuxDataDir().generic_string();
  key = "AuxiliaryData";
  list = removeDefault(root_path, default_path, key, list);

  default_path = getRootPaths().getConfiguration<PhzConfiguration::IntermediateDirConfig>().getIntermediateDir().generic_string();
  key = "IntermediateProducts";
  list = removeDefault(root_path, default_path, key, list);

  default_path = getRootPaths().getConfiguration<PhzConfiguration::ResultsDirConfig>().getResultsDir().generic_string();
  key = "Results";
  list = removeDefault(root_path, default_path, key, list);

  key = "LastUsed";
  list = removeDefault(root_path, default_path, key, list);

  QString path = QString::fromStdString(getGUIConfigPath())+QDir::separator() +"path.txt";
  QFile file(path);
  file.open(QIODevice::WriteOnly );
  QTextStream stream(&file);
  for (auto& item : list) {

      stream<< QString::fromStdString(item.first) << " " << QString::fromStdString(item.second) << " \n";
  }

  file.close();
}

std::map<std::string,std::string> FileUtils::readPath(){
  QString path = QString::fromStdString(getGUIConfigPath())+QDir::separator() +"path.txt";
  QFile file(path);

  auto map = std::map<std::string,std::string>{};
  if (file.open(QIODevice::ReadOnly)){
    while (!file.atEnd())
    {
       QString line = file.readLine();
       auto line_split = line.split(" ");
       if (line_split.length()>1){
         map.insert(std::make_pair(line_split[0].toStdString(),line_split[1].toStdString()));
       }
    }
  }

  std::string root_path = getRootPath(true);


 if (map.find("Catalogs")== map.end()){
   map.insert(std::make_pair("Catalogs",getRootPaths().getConfiguration<PhzConfiguration::CatalogDirConfig>().getCatalogDir().generic_string()));
 }

 if (map.find("AuxiliaryData")== map.end()){
   map.insert(std::make_pair("AuxiliaryData",getRootPaths().getConfiguration<PhzConfiguration::AuxDataDirConfig>().getAuxDataDir().generic_string()));
 }

 if (map.find("IntermediateProducts")== map.end()){
   map.insert(std::make_pair("IntermediateProducts",getRootPaths().getConfiguration<PhzConfiguration::IntermediateDirConfig>().getIntermediateDir().generic_string()));
 }

 if (map.find("Results")== map.end()){
   map.insert(std::make_pair("Results",getRootPaths().getConfiguration<PhzConfiguration::ResultsDirConfig>().getResultsDir().generic_string()));
 }

 if (map.find("LastUsed")== map.end()){
    map.insert(std::make_pair("LastUsed",root_path));
 }

  return map;
}


std::string FileUtils::getLastUsedPath(){
   return readPath()["LastUsed"];
}

 void FileUtils::setLastUsedPath(const std::string& path){
   auto map = readPath();

   QFileInfo info(QString::fromStdString(path));
   QString stored_path;
   if (info.isDir()){
     stored_path=info.absoluteFilePath();
   } else{
     stored_path=info.absolutePath();
   }

   map["LastUsed"] = stored_path.toStdString();
   savePath(map);
}


 std::string FileUtils::checkFileColumns(const std::string& file_name,
  const std::vector<std::string>& requiered_columns) {
bool not_found = false;
std::string missing_columns = "";

auto column_reader = PhzUITools::CatalogColumnReader(file_name);
std::map<std::string, bool> file_columns;

for (auto& name : column_reader.getColumnNames()) {
  file_columns[name] = true;
}

for (auto& column : requiered_columns) {
  if (file_columns.count(column) == 1) {
    file_columns[column] = false;

  } else {
    if (not_found) {
      missing_columns += ", ";
    }
    missing_columns += "'" + column + "'";
    not_found = true;
  }

}

if (not_found){
  return missing_columns;
} else {
  return "";
}

}



std::string FileUtils::getFilterRootPath(bool check)  {
    QString path = QString::fromStdString(readPath()["AuxiliaryData"])+QDir::separator()+"Filters";
    QFileInfo info(path);
    if (check){
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}



std::string FileUtils::getLuminosityFunctionCurveRootPath(bool check) {
  QString path = QString::fromStdString(readPath()["AuxiliaryData"])+QDir::separator()+"LuminosityFunctionCurves";
  QFileInfo info(path);
  if (check){
      if (!info.exists()){
          QDir().mkpath(path);
      }
  }
  return info.absoluteFilePath().toStdString();
}


std::string FileUtils::getSedRootPath(bool check)  {


    QString path = QString::fromStdString(readPath()["AuxiliaryData"])+QDir::separator()+"SEDs";
    QFileInfo info(path);
    if (check){;
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}
std::string FileUtils::getRedCurveRootPath(bool check)  {
    QString path = QString::fromStdString(readPath()["AuxiliaryData"])+QDir::separator()+"ReddeningCurves";
    QFileInfo info(path);
    if (check){
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}


std::string FileUtils::getCatalogConfigRootPath(bool check)  {
    QString path = QString::fromStdString(FileUtils::getGUIConfigPath())+QDir::separator()+"Catalogs";
    QFileInfo info(path);
         if (check){

        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}

std::string FileUtils::getModelRootPath(bool check)  {
    QString path = QString::fromStdString(FileUtils::getGUIConfigPath())+QDir::separator()+"ParameterSpace";
    QFileInfo info(path);
         if (check){

        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}

std::string FileUtils::getPhotCorrectionsRootPath(bool check, const std::string& catalog_type)  {
    return FileUtils::getIntermediaryProductRootPath(check,catalog_type);
}

std::string FileUtils::getPhotmetricGridRootPath(bool check, const std::string& catalog_type) {
  if (catalog_type.length()>0){
    QString path = QString::fromStdString(FileUtils::getIntermediaryProductRootPath(false,catalog_type))+QDir::separator()+"ModelGrids";
    QFileInfo info(path);
    if (check){
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
  }

  return "";
}


std::string FileUtils::getGalacticCorrectionGridRootPath(bool check, const std::string& catalog_type) {
  if (catalog_type.length()>0){
      QString path = QString::fromStdString(FileUtils::getIntermediaryProductRootPath(false,catalog_type))+QDir::separator()+"GalacticCorrectionCoefficientGrids";
      QFileInfo info(path);
      if (check){
          if (!info.exists()){
              QDir().mkpath(path);
          }
      }
      return info.absoluteFilePath().toStdString();
    }

    return "";
}


std::string FileUtils::getFilterShiftGridRootPath(bool check, const std::string& catalog_type) {
  if (catalog_type.length()>0){
        QString path = QString::fromStdString(FileUtils::getIntermediaryProductRootPath(false,catalog_type))+QDir::separator()+"FilterVariationCoefficientGrids";
        QFileInfo info(path);
        if (check){
            if (!info.exists()){
                QDir().mkpath(path);
            }
        }
        return info.absoluteFilePath().toStdString();
      }

      return "";
}


std::string FileUtils::getSedPriorRootPath() {
  QString path = QString::fromStdString(readPath()["AuxiliaryData"])+QDir::separator()+"GenericPriors"+QDir::separator()+"SedWeight";
  QFileInfo info(path);
  return info.absoluteFilePath().toStdString();
}


void FileUtils::buildDirectories(){
  auto aux_data_folder = QString::fromStdString(FileUtils::getAuxRootPath());

  std::vector<QString> folders{};

  folders.push_back(aux_data_folder);
  folders.push_back(aux_data_folder + QDir::separator()+"AxisPriors");
  folders.push_back(aux_data_folder + QDir::separator()+"AxisPriors"+ QDir::separator()+"ebv");
  folders.push_back(aux_data_folder + QDir::separator()+"AxisPriors"+ QDir::separator()+"red-curve");
  folders.push_back(aux_data_folder + QDir::separator()+"AxisPriors"+ QDir::separator()+"sed");
  folders.push_back(aux_data_folder + QDir::separator()+"AxisPriors"+ QDir::separator()+"z");
  folders.push_back(aux_data_folder + QDir::separator()+"GenericPriors");

  FileUtils::getFilterRootPath(true);
  FileUtils::getSedRootPath(true);
  FileUtils::getRedCurveRootPath(true);
  FileUtils::getLuminosityFunctionCurveRootPath(true);

  FileUtils::getGUIConfigPath();
  FileUtils::getCatalogRootPath(true,"");
  FileUtils::getIntermediaryProductRootPath(true,"");
  FileUtils::getResultRootPath(true,"","");

    for (auto& path : folders){
      QFileInfo info(path);
        if (!info.exists()){
              QDir().mkpath(path);
        }
    }
}


std::map<std::string, boost::program_options::variable_value> FileUtils::getPathConfiguration(bool add_cat,bool add_aux,bool add_inter, bool add_res){
  std::map<std::string, boost::program_options::variable_value> options{};

  options["phosphoros-root"].value() = boost::any(FileUtils::getRootPath(false));

  std::string current_cat_path = FileUtils::getCatalogRootPath(false,"");
  if (add_cat && current_cat_path!=FileUtils::getDefaultCatalogRootPath()){
    options["catalogs-dir"].value() = boost::any(current_cat_path);
  }

  std::string current_aux_path = FileUtils::getAuxRootPath();
  if (add_aux && current_aux_path!=FileUtils::getDefaultAuxRootPath()){
     options["aux-data-dir"].value() = boost::any(current_aux_path);
   }

  std::string current_int_path = FileUtils::getIntermediaryProductRootPath(false,"");
  if (add_inter && current_int_path!=FileUtils::getDefaultIntermediaryProductRootPath()){
    options["intermediate-products-dir"].value() = boost::any(current_int_path);
  }

  std::string current_res_path = FileUtils::getResultRootPath(false,"","");
  if (add_res && current_res_path!=FileUtils::getDefaultResultsRootPath()){
    options["results-dir"].value() = boost::any(current_res_path);
  }

  return options;
}


}
}
