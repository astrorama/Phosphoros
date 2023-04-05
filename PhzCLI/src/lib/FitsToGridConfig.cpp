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
 * @file src/lib/FitsToGrid.cpp
 * @date 2023/04/03
 * @author Florian Dubath
 */
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "SourceCatalog/SourceAttributes/TableRowAttributeFromRow.h"
#include "Configuration/CatalogConfig.h"

#include "PhzCLI/FitsToGridConfig.h"


namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzCLI {

static Elements::Logging logger = Elements::Logging::getLogger("FitsToGridConfig");

FitsToGridConfig::FitsToGridConfig(long manager_id) : Configuration(manager_id) {
	  declareDependency<PhzConfiguration::ModelGridOutputConfig>();
	  declareDependency<PhzConfiguration::IgmConfig>();
	  declareDependency<PhzConfiguration::ParameterSpaceConfig>();
	  declareDependency<PhzConfiguration::FilterConfig>();
	  declareDependency<PhzConfiguration::ModelNormalizationConfig>();

	  declareDependency<Euclid::Configuration::CatalogConfig>();
}

auto FitsToGridConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
	return{};
}


void FitsToGridConfig::preInitialize(const UserValues&) {
  getDependency<PhzConfiguration::ModelGridOutputConfig>().changeDefaultSubdir("ModelGrids");
}

void FitsToGridConfig::initialize(const UserValues& args) {

  std::shared_ptr<SourceCatalog::AttributeFromRow> handler_ptr {
     new SourceCatalog::TableRowAttributeFromRow {}
   };

   getDependency<Euclid::Configuration::CatalogConfig>().addAttributeHandler(std::move(handler_ptr));
}




}  // namespace PhzConfiguration
}  // namespace Euclid
