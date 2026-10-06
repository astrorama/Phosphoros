/*
 * gridHelper.h
 *
 *  Created on: 2024-09-10
 *      Author: fdubath
 */


#include <string>

#include "FileUtils.h"
#include <QDir>
#include <QFile>
#include <QString>

#include "ElementsKernel/Logging.h"
#include "PhzQtUI/PhzGridInfoHandler.h"
#include "PhzQtUI/gridHelper.h"
#include "PreferencesUtils.h"
#include <QMessageBox>
#include "PhzQtUI/DialogGridGeneration.h"
#include "PhzQtUI/DialogGalCorrGridGeneration.h"
#include "PhzQtUI/DialogFilterShiftGridGeneration.h"

namespace Euclid {
namespace PhzQtUI {


static Elements::Logging logger = Elements::Logging::getLogger("gridHelper");

static std::string getAxisDescription(const std::map<std::string, PhzDataModel::ModelAxesTuple>& axes) {
	std::string results = "";
	for (const auto& item : axes) {
		results+="Region name ='"+item.first+"', Card(Z) ="+ std::to_string(std::get<0>(item.second).size())
			                                    +", Card(EBV) ="+ std::to_string(std::get<1>(item.second).size())
			                                    +", Card(RedCurve) ="+ std::to_string(std::get<2>(item.second).size())
			                                    +", Card(SED) ="+ std::to_string(std::get<3>(item.second).size())
												+".\n";
	}
	return results;
}


gridHelper::gridHelper() {}

bool gridHelper::checkGridSelection(bool addFileCheck, bool acceptNewFile, std::string file_name, std::string survey_name) {
  if (file_name.compare("<Enter a new name>") == 0) {
    return false;
  }
  
  if (file_name.compare("") == 0) {
    return false;
  }

  if (!addFileCheck) {
    return true;
  }

  QFileInfo info(QString::fromStdString(FileUtils::getPhotmetricGridRootPath(false, survey_name)) + QDir::separator() + QString::fromStdString(file_name));
  return acceptNewFile || (info.exists());
}


bool gridHelper::checkGalacticGridSelection(bool addFileCheck, bool acceptNewFile, std::string file_name, std::string survey_name) {
  if (file_name.compare("<Enter a new name>") == 0) {
    return false;
  }

  if (file_name.compare("") == 0) {
    return false;
  }

  if (!addFileCheck) {
    return true;
  }

  QFileInfo info(QString::fromStdString(FileUtils::getGalacticCorrectionGridRootPath(true, survey_name)) + QDir::separator() + QString::fromStdString(file_name));
  return acceptNewFile || info.exists();
}


bool gridHelper::checkFilterShiftGridSelection(bool addFileCheck, bool acceptNewFile, std::string file_name, std::string survey_name) {
  if (file_name.compare("<Enter a new name>") == 0) {
    return false;
  }

  if (file_name.compare("") == 0) {
    return false;
  }

  if (!addFileCheck) {
    return true;
  }

  QFileInfo info(QString::fromStdString(FileUtils::getFilterShiftGridRootPath(true, survey_name)) + QDir::separator() + QString::fromStdString(file_name));
  return acceptNewFile || info.exists();
}



bool gridHelper::checkCompatibleModelGrid(std::string file_name, ModelSet& selected_model, std::string survey_name, GridInfoObject& grd_info ) {
  logger.debug()<<"checkCompatibleModelGrid for file "<< file_name;
  auto model_name = selected_model.getName();

  if (std::get<0>(m_cache_compatible_model_grid)==model_name && std::get<1>(m_cache_compatible_model_grid)==file_name){
	  return std::get<2>(m_cache_compatible_model_grid);
  }

  QFileInfo info(QString::fromStdString(FileUtils::getPhotmetricGridRootPath(true, survey_name)) + QDir::separator() + QString::fromStdString(file_name));

  if (!info.exists()) {
	logger.debug() << "checkCompatibleModelGrid: no grid with this name";
	m_cache_compatible_model_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, false};
    return false;
  } else {
    auto  axis = selected_model.getAxesTuple();
    logger.debug() << "checkCompatibleModelGrid => selected_model content :" << getAxisDescription(axis);
 

   
    std::vector<XYDataset::QualifiedName> mag_abs_scaling_filter_vector {};
    for (auto& filter :  grd_info.abs_mag_filter_list) {
          mag_abs_scaling_filter_vector.push_back(XYDataset::QualifiedName(filter));
    }

    auto  possible_files = PhzGridInfoHandler::getCompatibleGridFile(
         survey_name,
	 axis,
	 grd_info.filter_list,
         grd_info.igm,
         grd_info.has_igm_cgm,
         grd_info.IGM_CGM_param_A,
         grd_info.IGM_CGM_param_a,
         grd_info.IGM_CGM_param_c,
	 grd_info.lum_filter,
	 grd_info.pp_lum_filter,
	 PhotometryGrid,
	 mag_abs_scaling_filter_vector
	);

    logger.debug() << "possible_files "<<possible_files.size();

    bool valid = (std::find(possible_files.begin(), possible_files.end(), file_name) != possible_files.end());
    m_cache_compatible_model_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, valid};
    return valid;
  }
}

