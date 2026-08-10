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

bool PhzGridInfoHandler::checkGridFileCompatibility(const QString      file_path,
                                                    const std::map<std::string, PhzDataModel::ModelAxesTuple>& axes,
                                                    const std::list<std::string>& selected_filters,
                                                    const std::string igm_type,
                                                    bool              igm_cgm,
                                                    double            igm_cgm_param_A,
                                                    double            igm_cgm_param_a,
                                                    double            igm_cgm_param_c, 
                                                    const std::string luminosity_filter,
                                                    const std::string luminosity_pp_filter,
                                                    const std::vector<XYDataset::QualifiedName> scaling_filter_names) {
  logger.debug()<<"Checking compatibility for grid in file "<< file_path.toStdString();
  auto start = std::chrono::high_resolution_clock::now();
  try {  // If a file cannot be opened or is ill formated: just skip it!
    // We directly use the boost archive, because we just need the grid info
    // from the beginning of the file. Reading the full file whould be very
    // slow
    std::string file_name = file_path.toStdString();
    PhzDataModel::PhotometryGridInfo grid_info;
    std::ifstream                    in{file_name};
    std::string tex_suffix = ".txt";
    if (FileUtils::ends_with(file_name, tex_suffix)) {
       boost::archive::text_iarchive    bia{in};
       bia >> grid_info;
    } else {
       boost::archive::binary_iarchive   bia{in};
       bia >> grid_info;
    }

    
 
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
  	logger.debug()<<"Grid info loaded "<< duration << "[ms]";
  	start = stop;

    // Check the IGM type compatibility
    if (igm_type != grid_info.igm_method) {
      logger.debug() << "Incompatible IGM. (Expected: "<< igm_type << " found " << grid_info.igm_method <<  ")";
      return false;
    }

    // Check the IGM type compatibility
    if (igm_cgm != grid_info.cgm) {
      logger.debug() << "Incompatible IGM-CGM. (Expected: "<< igm_cgm << " found " << grid_info.cgm <<  ")";
      return false;
    }
    
    if (igm_cgm) {
        // Check the IGM-CGM parameters compatibility
        if (igm_cgm_param_A != grid_info.cgm_A) {
          logger.debug() << "Incompatible IGM parameter. (Expected: "<< igm_cgm_param_A << " found " << grid_info.cgm_A <<  ")";
          return false;
        }
            
        if (igm_cgm_param_a != grid_info.cgm_a) {
          logger.debug() << "Incompatible IGM. (Expected: "<< igm_cgm_param_a << " found " << grid_info.cgm_a <<  ")";
          return false;
        }
        
        if (igm_cgm_param_c != grid_info.cgm_c) {
          logger.debug() << "Incompatible IGM. (Expected: "<< igm_cgm_param_c << " found " << grid_info.cgm_c <<  ")";
          return false;
        }
    }
   

    // Check the Luminosity filter compatibility
    if (luminosity_filter != grid_info.luminosity_filter_name.qualifiedName()) {
        logger.debug() << "Incompatible Luminosity filter. (Expected: "<< luminosity_filter << " found " <<
        grid_info.luminosity_filter_name.qualifiedName() <<  ")";
      return false;
    }

    // Check the PP Luminosity filter compatibility
    if (luminosity_pp_filter != grid_info.luminosity_pp_filter_name.qualifiedName()) {
 	   logger.debug() << "Incompatible PP Luminosity filter. (Expected: "<< luminosity_pp_filter << " found " <<
 	   grid_info.luminosity_pp_filter_name.qualifiedName() <<  ")";
	   return false;
    }

    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
  	logger.debug()<<"IGM and Luminosity filter checked "<< duration << "[ms]";
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
        logger.debug() << "Incompatible Filter number. (Expected: "<< selected_filters.size() << " found " <<
        number_found <<  ")";
      return false;
    }
    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
	logger.debug()<<"Filter checked "<< duration << "[ms]";
	start = stop;

    // check the scaling_filter_names
    for(auto& scaling_filter: scaling_filter_names){
        logger.info()<<"Checking "<< scaling_filter.qualifiedName() ;
        auto iter = std::find( grid_info.scaling_filter_names.begin(), grid_info.scaling_filter_names.end(), scaling_filter);
        if ( iter == grid_info.scaling_filter_names.end()) {
            logger.debug() << "Missing Scaling Filter. "<< scaling_filter.qualifiedName() << " is not in the grid scaling filters)";
            return false;
      } else { 
       logger.debug() << "Found Filter. "<< (*iter).qualifiedName() << "in the grid scaling filters)";
         
      }
    }


    // check the axis
    if (grid_info.region_axes_map.size() != axes.size()) {
        logger.debug() << "Incompatible region number. (Expected: "<< grid_info.region_axes_map.size() << " found " <<
        axes.size() <<  ")";
      return false;
    }

    size_t found = 0;
    
    for (auto& current_axe : axes) {
      for (auto& file_axe : grid_info.region_axes_map) {
        if (current_axe.first!=file_axe.first){
            continue;
        }
        
        logger.debug() << "Checking region " << current_axe.first ;
        
        // SED

        auto& sed_axis_file      = std::get<PhzDataModel::ModelParameter::SED>(file_axe.second);
        auto& sed_axis_requested = std::get<PhzDataModel::ModelParameter::SED>(current_axe.second);
        if (sed_axis_file.size() != sed_axis_requested.size()) {
          logger.debug() << "Incompatible SED number in the region " << sed_axis_file.size() << " requested : " << sed_axis_requested.size();
          continue;
        }

        bool all_found = true;
        for (auto& sed_requested : sed_axis_requested) {
          if (std::find(sed_axis_file.begin(), sed_axis_file.end(), sed_requested) == sed_axis_file.end()) {
            all_found = false;
          }
        }

        if (!all_found) {
          logger.debug() << "Incompatible SED values in the region";
          continue;
        }

        // RED
        auto& red_axis_file      = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(file_axe.second);
        auto& red_axis_requested = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(current_axe.second);
        if (red_axis_file.size() != red_axis_requested.size()) {
          logger.debug() << "Incompatible RED number in the region";
          continue;
        }
        all_found = true;
        for (auto& red_requested : red_axis_requested) {
          if (std::find(red_axis_file.begin(), red_axis_file.end(), red_requested) == red_axis_file.end()) {
            all_found = false;
          }
        }

        if (!all_found) {
          logger.debug() << "Incompatible RED value in the region";
          continue;
        }

        size_t found_z = 0;
        double max_diff = 0.000001;
        
        if (std::get<PhzDataModel::ModelParameter::Z>(file_axe.second).size()!= std::get<PhzDataModel::ModelParameter::Z>(current_axe.second).size()) {
           logger.debug() <<  "Incompatible Z number in the region " << std::get<PhzDataModel::ModelParameter::Z>(file_axe.second).size() << "-" <<  std::get<PhzDataModel::ModelParameter::Z>(current_axe.second).size();
           continue;
        }
        
        for (double value_file : std::get<PhzDataModel::ModelParameter::Z>(file_axe.second)) {
            for (double value_current : std::get<PhzDataModel::ModelParameter::Z>(current_axe.second)) {
                if (std::abs(value_file-value_current)<max_diff) {
                    found_z+=1;
                    break;
                }
            }
        }
        
        if (found_z != std::get<PhzDataModel::ModelParameter::Z>(file_axe.second).size()) {
          logger.debug() << "Incompatible Z value in the region: found " << found_z << " expected " << std::get<PhzDataModel::ModelParameter::Z>(file_axe.second).size();
          continue;
        }
        
        size_t found_ebv = 0;
        
        if (std::get<PhzDataModel::ModelParameter::EBV>(file_axe.second).size()!= std::get<PhzDataModel::ModelParameter::EBV>(current_axe.second).size()) {
           logger.debug() <<  "Incompatible EBV number in the region " << std::get<PhzDataModel::ModelParameter::EBV>(file_axe.second).size() << "-" <<  std::get<PhzDataModel::ModelParameter::EBV>(current_axe.second).size();
           continue;
        }
        
        for (double value_file : std::get<PhzDataModel::ModelParameter::EBV>(file_axe.second)) {
            for (double value_current : std::get<PhzDataModel::ModelParameter::EBV>(current_axe.second)) {
                if (std::abs(value_file-value_current)<max_diff) {
                    found_ebv+=1;
                    break;
                }
            }
        }
        
        if (found_ebv != std::get<PhzDataModel::ModelParameter::EBV>(file_axe.second).size()) {
          logger.debug() << "Incompatible EBV value in the region";
          continue;
        }
        
        ++found;
        break;
      }
    }

    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
	logger.debug()<<"Axis checked for "<<file_path.toStdString()<<" "<< duration << "[ms]";
	start = stop;

    if (axes.size() != found) {
        logger.debug() << "Incompatible region.";

      return false;
    }
    logger.debug() << "return true.";
    return true;
  } catch (...) {
    logger.warn() << "Wrong format for the grid file " << file_path.toStdString();
    return false;
  }
}

