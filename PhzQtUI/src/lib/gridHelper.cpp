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

namespace Euclid {
namespace PhzQtUI {


static Elements::Logging logger = Elements::Logging::getLogger("gridHelper");

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
		 PhotometryGrid);

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




}  // namespace PhzQtUI
}  // namespace Euclid
