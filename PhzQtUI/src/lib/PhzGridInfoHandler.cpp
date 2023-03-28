/*
 * GridAxisHandler.cpp
 *
 *  Created on: Feb 24, 2015
 *      Author: fdubath
 */

#include "ElementsKernel/Logging.h"
#include <CCfits/CCfits>
#include <QDir>
#include <QFileInfo>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/program_options.hpp>
#include <cmath>
#include <ctgmath>
#include <fstream>
#include <vector>

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "DefaultOptionsCompleter.h"
#include "FileUtils.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzQtUI/PhzGridInfoHandler.h"
#include "PreferencesUtils.h"
#include "XYDataset/QualifiedName.h"
#include <chrono>


namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("PhzGridInfoHandler");

bool PhzGridInfoHandler::checkGridFileCompatibility(QString file_path,
                                                    const std::map<std::string, PhzDataModel::ModelAxesTuple>& axes,
                                                    const std::list<std::string>& selected_filters,
                                                    const std::string igm_type, const std::string luminosity_filter) {
  logger.debug()<<"Checking compatibility for grid in file "<< file_path.toStdString();
  auto start = std::chrono::high_resolution_clock::now();
  try {  // If a file cannot be opened or is ill formated: just skip it!
    // We directly use the boost archive, because we just need the grid info
    // from the beginning of the file. Reading the full file whould be very
    // slow

    PhzDataModel::PhotometryGridInfo grid_info;
    std::ifstream                    in{file_path.toStdString()};
    boost::archive::text_iarchive    bia{in};
    bia >> grid_info;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000000;
  	logger.debug()<<"Grid info loaded "<< duration << "[s]";
  	start = stop;

    // Check the IGM type compatibility
    if (igm_type != grid_info.igm_method) {
      //  logger.info() << "Incompatible IGM. (Expected: "<< igm_type << " found " << grid_info.igm_method <<  ")";
      return false;
    }

    // Check the Luminosity filter compatibility
    if (luminosity_filter != grid_info.luminosity_filter_name.qualifiedName()) {
      //  logger.info() << "Incompatible Luminosity filter. (Expected: "<< luminosity_filter << " found " <<
      //  grid_info.luminosity_filter_name.qualifiedName() <<  ")";
      return false;
    }
    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000000;
  	logger.debug()<<"IGM and Luminosity filter checked "<< duration << "[s]";
  	start = stop;

    // check the filters
    std::size_t number_found = 0;
    for (auto& filter : grid_info.filter_names) {
      if (std::find(selected_filters.begin(), selected_filters.end(), filter.qualifiedName()) !=
          selected_filters.end()) {
        ++number_found;
      }
    }

    if (selected_filters.size() != number_found) {
      //  logger.info() << "Incompatible Filter number. (Expected: "<< selected_filters.size() << " found " <<
      //  number_found <<  ")";
      return false;
    }
    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000000;
	logger.debug()<<"Filter checked "<< duration << "[s]";
	start = stop;

    // check the axis
    if (grid_info.region_axes_map.size() != axes.size()) {
      //  logger.info() << "Incompatible region number. (Expected: "<< grid_info.region_axes_map.size() << " found " <<
      //  axes.size() <<  ")";
      return false;
    }

    size_t found = 0;
    for (auto& current_axe : axes) {
      for (auto& file_axe : grid_info.region_axes_map) {

        // SED

        auto& sed_axis_file      = std::get<PhzDataModel::ModelParameter::SED>(file_axe.second);
        auto& sed_axis_requested = std::get<PhzDataModel::ModelParameter::SED>(current_axe.second);
        if (sed_axis_file.size() != sed_axis_requested.size()) {
          continue;
        }

        bool all_found = true;
        for (auto& sed_requested : sed_axis_requested) {
          if (std::find(sed_axis_file.begin(), sed_axis_file.end(), sed_requested) == sed_axis_file.end()) {
            all_found = false;
          }
        }

        if (!all_found) {
          continue;
        }

        // RED
        auto& red_axis_file      = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(file_axe.second);
        auto& red_axis_requested = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(current_axe.second);
        if (red_axis_file.size() != red_axis_requested.size()) {
          continue;
        }
        all_found = true;
        for (auto& red_requested : red_axis_requested) {
          if (std::find(red_axis_file.begin(), red_axis_file.end(), red_requested) == red_axis_file.end()) {
            all_found = false;
          }
        }

        if (!all_found) {
          continue;
        }

        std::vector<double> z_axis_file;
        for (double value : std::get<PhzDataModel::ModelParameter::Z>(file_axe.second)) {
          z_axis_file.push_back(value);
        }

        std::vector<double> z_axis_requested;
        for (double value : std::get<PhzDataModel::ModelParameter::Z>(current_axe.second)) {
          z_axis_requested.push_back(value);
        }

        if (z_axis_file.size() != z_axis_requested.size()) {
          continue;
        }

        std::sort(z_axis_file.begin(), z_axis_file.end());
        std::sort(z_axis_requested.begin(), z_axis_requested.end());

        bool match     = true;
        auto z_file_it = z_axis_file.begin();
        for (auto& z_requested : z_axis_requested) {
          if (std::fabs(*z_file_it - z_requested) > 1E-10) {
            match = false;
            break;
          }
          ++z_file_it;
        }

        if (!match) {
          continue;
        }

        std::vector<double> ebv_axis_file;
        for (double value : std::get<PhzDataModel::ModelParameter::EBV>(file_axe.second)) {
          ebv_axis_file.push_back(value);
        }

        std::vector<double> ebv_axis_requested;
        for (double value : std::get<PhzDataModel::ModelParameter::EBV>(current_axe.second)) {
          ebv_axis_requested.push_back(value);
        }

        if (ebv_axis_file.size() != ebv_axis_requested.size()) {
          continue;
        }

        std::sort(ebv_axis_file.begin(), ebv_axis_file.end());
        std::sort(ebv_axis_requested.begin(), ebv_axis_requested.end());

        auto ebv_file_it = ebv_axis_file.begin();
        for (auto& ebv_requested : ebv_axis_requested) {
          if (std::fabs(*ebv_file_it - ebv_requested) > 1E-10) {
            match = false;
            break;
          }
          ++ebv_file_it;
        }

        if (!match) {
          continue;
        }

        ++found;
        break;
      }
    }

    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000000;
	logger.debug()<<"Axis checked "<< duration << "[s]";
	start = stop;

    if (axes.size() != found) {
      //  logger.info() << "Incompatible region.";

      return false;
    }

    return true;
  } catch (...) {
    logger.warn() << "Wrong format for the grid file " << file_path.toStdString();
    return false;
  }
}

