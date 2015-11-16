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
 * @file src/lib/ComputeModelSedConfig.cpp
 * @date 11/16/15
 * @author nikoapos
 */

#include "ElementsKernel/Exception.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzCLI/ComputeModelSedConfig.h"

namespace po = boost::program_options;
using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzCLI {

static const std::string SED_NAME {"sed-name"};
static const std::string REDDENING_CURVE_NAME {"reddening-curve-name"};
static const std::string EBV_VALUE {"ebv-value"};
static const std::string Z_VALUE {"z-value"};

ComputeModelSedConfig::ComputeModelSedConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<SedProviderConfig>();
  declareDependency<ReddeningProviderConfig>();
  declareDependency<IgmConfig>();
}

auto ComputeModelSedConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Compute Model SED options", {
    {SED_NAME.c_str(), po::value<std::string>()->required(), "The SED name"},
    {REDDENING_CURVE_NAME.c_str(), po::value<std::string>()->required(), "The reddening curve name"},
    {EBV_VALUE.c_str(), po::value<double>()->required(), "The E(B-V) value"},
    {Z_VALUE.c_str(), po::value<double>()->required(), "The redshift value"}
  }}};
}

void ComputeModelSedConfig::initialize(const UserValues& args) {

  XYDataset::QualifiedName sed {args.at(SED_NAME).as<std::string>()};
  XYDataset::QualifiedName red_curve {args.at(REDDENING_CURVE_NAME).as<std::string>()};
  auto ebv = args.at(EBV_VALUE).as<double>();
  auto z = args.at(Z_VALUE).as<double>();
  
  m_grid_axes.reset(new PhzDataModel::ModelAxesTuple {
    PhzDataModel::createAxesTuple({z}, {ebv}, {red_curve}, {sed})
  });
  
  auto sed_data = getDependency<SedProviderConfig>().getSedDatasetProvider()->getDataset(sed);
  if (sed_data == nullptr) {
    throw Elements::Exception() << "Missing data for SED " << sed.qualifiedName();
  }
  m_sed_map.emplace(std::make_pair(sed, std::move(*sed_data)));
  sed_data.reset(nullptr);
  
  auto red_curve_data = getDependency<ReddeningProviderConfig>().getReddeningDatasetProvider()->getDataset(red_curve);
  if (red_curve_data == nullptr) {
    throw Elements::Exception() << "Missing data for reddening curve " << red_curve.qualifiedName();
  }
  m_red_curve_map.emplace(std::make_pair(red_curve, MathUtils::interpolate(*red_curve_data, MathUtils::InterpolationType::LINEAR)));
}

const PhzDataModel::ModelAxesTuple& ComputeModelSedConfig::getGridAxes() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getGridAxes() call on uninitialized ComputeModelSedConfig";
  }
  return *m_grid_axes;
}

const std::map<XYDataset::QualifiedName, XYDataset::XYDataset>& ComputeModelSedConfig::getSedMap() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getSedMap() call on uninitialized ComputeModelSedConfig";
  }
  return m_sed_map;
}

const std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function> >& ComputeModelSedConfig::getReddeningCurveMap() {
  if (getCurrentState() < State::INITIALIZED) {
    throw Elements::Exception() << "getReddeningCurveMap() call on uninitialized ComputeModelSedConfig";
  }
  return m_red_curve_map;
}

} // PhzCLI namespace
} // Euclid namespace



