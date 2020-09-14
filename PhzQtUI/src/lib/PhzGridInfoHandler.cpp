/*
 * GridAxisHandler.cpp
 *
 *  Created on: Feb 24, 2015
 *      Author: fdubath
 */


#include <cmath>
#include <ctgmath>
#include <fstream>
#include <QFileInfo>
#include <QDir>
#include <CCfits/CCfits>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "Configuration/ConfigManager.h"
#include "PhzQtUI/PhzGridInfoHandler.h"
#include "XYDataset/QualifiedName.h"
#include "FileUtils.h"
#include "PreferencesUtils.h"
#include "DefaultOptionsCompleter.h"
#include "Configuration/Utils.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"


namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {

 bool PhzGridInfoHandler::checkGridFileCompatibility(QString file_path,
    const std::map<std::string,PhzDataModel::ModelAxesTuple>& axes,
    const std::list<std::string> & selected_filters,
    std::string igm_type) {


   try { // If a file cannot be opened or is ill formated: just skip it!
         // We directly use the boost iarchive, because we just need the grid info
         // from the beginning of the file. Reading the full file whould be very
         // slow
         PhzDataModel::PhotometryGridInfo grid_info;
         std::ifstream in {file_path.toStdString()};
         boost::archive::binary_iarchive bia {in};
         bia >> grid_info;

         // Check the IGM type compatibility
         if (igm_type!=grid_info.igm_method) {
                 return false;
         }

         // check the filters
         std::size_t number_found=0;
         for (auto& filter : grid_info.filter_names) {
           if (std::find(selected_filters.begin(), selected_filters.end(), filter.qualifiedName())!=selected_filters.end()) {
             ++number_found;
           }
         }

         if (selected_filters.size() != number_found) {
           return false;
         }


         // check the axis
         if (grid_info.region_axes_map.size()!=axes.size()){
           return false;
         }

         size_t found=0;
         for (auto& current_axe : axes){
           for (auto& file_axe : grid_info.region_axes_map){

             // SED

             auto& sed_axis_file = std::get<PhzDataModel::ModelParameter::SED>(file_axe.second);
             auto& sed_axis_requested = std::get<PhzDataModel::ModelParameter::SED>(current_axe.second);
             if (sed_axis_file.size()!=sed_axis_requested.size()) {
               return false;
             }

             bool all_found=true;
             for(auto& sed_requested : sed_axis_requested) {
               if (std::find(sed_axis_file.begin(), sed_axis_file.end(), sed_requested)==sed_axis_file.end()) {
                 all_found = false;
                }
             }

             if (!all_found) {
               return false;
             }

             // RED
             auto& red_axis_file = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(file_axe.second);
             auto& red_axis_requested = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(current_axe.second);
             if (red_axis_file.size()!=red_axis_requested.size()) {
               return false;
             }
             all_found=true;
             for(auto& red_requested : red_axis_requested) {
               if (std::find(red_axis_file.begin(), red_axis_file.end(), red_requested)==red_axis_file.end()) {
                 all_found=false;
               }
             }

             if (!all_found) {
               return false;
             }

             std::vector<double> z_axis_file;
             for(double value : std::get<PhzDataModel::ModelParameter::Z>(file_axe.second)){
               z_axis_file.push_back(value);
             }

             std::vector<double> z_axis_requested;
             for(double value : std::get<PhzDataModel::ModelParameter::Z>(current_axe.second)){
               z_axis_requested.push_back(value);
             }

             if (z_axis_file.size()!=z_axis_requested.size()) {
               return false;
             }

             std::sort(z_axis_file.begin(),z_axis_file.end());
             std::sort(z_axis_requested.begin(),z_axis_requested.end());

             bool match=true;
             auto z_file_it = z_axis_file.begin();
             for(auto& z_requested : z_axis_requested) {
               if (std::fabs(*z_file_it - z_requested) > 1E-10) {
                 match=false;
                 break;
               }
               ++z_file_it;
             }

             if (!match) {
               return false;
             }

             std::vector<double> ebv_axis_file;
             for(double value : std::get<PhzDataModel::ModelParameter::EBV>(file_axe.second)){
               ebv_axis_file.push_back(value);
             }

             std::vector<double> ebv_axis_requested;
             for(double value : std::get<PhzDataModel::ModelParameter::EBV>(current_axe.second)){
               ebv_axis_requested.push_back(value);
             }

             if (ebv_axis_file.size()!=ebv_axis_requested.size()) {
               return false;
             }

             std::sort(ebv_axis_file.begin(),ebv_axis_file.end());
             std::sort(ebv_axis_requested.begin(),ebv_axis_requested.end());

             auto ebv_file_it = ebv_axis_file.begin();
             for(auto& ebv_requested : ebv_axis_requested) {
               if (std::fabs(*ebv_file_it - ebv_requested) > 1E-10) {
                 match=false;
                 break;
               }
               ++ebv_file_it;
             }

             if (!match) {
               return false;
             }

             ++found;
             break;
           }
         }

         if (axes.size()!=found){
           return false;
         }


         return true;
       } catch(...) {
         return false;
       }

 }

std::list<std::string> PhzGridInfoHandler::getCompatibleGridFile(
    std::string catalog,
    const std::map<std::string,PhzDataModel::ModelAxesTuple>& axes,
    const std::list<std::string>& selected_filters,
    std::string igm_type,
    bool model_grid) {

  std::string rootPath = FileUtils::getPhotmetricGridRootPath(true,catalog);
  if (!model_grid){
    rootPath = FileUtils::getGalacticCorrectionGridRootPath(true,catalog);
  }
  std::list < std::string > list;
  if (rootPath.length()>0){

    QDir root_qdir(QString::fromStdString(rootPath));
    QStringList fileNames = root_qdir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    foreach (const QString &fileName , fileNames) {
      auto file_path = root_qdir.absoluteFilePath(fileName);
      if (checkGridFileCompatibility(file_path, axes, selected_filters, igm_type)) {
        list.push_back(fileName.toStdString());
      }
    }
  }
  return list;
}

std::map<std::string, boost::program_options::variable_value> PhzGridInfoHandler::GetConfigurationMap(
    std::string catalog,
    std::string output_file,
    ModelSet model,
    const std::list<std::string>& selected_filters, std::string igm_type) {


  std::map<std::string, boost::program_options::variable_value> options_map =
         FileUtils::getPathConfiguration(false, true, true, false);

  auto model_option = model.getConfigOptions();
  for(auto& pair : model_option){
    options_map[pair.first]=pair.second;
  }

  auto global_options = PreferencesUtils::getThreadConfigurations();
  for(auto& pair : global_options){
      options_map[pair.first]=pair.second;
  }


  options_map["catalog-type"].value() = boost::any(catalog);

  options_map["output-model-grid"].value() = boost::any(output_file);

  std::vector < std::string > filter_add_vector;
  for (auto& filter_item : selected_filters) {
     filter_add_vector.push_back(filter_item);
  }
  options_map["filter-name"].value() = boost::any(filter_add_vector);
  options_map["igm-absorption-type"].value() = boost::any(igm_type);

  return options_map;
}

}
}

