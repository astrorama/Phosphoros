/*
 * gridHelper.h
 *
 *  Created on: 2024-09-10
 *      Author: fdubath
 */

#ifndef PHZQTUI_SRC_LIB_GRIDHELPER_H_
#define PHZQTUI_SRC_LIB_GRIDHELPER_H_

#include <string>
#include <tuple>
#include <list>
#include <map>
#include <QDialog>
#include "ModelSet.h"
#include <boost/program_options.hpp>



namespace Euclid {
namespace PhzQtUI {
class GridInfoObject {
  public:
    GridInfoObject(std::list<std::string> new_filter_list,  std::list<std::string> new_abs_mag_filter_list, std::string new_igm, bool new_has_igm_cgm, double new_IGM_CGM_param_A, double new_IGM_CGM_param_a, double new_IGM_CGM_param_c, std::string new_lum_filter, std::string new_pp_lum_filter){
       filter_list=new_filter_list;
       abs_mag_filter_list=new_abs_mag_filter_list;
       igm = new_igm;
       has_igm_cgm = new_has_igm_cgm;
       IGM_CGM_param_A = new_IGM_CGM_param_A;
       IGM_CGM_param_a = new_IGM_CGM_param_a;
       IGM_CGM_param_c = new_IGM_CGM_param_c;
       lum_filter = new_lum_filter;
       pp_lum_filter = new_pp_lum_filter;
    }
    std::list<std::string> filter_list;
    std::list<std::string> abs_mag_filter_list;
    std::string igm;
    bool has_igm_cgm;
    double IGM_CGM_param_A;
    double IGM_CGM_param_a;
    double IGM_CGM_param_c;
    std::string lum_filter;
    std::string pp_lum_filter;
    
};

class gridHelper {
    public:
        gridHelper();
      
        static bool checkGridSelection(bool addFileCheck, bool acceptNewFile, std::string file_name, std::string survey_name);

        static bool checkGalacticGridSelection(bool addFileCheck, bool acceptNewFile, std::string file_name, std::string survey_name);

        static bool checkFilterShiftGridSelection(bool addFileCheck, bool acceptNewFile, std::string file_name, std::string survey_name);

        bool checkCompatibleModelGrid(std::string file_name, ModelSet& selected_model, std::string survey_name, GridInfoObject& grd_info);

        bool checkCompatibleGalacticGrid(std::string file_name, ModelSet& selected_model, std::string survey_name, GridInfoObject& grd_info);

        bool checkCompatibleFilterShiftGrid(std::string file_name, ModelSet& selected_model, std::string survey_name, GridInfoObject& grd_info);
        
        void resetCache();
        
        static std::map<std::string, boost::program_options::variable_value> getGridConfiguration(const std::list<float>& zs,  
                                                                                                  ModelSet& selected_model, 
                                                                                                  std::string survey_name, 
                                                                                                  GridInfoObject& grd_info, 
                                                                                                  std::string file_name);
                                                                          
        static std::map<std::string, boost::program_options::variable_value> getGalacticCorrectionGridConfiguration(
                                                                                                  QWidget* parent,
                                                                                                  std::string catalog_type, 
                                                                                                  GridInfoObject& grd_info, 
                                                                                                  std::string grid_name,
                                                                                                  std::string file_name,
                                                                                                  std::string mwrc);

        static std::map<std::string, boost::program_options::variable_value> getFilterShiftGridConfiguration(
                                                                                                  double min_value,
                                                                                                  double max_value,
                                                                                                  int sample_number,
                                                                                                  GridInfoObject& grd_info, 
                                                                                                  std::string grid_name,
                                                                                                  std::string output_grid_name,
                                                                                                  std::string survey_name,
                                                                                                  std::string mwrc);
                                                                                                  
                                                                                                                                                                                                     
        bool BuildModelGrid(const std::list<float>& zs, 
                            std::string file_name, 
                            ModelSet& selected_model, 
                            std::string survey_name, 
                            GridInfoObject& grid_info_object, 
                            QWidget* parent);

        bool BuildMwCorrGrid(std::string file_name,  
                             ModelSet& selected_model, 
                             std::string survey_name, 
                             GridInfoObject& grid_info_object, 
                             std::string main_grid_name, 
                             std::string mwrc, 
                             QWidget* parent);

        bool BuildFilterShiftGrid(std::string file_name,  
                                  ModelSet& selected_model, 
                                  std::string survey_name, 
                                  GridInfoObject& grid_info_object, 
                                  std::string main_grid_name, 
                                  std::string mwrc, 
                                  double min_value,
                                  double max_value,
                                  int sample_number,
                                  QWidget* parent);
               
        
        
    private:
        std::tuple<std::string, std::string, bool> m_cache_compatible_model_grid{"","",false};
        std::tuple<std::string, std::string, bool> m_cache_compatible_galactic_grid{"","",false};
        std::tuple<std::string, std::string, bool> m_cache_compatible_shift_grid{"","",false};
 
};

}  // namespace PhzQtUI
}  // namespace Euclid
#endif /* PHZQTUI_SRC_LIB_GRIDHELPER_H_ */
