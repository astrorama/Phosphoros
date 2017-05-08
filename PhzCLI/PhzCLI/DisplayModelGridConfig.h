/*  
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */  

/**
 * @file PhzCLI/DisplayModelGridConfig.h
 * @date 11/16/15
 * @author nikoapos
 */

#ifndef _PHZCLI_DISPLAYMODELGRIDCONFIG_H
#define _PHZCLI_DISPLAYMODELGRIDCONFIG_H

#include <tuple>
#include <memory>
#include "Configuration/Configuration.h"

namespace Euclid {
namespace PhzCLI {

/**
 * @class DisplayModelGridConfig
 * @brief
 *
 */
class DisplayModelGridConfig : public Configuration::Configuration {

public:

  DisplayModelGridConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~DisplayModelGridConfig() = default;
  
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  const std::string& getRegionName() const;

  std::shared_ptr<std::tuple<size_t, size_t, size_t, size_t>> getRequestedCellCoords() const;

  bool showAllRegionsInfo() const;

  bool showEbvAxis() const;

  bool showGeneric() const;

  bool showOverall() const;

  bool showReddeningCurveAxis() const;

  bool showRedshiftAxis() const;

  bool showSedAxis() const;

  bool exportAsCatalog() const;
  
  const std::string& getOutputFitsName() const;

private:
  
  bool m_export_as_catalog = false;
  std::string m_output_fits_name = "";
  bool m_show_overall;
  bool m_show_all_regions_info;
  std::string m_region_name;
  bool m_show_generic;
  bool m_show_sed_axis;
  bool m_show_red_curve_axis;
  bool m_show_ebv_axis;
  bool m_show_redshift_axis;
  std::shared_ptr<std::tuple<size_t,size_t,size_t,size_t>> m_requested_cell_coords;

}; /* End of DisplayModelGridConfig class */

} /* namespace PhzCLI */
} /* namespace Euclid */


#endif
