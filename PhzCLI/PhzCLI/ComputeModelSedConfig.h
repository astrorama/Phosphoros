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
 * @file PhzCLI/ComputeModelSedConfig.h
 * @date 11/16/15
 * @author nikoapos
 */

#ifndef _PHZCLI_COMPUTEMODELSEDCONFIG_H
#define _PHZCLI_COMPUTEMODELSEDCONFIG_H

#include <map>
#include <memory>
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDataset.h"
#include "MathUtils/function/Function.h"
#include "Configuration/Configuration.h"
#include "PhzDataModel/PhzModel.h"

namespace Euclid {
namespace PhzCLI {

/**
 * @class ComputeModelSedConfig
 * @brief
 *
 */
class ComputeModelSedConfig : public Configuration::Configuration {

public:

  ComputeModelSedConfig(long manager_id);

  /**
   * @brief Destructor
   */
  virtual ~ComputeModelSedConfig() = default;
  
  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  const PhzDataModel::ModelAxesTuple& getGridAxes();
  
  const std::map<XYDataset::QualifiedName, XYDataset::XYDataset>& getSedMap();
  
  const std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>>& getReddeningCurveMap();

private:

  std::unique_ptr<PhzDataModel::ModelAxesTuple> m_grid_axes;
  std::map<XYDataset::QualifiedName, XYDataset::XYDataset> m_sed_map;
  std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>> m_red_curve_map;
  
}; /* End of ComputeModelSedConfig class */

} /* namespace PhzCLI */
} /* namespace Euclid */


#endif
