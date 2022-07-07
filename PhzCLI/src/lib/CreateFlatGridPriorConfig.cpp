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
 * @file src/lib/CreateFlatGridPriorConfig.cpp
 * @date 01/22/16
 * @author nikoapos
 */

#include "PhzCLI/CreateFlatGridPriorConfig.h"
#include "PhzConfiguration/AuxDataDirConfig.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzCLI {

CreateFlatGridPriorConfig::CreateFlatGridPriorConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhzConfiguration::AuxDataDirConfig>();
}

auto CreateFlatGridPriorConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Create Flat Grid Prior options",
           {{"out-grid-name", po::value<std::string>()->required(), "The name of the prior grid to create"}}}};
}

void CreateFlatGridPriorConfig::initialize(const UserValues& args) {
  m_filename = args.at("out-grid-name").as<std::string>();
  if (!m_filename.is_absolute()) {
    auto& aux_dir = getDependency<PhzConfiguration::AuxDataDirConfig>().getAuxDataDir();
    m_filename    = aux_dir / "GenericPriors" / m_filename;
  }
}

const boost::filesystem::path& CreateFlatGridPriorConfig::getGridPriorFilename() const {
  return m_filename;
}

}  // namespace PhzCLI
}  // namespace Euclid
