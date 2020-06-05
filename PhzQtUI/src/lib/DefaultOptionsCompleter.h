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
 * @file src/lib/DefaultOptionsCompleter.h
 * @date 16/11/15
 * @author nikoapos
 */

#ifndef _DEFAULTOPTIONSCOMPLETER_H_
#define _DEFAULTOPTIONSCOMPLETER_H_

#include <map>
#include <string>
#include <chrono>
#include <boost//program_options.hpp>
#include "Configuration/ConfigManager.h"
#include "ElementsKernel/Logging.h"
#include "Configuration/Utils.h"
#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace PhzQtUI {

//static Elements::Logging logger_dbg = Elements::Logging::getLogger("completeWithDefaults");

template <typename T>
static void completeWithDefaults(std::map<std::string, boost::program_options::variable_value>& options_map) {
  long config_manager_id = Configuration::getUniqueManagerId();
  auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
  config_manager.template registerConfiguration<T>();
  auto options_desc = config_manager.closeRegistration();

  boost::program_options::variables_map default_options {};
  boost::program_options::store(
        boost::program_options::command_line_parser({}).options(options_desc).allow_unregistered().run(),
        default_options
  );

 // logger_dbg.info() << "Completing " ;
  for (auto& pair : default_options) {
    if (options_map.count(pair.first) == 0) {
      //logger_dbg.info() << pair.first << " - " ;
      options_map.emplace(pair);
    }
  }
}

}
}

#endif /* _DEFAULTOPTIONSCOMPLETER_H_ */

