#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QSettings>
#include "PhzQtUI/FileUtils.h"

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
    QSettings settings("SDC-CH", "PhosphorosUI");
    QString path =settings.value(QString::fromStdString("General/root-path")).toString()+QDir::separator();
    return path.toStdString();
}

void FileUtils::setRootPath(std::string path) {
    QSettings settings("SDC-CH", "PhosphorosUI");
    settings.setValue(QString::fromStdString("General/root-path"),QString::fromStdString(path));
}

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
std::string FileUtils::getFilterRootPath(bool check)  {
    QString path = QString::fromStdString(FileUtils::getRootPath())+QDir::separator()+"Filter";
    QFileInfo info(path);
    if (check){
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}
std::string FileUtils::getSedRootPath(bool check)  {
    QString path = QString::fromStdString(FileUtils::getRootPath())+QDir::separator()+"SED";
    QFileInfo info(path);
    if (check){;
        if (!info.exists()){
            QDir().mkpath(path);
        }
    }
    return info.absoluteFilePath().toStdString();
}
std::string FileUtils::getRedCurveRootPath(bool check)  {
    QString path = QString::fromStdString(FileUtils::getRootPath())+QDir::separator()+"RedCurve";
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

 std::string FileUtils::getLastUsedPath(){
   QSettings settings("SDC-CH", "PhosphorosUI");

   std::string test_value = "default";
   if (test_value.compare(settings.value(QString::fromStdString("General/last-used-path"), QString::fromStdString(test_value)).toString().toStdString())==0){
     return getRootPath();
   }

   return settings.value(QString::fromStdString("General/last-used-path")).toString().toStdString();
}

 void FileUtils::setLastUsedPath(std::string path){
   QSettings settings("SDC-CH", "PhosphorosUI");
   QFileInfo info(QString::fromStdString(path));
   QString stored_path;
   if (info.isDir()){
     stored_path=info.absoluteFilePath();
   } else{
     stored_path=info.absolutePath();
   }
   settings.setValue(QString::fromStdString("General/last-used-path"),stored_path);
}

}
}
