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
#include <vector>
#include <boost/program_options.hpp>

#include "PhzConfiguration/PhotometryGridConfiguration.h"
#include "PhzQtUI/PhzGridInfoHandler.h"
#include "PhzQtUI/XYDataSetTreeModel.h"
#include "XYDataset/QualifiedName.h"
#include "PhzQtUI/FileUtils.h"

#include "PhzUITools/InfoOnlyCellManager.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {

PhzDataModel::ModelAxesTuple PhzGridInfoHandler::getAxesTuple(
    const ModelSet& model_set) {
  std::vector<double> zs { };
  std::vector<double> ebvs { };
  std::vector < XYDataset::QualifiedName > reddening_curves { };
  std::vector < XYDataset::QualifiedName > seds { };

  auto rules = model_set.getParameterRules();
  for (auto& rule : rules) {
    auto z_range = rule.second.getZRange();
    if (z_range.getStep() > 0 && z_range.getMax() > z_range.getMin()) {
      for (double z = z_range.getMin(); z <= z_range.getMax();
          z += z_range.getStep()) {
        zs.push_back(z);
      }
    } else {
      zs.push_back(0.);
    }

    auto red_range = rule.second.getEbvRange();
    if (red_range.getStep() > 0 && red_range.getMax() > red_range.getMin()) {
      for (double ebv = red_range.getMin(); ebv <= red_range.getMax(); ebv +=
          red_range.getStep()) {
        ebvs.push_back(ebv);
      }
    } else {
      ebvs.push_back(0.);
    }

    XYDataSetTreeModel treeModel_sed;
    treeModel_sed.loadDirectory(
        FileUtils::getSedRootPath(false), false,
        "SEDs");
    treeModel_sed.setState(rule.second.getSedRootObject(),
        rule.second.getExcludedSeds());
    for (auto sed : treeModel_sed.getSelectedLeaf("")) {
      seds.push_back(XYDataset::QualifiedName { sed });
    }

    XYDataSetTreeModel treeModel_red;
    treeModel_red.loadDirectory(
        FileUtils::getRedCurveRootPath(false), false,
        "Reddening Curves");
    treeModel_red.setState(rule.second.getReddeningRootObject(),
        rule.second.getExcludedReddenings());
    for (auto red : treeModel_red.getSelectedLeaf("")) {
      reddening_curves.push_back(XYDataset::QualifiedName { red });
    }

    // TODO By now we assume that there is a single parameter rules
    break;
  }

  return PhzDataModel::createAxesTuple(zs, ebvs, reddening_curves, seds);
}

