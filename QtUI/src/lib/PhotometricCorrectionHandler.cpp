#include "QtUI/PhotometricCorrectionHandler.h"
#include "QtUI/FileUtils.h"

#include <fstream>

#include <QDir>
#include <QFileInfo>

#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "XYDataset/QualifiedName.h"

#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/AsciiParser.h"

namespace Euclid {
namespace PhosphorosUiDm {


PhotometricCorrectionHandler::PhotometricCorrectionHandler(){}

std::list<std::string> PhotometricCorrectionHandler::getCompatibleCorrectionFiles(std::map<std::string,bool> selected_filters){
      std::string folder = FileUtils::getPhotCorrectionsRootPath(true);
      std::list<std::string> requiered_filters;

      for(auto& filter_pair: selected_filters){

        if (filter_pair.second){
          requiered_filters.push_back(FileUtils::removeStart(FileUtils::removeStart(filter_pair.first,folder),QString(QDir::separator()).toStdString()));
        }
      }

      std::list<std::string> file_list;
      QDir dir(QString::fromStdString(folder));
      Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files)) {
          if (!info.isDir()) {
            std::ifstream in {info.absoluteFilePath().toStdString() };
            auto result = PhzDataModel::readPhotometricCorrectionMap(in);

            if (result.size()==requiered_filters.size()){
              bool match=true;
                for(auto& name : requiered_filters){
                  if (result.count(name)==0){
                    match=false;
                  }
                }

               if (match){
                 file_list.push_back(info.fileName().toStdString());
               }

            }
          }
      }

      return file_list;
}


}
}