std::list<std::string>
PhzGridInfoHandler::getCompatibleGridFile(const std::string  catalog,
                                          const std::map<std::string, PhzDataModel::ModelAxesTuple>& axes,
                                          const std::list<std::string>& selected_filters, 
                                          const std::string igm_type,
                                          bool  igm_cgm,
                                          double igm_cgm_param_A,
                                          double igm_cgm_param_a,
                                          double igm_cgm_param_c, 
                                          const std::string luminosity_filter, 
                                          const std::string luminosity_pp_filter,
                                          const GridType grid_type,
                                          const std::vector<XYDataset::QualifiedName> scaling_filter_names) {
  auto start = std::chrono::high_resolution_clock::now();
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
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
    logger.debug()<<"getCompatibleGridFile => Got the files in  "<<rootPath<<" total: "<<fileNames.size()<<" "<< duration << "[ms]";
    start = stop;

    foreach (const QString& fileName, fileNames) {
      auto file_path = root_qdir.absoluteFilePath(fileName);
      logger.debug() << "Checking parameter compatibility for file :" << file_path.toStdString();
      if (checkGridFileCompatibility(file_path, axes, selected_filters, igm_type, igm_cgm, igm_cgm_param_A, igm_cgm_param_a, igm_cgm_param_c, luminosity_filter, luminosity_pp_filter, scaling_filter_names)) {
        logger.debug() << "File accepted :" << file_path.toStdString();
        list.push_back(fileName.toStdString());
      }
    }

    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
    logger.debug()<<"getCompatibleGridFile => Checked the files total compatible : "<<list.size()<<" "<< duration << "[ms]";
    start = stop;
  }
  return list;
}

