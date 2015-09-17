#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QSettings>
#include <QTextStream>
#include "FileUtils.h"

namespace Euclid {
namespace PhzQtUI {

FileUtils::FileUtils()
{
}


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

 PhzConfiguration::PhosphorosPathConfiguration FileUtils::getRootPaths(){
   std::map<std::string, boost::program_options::variable_value> map{};
   return PhzConfiguration::PhosphorosPathConfiguration(map);
 }

std::string FileUtils::getRootPath()  {
    return getRootPaths().getPhosphorosRootDir().generic_string();
}

std::string FileUtils::getRootPath(bool with_separator){
  if (with_separator)
    return (QString::fromStdString(getRootPaths().getPhosphorosRootDir().generic_string())+QDir::separator()).toStdString();

  return getRootPath();
}


std::string FileUtils::getGUIConfigPath(){
  auto path = QString::fromStdString(getRootPath())+QDir::separator() + "config"+QDir::separator() +"GUI";
  QFileInfo info(path);
  if (!info.exists()){
      QDir().mkpath(path);
  }

  return path.toStdString();
}


void FileUtils::savePath(const std::map<std::string,std::string>& path_list){
  QString path = QString::fromStdString(getGUIConfigPath())+QDir::separator() +"path.txt";
  QFile file(path);
  file.open(QIODevice::WriteOnly );
  QTextStream stream(&file);

  for (auto& item : path_list) {
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

 auto default_paths = getRootPaths();



 if (map.find("Catalogs")== map.end()){
   map.insert(std::make_pair("Catalogs",default_paths.getCatalogsDir().generic_string()));
 }

 if (map.find("AuxiliaryData")== map.end()){
   map.insert(std::make_pair("AuxiliaryData",default_paths.getAuxDataDir().generic_string()));
 }

 if (map.find("IntermediateProducts")== map.end()){
   map.insert(std::make_pair("IntermediateProducts",default_paths.getIntermediateDir().generic_string()));
 }

 if (map.find("Results")== map.end()){
   map.insert(std::make_pair("Results",default_paths.getResultsDir().generic_string()));
 }

 if (map.find("LastUsed")== map.end()){
    map.insert(std::make_pair("LastUsed",default_paths.getPhosphorosRootDir().generic_string()));
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

std::map<std::string,std::map<std::string,std::string>> FileUtils::readUserPreferences(){
  QString path = QString::fromStdString(getGUIConfigPath())+QDir::separator() +"pref.txt";
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

void FileUtils::writeUserPreferences(std::map<std::string,std::map<std::string,std::string>> preferences){
   QString path = QString::fromStdString(getGUIConfigPath())+QDir::separator() +"pref.txt";
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

void FileUtils::setUserPreference(const std::string& catalog, const std::string& key, const std::string& value ){
  auto map = FileUtils::readUserPreferences();
  if (map.find(catalog) == map.end()){
    auto new_cat_map= std::map<std::string,std::string>{};
    map.insert(std::make_pair(catalog,new_cat_map));
  }

  if (map[catalog].find(key) == map[catalog].end()){
    map[catalog].insert(std::make_pair(key,value));
  } else {
    map[catalog][key] = value;
  }

  FileUtils::writeUserPreferences(map);

}

std::string FileUtils::getUserPreference(const std::string& catalog, const std::string& key){
  auto map = FileUtils::readUserPreferences();
  if (map.find(catalog) != map.end()){
    if (map[catalog].find(key) != map[catalog].end()){
      return map[catalog][key];
    }
  }

  return "";
}

void FileUtils::clearUserPreference(const std::string& catalog, const std::string& key){
  auto map = FileUtils::readUserPreferences();
   if (map.find(catalog) != map.end() && map[catalog].find(key) != map[catalog].end()){
     map[catalog].erase(key);
     FileUtils::writeUserPreferences(map);
   }
}

std::string FileUtils::getAuxRootPath(){
   return readPath()["AuxiliaryData"];
 }


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

std::string FileUtils::getIntermediaryProductRootPath(bool check, const std::string& catalog_type){
  QString path = QString::fromStdString(readPath()["IntermediateProducts"]);

  if (catalog_type.size()>0){
    path = path + QDir::separator()+ QString::fromStdString(catalog_type);
  } else {
    check=false;
  }

  QFileInfo info(path);
  if (check){
     if (!info.exists()){
        QDir().mkpath(path);
     }
  }
  return info.absoluteFilePath().toStdString();
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
  QString path = QString::fromStdString(FileUtils::getIntermediaryProductRootPath(false,catalog_type))+QDir::separator()+"ModelGrids";
  QFileInfo info(path);
  if (check){
      if (!info.exists()){
          QDir().mkpath(path);
      }
  }
  return info.absoluteFilePath().toStdString();
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

std::string FileUtils::getLuminosityFunctionGridRootPath(bool check, const std::string & catalog_type, const std::string& model){
  QString path = QString::fromStdString(FileUtils::getIntermediaryProductRootPath(check,""))+QDir::separator()
  +QString::fromStdString(catalog_type)
  + QDir::separator()+"LuminosityModelGrids"
  + QDir::separator()+QString::fromStdString(model);
    QFileInfo info(path);
    if (check){
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();

}

}
}