bool gridHelper::checkCompatibleGalacticGrid(std::string file_name, ModelSet& selected_model, std::string survey_name, GridInfoObject& grd_info) {
  logger.debug()<<"checkCompatibleGalacticGrid for file "<< file_name;
  auto model_name = selected_model.getName();

  if (std::get<0>(m_cache_compatible_galactic_grid)==model_name && std::get<1>(m_cache_compatible_galactic_grid)==file_name){
 	return std::get<2>(m_cache_compatible_galactic_grid);
  }

  QFileInfo info(QString::fromStdString(FileUtils::getGalacticCorrectionGridRootPath(true, survey_name)) + QDir::separator() + QString::fromStdString(file_name));

  if (!info.exists()) {
	logger.debug() << "checkCompatibleGalacticGrid: no grid with this name";
	m_cache_compatible_galactic_grid = std::tuple<std::string, std::string, bool>{model_name, file_name, false};
    return false;
  } else {
    auto  axis = selected_model.getAxesTuple();
    logger.debug() << "checkCompatibleGalacticGrid => selected_model content :" << getAxisDescription(axis);
    
    auto  possible_files = PhzGridInfoHandler::getCompatibleGridFile(
         survey_name, 
         axis, 
		 grd_info.filter_list,
         grd_info.igm,
         grd_info.has_igm_cgm,
         grd_info.IGM_CGM_param_A,
         grd_info.IGM_CGM_param_a,
         grd_info.IGM_CGM_param_c,
		 grd_info.lum_filter,
		 grd_info.pp_lum_filter,
         GalacticReddeningCorrectionGrid);
         
	logger.debug()<<"checkCompatibleGalacticGrid => Files checked "<<possible_files.size()<<" compatible files ";

    bool valid = (std::find(possible_files.begin(), possible_files.end(), file_name) != possible_files.end());
    m_cache_compatible_galactic_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, valid};
    return valid;
  }
}


bool gridHelper::checkCompatibleFilterShiftGrid(std::string file_name, ModelSet& selected_model, std::string survey_name, GridInfoObject& grd_info) {
  auto model_name = selected_model.getName();

  if (std::get<0>(m_cache_compatible_shift_grid)==model_name && std::get<1>(m_cache_compatible_shift_grid)==file_name){
 	return std::get<2>(m_cache_compatible_shift_grid);
  }

  QFileInfo info(QString::fromStdString(FileUtils::getFilterShiftGridRootPath(true, survey_name)) + QDir::separator() + QString::fromStdString(file_name));
  
  if (!info.exists()) {
	logger.debug() << "checkCompatibleFilterShiftGrid: no grid with this name";
	m_cache_compatible_shift_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, false};
    return false;
  } else {
    auto  axis = selected_model.getAxesTuple();
    logger.debug() << "checkCompatibleFilterShiftGrid => selected_model content :" << getAxisDescription(axis);
    
    auto  possible_files = PhzGridInfoHandler::getCompatibleGridFile(
         survey_name, 
         axis, 
		 grd_info.filter_list,
         grd_info.igm,
         grd_info.has_igm_cgm,
         grd_info.IGM_CGM_param_A,
         grd_info.IGM_CGM_param_a,
         grd_info.IGM_CGM_param_c,
		 grd_info.lum_filter,
		 grd_info.pp_lum_filter,
		 FilterShiftCorrectionGrid);
		 
	logger.debug()<<"checkCompatiblecheckCompatibleFilterShiftGridGalacticGrid => Files checked "<<possible_files.size()<<" compatible files ";
		 
    bool valid = (std::find(possible_files.begin(), possible_files.end(), file_name) != possible_files.end());
    m_cache_compatible_shift_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, valid};
    return valid;
  }
}


void gridHelper::resetCache() {
    m_cache_compatible_model_grid = std::tuple<std::string, std::string, bool>{"","",false};
    m_cache_compatible_galactic_grid= std::tuple<std::string, std::string, bool>{"","",false};
    m_cache_compatible_shift_grid= std::tuple<std::string, std::string, bool>{"","",false};
}

