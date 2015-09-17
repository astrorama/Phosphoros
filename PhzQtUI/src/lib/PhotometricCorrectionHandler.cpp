#include "PhzQtUI/PhotometricCorrectionHandler.h"
#include "FileUtils.h"

#include <fstream>
#include <vector>

#include <QDir>
#include <QFileInfo>
#include <boost/program_options.hpp>

#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "XYDataset/QualifiedName.h"

#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/AsciiParser.h"

namespace Euclid {
namespace PhzQtUI {

std::list<std::string> PhotometricCorrectionHandler::getCompatibleCorrectionFiles(std::string catalog, std::list<std::string> selected_filters){
      std::string folder = FileUtils::getPhotCorrectionsRootPath(true,catalog);
      std::list<std::string> requiered_filters;

      for(auto& filter: selected_filters){
          requiered_filters.push_back(FileUtils::removeStart(FileUtils::removeStart(filter,folder),QString(QDir::separator()).toStdString()));
      }

      std::list<std::string> file_list;
      QDir dir(QString::fromStdString(folder));
      Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files)) {
          if (!info.isDir()) {
            try{ // If a file cannot be opened or is ill formated: just skip it!
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
            } catch(...){}
          }
      }

      return file_list;
}

PhzDataModel::PhotometricCorrectionMap PhotometricCorrectionHandler::getCorrections(std::string catalog,std::string file){
  std::string folder = FileUtils::getPhotCorrectionsRootPath(true,catalog);
  QFileInfo file_info(QString::fromStdString(folder)+ QDir::separator()+QString::fromStdString(file) );
  std::ifstream in {file_info.absoluteFilePath().toStdString() };
  return PhzDataModel::readPhotometricCorrectionMap(in);
}

void PhotometricCorrectionHandler::writeCorrections(std::string catalog,PhzDataModel::PhotometricCorrectionMap map, std::string file){
  std::string folder = FileUtils::getPhotCorrectionsRootPath(true,catalog);
   QFileInfo file_info(QString::fromStdString(folder)+ QDir::separator()+QString::fromStdString(file) );
   std::ofstream out {file_info.absoluteFilePath().toStdString() };
   PhzDataModel::writePhotometricCorrectionMap(out,map);
   out.close();
}

std::map<std::string, boost::program_options::variable_value> PhotometricCorrectionHandler::GetConfigurationMap(
    std::string catalog,
    std::string output_file_name,
    int iteration_number,
    double tolerance,
    double non_detection,
    std::string method,
    std::string photometric_grid_file,
    std::string training_catalog_file,
    std::string id_column,
    std::string z_column,
    std::list<std::string> filter_excluded)
  {


    auto path_filename = FileUtils::getPhotCorrectionsRootPath(true,catalog)
        + QString(QDir::separator()).toStdString() + output_file_name;


    std::map<std::string, boost::program_options::variable_value> options_map =
           FileUtils::getPathConfiguration(true,false,true,false);

    options_map["catalog-type"].value() = boost::any(catalog);
    options_map["missing-photometry-flag"].value() = boost::any(non_detection);


    options_map["output-phot-corr-file"].value() = boost::any(path_filename);
    options_map["phot-corr-iter-no"].value() = boost::any(iteration_number);
    options_map["phot-corr-tolerance"].value() = boost::any(tolerance);
    options_map["phot-corr-selection-method"].value() = boost::any(method);


    auto path_grid_filename = FileUtils::getPhotmetricGridRootPath(false,catalog)
           + QString(QDir::separator()).toStdString() + photometric_grid_file;
    options_map["model-grid-file"].value() = boost::any(path_grid_filename);

    options_map["input-catalog-file"].value() = boost::any(training_catalog_file);
    options_map["source-id-column-name"].value() = boost::any(id_column);
    options_map["spec-z-column-name"].value() = boost::any(z_column);


    if (filter_excluded.size()>0){
       std::vector < std::string > excluded;
       for (auto& filter : filter_excluded) {
         excluded.push_back(filter);
       }
       options_map["exclude-filter"].value() = boost::any(excluded);
    }

    return options_map;

}

}
}