std::list<std::string> PhzGridInfoHandler::getCompatibleGridFile(
    std::string catalog,
    const PhzDataModel::ModelAxesTuple& axes,
    const std::list<std::string>& selected_filters,
    std::string igm_type) {

  std::string rootPath = FileUtils::getPhotmetricGridRootPath(true,catalog);
  std::list < std::string > list;

  QDir root_qdir(QString::fromStdString(rootPath));
  QStringList fileNames = root_qdir.entryList(QDir::Files | QDir::NoDotAndDotDot);


  foreach (const QString &fileName , fileNames) {
    std::map < std::string, boost::program_options::variable_value > options_map;

    // auto file_path = root_qdir.path() +QDir::separator()+ fileName;
    auto file_path = root_qdir.absoluteFilePath(fileName);
    options_map["model-grid-file"].value() = boost::any(file_path.toStdString());

    options_map["catalog-name"].value() = boost::any(catalog);
    options_map["intermediate-products-dir"].value() = boost::any(FileUtils::getIntermediaryProductRootPath(false,""));

    try { // If a file cannot be opened or is ill formated: just skip it!
      auto grid_config = PhzConfiguration::PhotometryGridConfiguration(options_map);
      auto grid_info = grid_config.getPhotometryGridInfo();

      // Check the IGM type compatibility
      if (igm_type!=grid_info.igm_method) {
              continue;
      }

      // check the filters
      std::size_t number_found=0;
      for (auto& filter : grid_info.filter_names) {
        if (std::find(selected_filters.begin(), selected_filters.end(), filter.qualifiedName())!=selected_filters.end()) {
          ++number_found;
        }
      }

      if (selected_filters.size() != number_found) {
        continue;
      }


      // check the axis

      auto& sed_axis_file = std::get<PhzDataModel::ModelParameter::SED>(grid_info.axes);
      auto& sed_axis_requested = std::get<PhzDataModel::ModelParameter::SED>(axes);
      if (sed_axis_file.size()!=sed_axis_requested.size()) {
        continue;
      }
      bool all_found=true;
      for(auto& sed_requested : sed_axis_requested) {
        if (std::find(sed_axis_file.begin(), sed_axis_file.end(), sed_requested)==sed_axis_file.end()) {
          all_found = false;
        }
      }
      if (!all_found) {
        continue;
      }

      auto& red_axis_file = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(grid_info.axes);
      auto& red_axis_requested = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(axes);
      if (red_axis_file.size()!=red_axis_requested.size()) {
        continue;
      }
      all_found=true;
      for(auto& red_requested : red_axis_requested) {
        if (std::find(red_axis_file.begin(), red_axis_file.end(), red_requested)==red_axis_file.end()) {
          all_found=false;
        }
      }
      if (!all_found) {
        continue;
      }

      std::vector<double> z_axis_file;
      for(double value : std::get<PhzDataModel::ModelParameter::Z>(grid_info.axes)){
        z_axis_file.push_back(value);
      }

      std::vector<double> z_axis_requested;
      for(double value : std::get<PhzDataModel::ModelParameter::Z>(axes)){
        z_axis_requested.push_back(value);
      }

      if (z_axis_file.size()!=z_axis_requested.size()) {
        continue;
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
        continue;
      }

      std::vector<double> ebv_axis_file;
      for(double value : std::get<PhzDataModel::ModelParameter::EBV>(grid_info.axes)){
        ebv_axis_file.push_back(value);
      }
      std::vector<double> ebv_axis_requested;
      for(double value : std::get<PhzDataModel::ModelParameter::EBV>(axes)){
        ebv_axis_requested.push_back(value);
      }
      if (ebv_axis_file.size()!=ebv_axis_requested.size()) {
        continue;
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
        continue;
      }

      list.push_back(fileName.toStdString());
    } catch(...) {}
  }
  return list;

}

std::map<std::string, boost::program_options::variable_value> PhzGridInfoHandler::GetConfigurationMap(
    std::string catalog,
    std::string output_file, const PhzDataModel::ModelAxesTuple& axes,
    const std::list<std::string>& selected_filters, std::string igm_type) {

  std::map < std::string, boost::program_options::variable_value > options_map;

  options_map["phosphoros-root"].value() = boost::any(FileUtils::getRootPath());
  options_map["aux-data-dir"].value() = boost::any(FileUtils::getAuxRootPath());
  options_map["intermediate-products-dir"].value() = boost::any(FileUtils::getIntermediaryProductRootPath(false,""));
  options_map["catalog-name"].value() = boost::any(catalog);

  auto path_filename = FileUtils::getPhotmetricGridRootPath(true,catalog)
      + QString(QDir::separator()).toStdString() + output_file;
  options_map["output-model-grid"].value() = boost::any(path_filename);

  std::vector < std::string > sed_add_vector;

  for (auto& sed_item : std::get < PhzDataModel::ModelParameter::SED
      > (axes)) {
    sed_add_vector.push_back(sed_item.qualifiedName());
  }
  options_map["sed-name"].value() = boost::any(sed_add_vector);

  std::vector < std::string > red_add_vector;

  for (auto& red_item : std::get
      < PhzDataModel::ModelParameter::REDDENING_CURVE > (axes)) {
    red_add_vector.push_back(red_item.qualifiedName());
  }
  options_map["reddening-curve-name"].value() = boost::any(red_add_vector);

  std::vector < std::string > ebv_value_vector;
  for (auto& ebv_item : std::get < PhzDataModel::ModelParameter::EBV
      > (axes)) {
    ebv_value_vector.push_back(std::to_string(ebv_item));
  }
  options_map["ebv-value"].value() = boost::any(ebv_value_vector);

  std::vector < std::string > z_value_vector;
  for (auto& z_item : std::get < PhzDataModel::ModelParameter::Z
      > (axes)) {
    z_value_vector.push_back(std::to_string(z_item));
  }
  options_map["z-value"].value() = boost::any(z_value_vector);

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