std::list<std::string>
PhzGridInfoHandler::getCompatibleGridFile(std::string                                                catalog,
                                          const std::map<std::string, PhzDataModel::ModelAxesTuple>& axes,
                                          const std::list<std::string>& selected_filters, std::string igm_type,
                                          const std::string luminosity_filter, GridType grid_type) {

  std::string rootPath = FileUtils::getPhotmetricGridRootPath(true, catalog);
  if (grid_type == GalacticReddeningCorrectionGrid) {
    rootPath = FileUtils::getGalacticCorrectionGridRootPath(true, catalog);
  } else if (grid_type == FilterShiftCorrectionGrid) {
    rootPath = FileUtils::getFilterShiftGridRootPath(true, catalog);
  }
  std::list<std::string> list;
  if (rootPath.length() > 0) {

    QDir        root_qdir(QString::fromStdString(rootPath));
    QStringList fileNames = root_qdir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    foreach (const QString& fileName, fileNames) {
      auto file_path = root_qdir.absoluteFilePath(fileName);
      //  logger.info() << "Checking parameter compatibility for file :" << file_path.toStdString();
      if (checkGridFileCompatibility(file_path, axes, selected_filters, igm_type, luminosity_filter)) {
        //  logger.info() << "File accepted :" << file_path.toStdString();
        list.push_back(fileName.toStdString());
      }
    }
  }
  return list;
}

std::map<std::string, boost::program_options::variable_value>
PhzGridInfoHandler::GetConfigurationMap(std::string catalog, std::string output_file, ModelSet model,
                                        const std::list<std::string>& selected_filters, std::string luminosity_filter,
                                        std::string igm_type) {

  std::map<std::string, boost::program_options::variable_value> options_map =
      FileUtils::getPathConfiguration(false, true, true, false);

  auto model_option = model.getConfigOptions();
  for (auto& pair : model_option) {
    options_map[pair.first] = pair.second;
  }

  auto global_options = PreferencesUtils::getThreadConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  options_map["normalization-filter"].value()    = boost::any(luminosity_filter);
  std::string sun_sed                            = PreferencesUtils::getUserPreference("AuxData", "SUN_SED");
  options_map["normalization-solar-sed"].value() = boost::any(sun_sed);
  options_map["catalog-type"].value()            = boost::any(catalog);

  options_map["output-model-grid"].value() = boost::any(output_file);

  std::vector<std::string> filter_add_vector;
  for (auto& filter_item : selected_filters) {
    filter_add_vector.push_back(filter_item);
  }
  options_map["filter-name"].value()         = boost::any(filter_add_vector);
  options_map["igm-absorption-type"].value() = boost::any(igm_type);

  return options_map;
}

}  // namespace PhzQtUI
}  // namespace Euclid