std::map<std::string, boost::program_options::variable_value> gridHelper::getGridConfiguration(
                                                                                        const std::list<float>& zs,  
                                                                                        ModelSet& selected_model, 
                                                                                        std::string survey_name, 
                                                                                        GridInfoObject& grd_info, 
                                                                                        std::string file_name) {
    auto config = PhzGridInfoHandler::GetConfigurationMap(
                            survey_name, 
                            file_name, 
                            selected_model, 
                            grd_info.filter_list,
                            grd_info.lum_filter,
                            grd_info.pp_lum_filter,
                            grd_info.abs_mag_filter_list,
                            grd_info.igm,
                            grd_info.has_igm_cgm,
                            grd_info.IGM_CGM_param_A,
                            grd_info.IGM_CGM_param_a,
                            grd_info.IGM_CGM_param_c,
                            zs);

    
    std::string tex_suffix = ".txt";
    std::string text_format                    =(FileUtils::ends_with(file_name, tex_suffix))? "TEXT":"BINARY";
    config["output-model-grid-format"].value() = boost::any(text_format);
    return config;
}

std::map<std::string, boost::program_options::variable_value> gridHelper::getGalacticCorrectionGridConfiguration(
                                                                                                  QWidget* parent,
                                                                                                  std::string catalog_type, 
                                                                                                  GridInfoObject& grd_info, 
                                                                                                  std::string grid_name,
                                                                                                  std::string file_name,
                                                                                                  std::string mwrc) {

      QFileInfo g23_curve_info(QString::fromStdString(FileUtils::getRedCurveRootPath(false)) + QDir::separator() +
                               QString::fromStdString("Gordon23") + QDir::separator() + QString::fromStdString("G23.dat"));
      QFileInfo f99_curve_info(QString::fromStdString(FileUtils::getRedCurveRootPath(false)) + QDir::separator() +
                               QString::fromStdString("F99") + QDir::separator() + QString::fromStdString("F99_3.1.dat"));



      std::string mwrc_arg = "Gordon23/G23";
      if (mwrc=="Fitzpatrick 1999") {
	      if (!f99_curve_info.exists()) {
	          QMessageBox::warning(
	              parent, "Missing Reddening curve...",
	              "The Milky Way reddening curve stored by default in <ReddeningCurves>/F99/F99_3.1.dat is missing. "
	              "This computation need it, please provide it and try again. (You may try to reload the last data pack)",
	              QMessageBox::Ok);
	          return {};
	      }
	      mwrc_arg = "F99/F99_3.1";
      } else {
	      if (!g23_curve_info.exists()) {
	          QMessageBox::warning(
	              parent, "Missing Reddening curve...",
	              "The Milky Way reddening curve stored by default in <ReddeningCurves>/Gordon23/G23.dat is missing. "
	              "This computation need it, please provide it and try again. (You may try to reload the last data pack)",
	              QMessageBox::Ok);
	          return {};
	       }
      }


      std::map<std::string, boost::program_options::variable_value> options_map =
          FileUtils::getPathConfiguration(false, true, true, false);

      options_map["catalog-type"].value()                                = boost::any(catalog_type);
      options_map["output-galactic-correction-coefficient-grid"].value() = boost::any(file_name);

      std::string tex_suffix = ".txt";
      std::string text_format                    =(FileUtils::ends_with(file_name, tex_suffix))? "TEXT":"BINARY";
      options_map["output-galactic-correction-coefficient-grid-format"].value() = boost::any(text_format);

      options_map["model-grid-file"].value()         = boost::any(grid_name);

      options_map["milky-way-reddening-curve-name"].value() = boost::any(mwrc_arg);
      auto global_options                                   = PreferencesUtils::getThreadConfigurations();
      for (auto& pair : global_options) {
        options_map[pair.first] = pair.second;
      }

      global_options = PreferencesUtils::getLogLevelConfigurations();
      for (auto& pair : global_options) {
        options_map[pair.first] = pair.second;
      }

      return options_map;
}

std::map<std::string, boost::program_options::variable_value> gridHelper::getFilterShiftGridConfiguration(
                                                                                        double min_value,
                                                                                        double max_value,
                                                                                        int sample_number,
                                                                                        GridInfoObject& grd_info, 
                                                                                        std::string grid_name,
                                                                                        std::string output_grid_name,
                                                                                        std::string survey_name,
                                                                                        std::string mwrc) {
  std::map<std::string, boost::program_options::variable_value> options_map = FileUtils::getPathConfiguration(false, true, true, false);

  std::string mwrc_arg = "Gordon23/G23";
  if (mwrc=="Fitzpatrick 1999") {
 	  mwrc_arg = "F99/F99_3.1";
  }

  options_map["filter-variation-min-shift"].value()     = boost::any(min_value);
  options_map["filter-variation-max-shift"].value()     = boost::any(max_value);
  options_map["filter-variation-shift-samples"].value() = boost::any(sample_number);
  options_map["milky-way-reddening-curve-name"].value() = boost::any(mwrc_arg);

  auto global_options = PreferencesUtils::getThreadConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  global_options = PreferencesUtils::getLogLevelConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  options_map["catalog-type"].value() = boost::any(survey_name);

  options_map["model-grid-file"].value()         = boost::any(grid_name);
  options_map["output-filter-variation-coefficient-grid"].value()        = boost::any(output_grid_name);
  std::string tex_suffix = ".txt";
  std::string text_format                    =(FileUtils::ends_with(output_grid_name, tex_suffix))? "TEXT":"BINARY";
  options_map["output-filter-variation-coefficient-grid-format"].value() = boost::any(text_format);
  return options_map;
}

