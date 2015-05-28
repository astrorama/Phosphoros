#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QSettings>
#include <QTextStream>
#include "PhzQtUI/FileUtils.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"

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



std::string FileUtils::getRootPath()  {

  std::map<std::string, boost::program_options::variable_value> map{};
    auto path_conf= PhzConfiguration::PhosphorosPathConfiguration(map);
    return path_conf.getPhosphorosRootDir().generic_string();

}


std::string FileUtils::getGUIConfigPath(){
  auto path = QString::fromStdString(getRootPath())+QDir::separator() + "config"+QDir::separator() +"GUI";
  QFileInfo info(path);
  if (!info.exists()){
      QDir().mkpath(path);
  }

  return path.toStdString();
}


void FileUtils::savePath(std::map<std::string,std::string> path_list){
  QString path = QString::fromStdString(getGUIConfigPath())+QDir::separator() +"path.txt";
  QFile file(path);
  file.open(QIODevice::WriteOnly );
  QTextStream stream(&file);

  for (auto& item : path_list) {
      stream<< QString::fromStdString(item.first) << " " << QString::fromStdString(item.second) << "\n";
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

 auto base_path = QString::fromStdString(getRootPath())+QDir::separator();


 if (map.find("Catalogs")== map.end()){
   map.insert(std::make_pair("Catalogs",base_path.toStdString()+"Catalogs"));
 }

 if (map.find("AuxiliaryData")== map.end()){
   map.insert(std::make_pair("AuxiliaryData",base_path.toStdString()+"AuxiliaryData"));
 }

 if (map.find("IntermediateProducts")== map.end()){
   map.insert(std::make_pair("IntermediateProducts",base_path.toStdString()+"IntermediateProducts"));
 }

 if (map.find("Results")== map.end()){
   map.insert(std::make_pair("Results",base_path.toStdString()+"Results"));
 }

 if (map.find("LastUsed")== map.end()){
    map.insert(std::make_pair("LastUsed",base_path.toStdString()));
 }

  return map;
}


std::string FileUtils::getLastUsedPath(){
   return readPath()["LastUsed"];
}

 void FileUtils::setLastUsedPath(std::string path){
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

std::string FileUtils::getCatalogRootPath(bool check){
  QString path = QString::fromStdString(readPath()["Catalogs"]);
  QFileInfo info(path);
  if (check){
     if (!info.exists()){
        QDir().mkpath(path);
     }
  }
  return info.absoluteFilePath().toStdString();
}

std::string FileUtils::getIntermediaryProductRootPath(bool check, std::string catalog_name){
  QString path = QString::fromStdString(readPath()["IntermediateProducts"])
  + QDir::separator()+ QString::fromStdString(catalog_name);
  QFileInfo info(path);
  if (check){
     if (!info.exists()){
        QDir().mkpath(path);
     }
  }
  return info.absoluteFilePath().toStdString();
}

std::string FileUtils::getResultRootPath(bool check, std::string catalog_name){
  QString path = QString::fromStdString(readPath()["Results"])
  + QDir::separator()+ QString::fromStdString(catalog_name);
  QFileInfo info(path);
  if (check){
     if (!info.exists()){
        QDir().mkpath(path);
     }
  }
  return info.absoluteFilePath().toStdString();
}


std::string FileUtils::getFilterRootPath(bool check)  {
    QString path = QString::fromStdString(readPath()["AuxiliaryData"])+QDir::separator()+"Filter";
    QFileInfo info(path);
    if (check){
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}
std::string FileUtils::getSedRootPath(bool check)  {


    QString path = QString::fromStdString(readPath()["AuxiliaryData"])+QDir::separator()+"SED";
    QFileInfo info(path);
    if (check){;
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}
std::string FileUtils::getRedCurveRootPath(bool check)  {
    QString path = QString::fromStdString(readPath()["AuxiliaryData"])+QDir::separator()+"RedCurve";
    QFileInfo info(path);
    if (check){
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}


// todo bellow
std::string FileUtils::getModelRootPath(bool check)  {
    QString path = QString::fromStdString(FileUtils::getRootPath())+QDir::separator()+"UI"+QDir::separator()+"ModelSet";
    QFileInfo info(path);
         if (check){

        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}

std::string FileUtils::getMappingRootPath(bool check)  {
    QString path = QString::fromStdString(FileUtils::getRootPath())+QDir::separator()+"UI"+QDir::separator()+"Survey";
    QFileInfo info(path);
    if (check){
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}



std::string FileUtils::getPhotCorrectionsRootPath(bool check)  {
    QString path = QString::fromStdString(FileUtils::getRootPath())+QDir::separator()+"PhotometricCorrections";
    QFileInfo info(path);
    if (check){
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}


std::string FileUtils::getPhotmetricGridRootPath(bool check) {
  QString path = QString::fromStdString(FileUtils::getRootPath())+QDir::separator()+"PhotometricGrid";
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
