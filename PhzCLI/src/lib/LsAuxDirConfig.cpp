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
 * @file src/lib/LsAuxDirConfig.cpp
 * @date 11/16/15
 * @author morisset
 */

#include <memory>
#include "XYDataset/FileParser.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzCLI/LsAuxDirConfig.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "PhzConfiguration/AuxDataDirConfig.h"

namespace po = boost::program_options;
using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzCLI {


LsAuxDirConfig::LsAuxDirConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<AuxDataDirConfig>();
}

auto LsAuxDirConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"", {
      {"type", po::value<std::string>()->required(),
          "The type of the contents to list (one of SEDs, Filters or ReddeningCurves"},
      {"group", po::value<std::string>(),
          "List the contents of the given group"},
      {"data", po::value<std::string>(),
          "Print the data of the given dataset"}
  }}};
}

void LsAuxDirConfig::initialize(const UserValues& args) {
  m_group           = args.count("group") > 0 ? args.at("group").as<std::string>() : "";
  m_dataset_to_show = args.count("data") > 0 ? args.at("data").as<std::string>() : "";
  m_show_data       = args.count("data") > 0;

  // Get the dataset provider to use. If the user didn't gave the data-root-path
  // use the current directory
  std::string path = getDependency<AuxDataDirConfig>().getAuxDataDir().string();
  if ((args.count("type") > 0) && (args.at("type").as<std::string>().size() != 0)) {
    path += "/" + args.at("type").as<std::string>();
  }
  std::unique_ptr<XYDataset::FileParser> file_parser {new XYDataset::AsciiParser{}};
  m_provider = std::shared_ptr<XYDataset::XYDatasetProvider> {
               new XYDataset::FileSystemProvider{path, std::move(file_parser)}
  };

}


} // PhzCLI namespace
} // Euclid namespace