bool gridHelper::BuildModelGrid(const std::list<float>& zs, std::string file_name,  ModelSet& selected_model, std::string survey_name, GridInfoObject& grid_info_object, QWidget* parent) {
    if (!gridHelper::checkGridSelection(true, true, file_name, survey_name)) {
        QMessageBox::warning(
            parent, "Unavailable name...",
            "It is not possible to save the Grid under the name you have provided. Please enter a new name.",
            QMessageBox::Ok);
        return false; // grid not generated
    } else {
        if (gridHelper::checkGridSelection(true, false, file_name, survey_name)) {
            if (QMessageBox::warning(parent, "Override existing file...",
                                   "A Model Grid file with the very same name as the one you provided already exist. "
                                   "Do you want to replace it?",
                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
                return true; // grid already ready
            }
        }

        auto config_map = gridHelper::getGridConfiguration(zs, selected_model, survey_name, grid_info_object, file_name);
        std::unique_ptr<DialogGridGeneration> dialog(new DialogGridGeneration());
        dialog->setValues(file_name, config_map,
        selected_model.getNormValue());
        if (dialog->exec()) {
            resetCache();
            return true; // build succeed
        } else {
            return false; // build failed
        }
    }
}

bool gridHelper::BuildMwCorrGrid(std::string file_name, ModelSet& selected_model, std::string survey_name, GridInfoObject& grid_info_object, std::string main_grid_name, std::string mwrc, QWidget* parent){
	if (!gridHelper::checkGalacticGridSelection(true, true, file_name, survey_name)) {
			QMessageBox::warning(parent, "Unavailable name...",
			"It is not possible to save the Galactic Correction Grid under the name you have provided. "
			"Please enter a new name.",
			QMessageBox::Ok);
		return false; // grid not generated
	} else {
		if (gridHelper::checkGalacticGridSelection(true, false, file_name, survey_name)) {
			if (QMessageBox::warning(
					parent, "Override existing file...",
					"A Galactic Correction Grid file with the very same name as the one you provided already exist. "
					"Do you want to replace it?",
					QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
				return true; // grid already ready
			}
		}

		auto config_map = getGalacticCorrectionGridConfiguration(parent, survey_name, grid_info_object, main_grid_name, file_name, mwrc);
		if (config_map.size() > 0) {
			std::unique_ptr<DialogGalCorrGridGeneration> dialog(new DialogGalCorrGridGeneration());
			dialog->setValues(file_name, config_map, selected_model.getNormValue());
			if (dialog->exec()) {
	            resetCache();
				return true; // build succeed
			} else {
				return false; // build failed
			}
		} else {
			return false; // No grid needed
		}
	}
}

bool gridHelper::BuildFilterShiftGrid(std::string file_name,  ModelSet& selected_model, std::string survey_name, GridInfoObject& grid_info_object, std::string main_grid_name, std::string mwrc, double min_value, double max_value, int sample_number, QWidget* parent){
	if (!gridHelper::checkFilterShiftGridSelection(true, true, file_name, survey_name)) {
						 QMessageBox::warning(parent, "Unavailable name...",
						 "It is not possible to save the Filter Variation Coefficients Grid under the name you have "
						 "provided. Please enter a new name.",
						 QMessageBox::Ok);
		return false; // grid not generated
	} else {
		if (gridHelper::checkFilterShiftGridSelection(true, false, file_name, survey_name)) {
		  if (QMessageBox::warning(parent, "Override existing file...",
								   "A Filter Variation Coefficients  Grid file with the very same name as the one you "
								   "provided already exist. "
								   "Do you want to replace it?",
								   QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
			 return true; // grid already ready
		  }
		}

		auto config_map = getFilterShiftGridConfiguration(min_value, max_value, sample_number, grid_info_object, main_grid_name, file_name, survey_name, mwrc);
		if (config_map.size() > 0) {

		  std::unique_ptr<DialogFilterShiftGridGeneration> dialog(new DialogFilterShiftGridGeneration());
		  dialog->setValues(file_name, config_map, selected_model.getNormValue());
		  if (dialog->exec()) {
	          resetCache();
			  return true; // build succeed
		  } else {
		      return false; // build failed
		  }
		} else {
			return false; // No grid needed
		}
	}
}



}  // namespace PhzQtUI
}  // namespace Euclid