std::map<std::string, boost::program_options::variable_value>
PhzGridInfoHandler::GetConfigurationMap(const std::string catalog, 
                                        const std::string output_file, 
                                        const ModelSet    model,
                                        const std::list<std::string>& selected_filters,
                                        const std::string luminosity_filter,
                                        const std::string luminosity_pp_filter,
                                        const std::list<std::string>& abs_mag_selected_filters,
                                        const std::string igm_type,
                                        bool              igm_cgm,
                                        double            igm_cgm_param_A,
                                        double            igm_cgm_param_a,
                                        double            igm_cgm_param_c,  
                                        const std::list<float>& zs) {

  std::map<std::string, boost::program_options::variable_value> options_map =
      FileUtils::getPathConfiguration(false, true, true, false);

  auto model_option = model.getConfigOptions(zs);
  for (auto& pair : model_option) {
    options_map[pair.first] = pair.second;
  }

  auto global_options = PreferencesUtils::getThreadConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  options_map["normalization-filter"].value()    = boost::any(luminosity_filter);
  options_map["normalization-pp-filter"].value()    = boost::any(luminosity_pp_filter);
  std::string sun_sed                            = PreferencesUtils::getUserPreference("AuxData", "SUN_SED");
  options_map["normalization-solar-sed"].value() = boost::any(sun_sed);
  options_map["catalog-type"].value()            = boost::any(catalog);

  options_map["output-model-grid"].value() = boost::any(output_file);

  std::vector<std::string> filter_add_vector;
  for (auto& filter_item : selected_filters) {
    filter_add_vector.push_back(filter_item);
  }
  options_map["filter-name"].value()         = boost::any(filter_add_vector);
  
  std::vector<std::string> abs_mag_filter_add_vector;
  for (auto& filter_item : abs_mag_selected_filters) {
    abs_mag_filter_add_vector.push_back(filter_item);
  }
  options_map["abs-mag-filters"].value()     = boost::any(abs_mag_filter_add_vector);
  
  
  options_map["igm-absorption-type"].value() = boost::any(igm_type);
  if (igm_cgm && igm_type!="OFF"){
      std::string yes="YES";
      options_map["igm-absorption-add-cgm"].value() = boost::any(yes);
      options_map["igm-absorption-cgm-A"].value() = boost::any(igm_cgm_param_A);
      options_map["igm-absorption-cgm-a"].value() = boost::any(igm_cgm_param_a);
      options_map["igm-absorption-cgm-c"].value() = boost::any(igm_cgm_param_c);
  }
  
  return options_map;
}

}  // namespace PhzQtUI
}  // namespace Euclid
