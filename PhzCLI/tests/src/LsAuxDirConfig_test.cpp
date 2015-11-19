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
 * @file tests/src/LsAuxDirConfig_test.cpp
 * @date 11/16/15
 * @author morisset
 */

#include <boost/test/unit_test.hpp>

#include "PhzCLI/LsAuxDirConfig.h"
#include "ConfigManager_fixture.h"

using namespace Euclid::PhzCLI;
namespace po = boost::program_options;

struct LsAuxDirConfig_fixture : public ConfigManager_fixture {

  const std::string STR_CONTENT_TYPE {"type"};
  const std::string STR_GROUP {"group"};
  const std::string STR_DATA {"data"};

  std::map<std::string, po::variable_value> options_map {};

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LsAuxDirConfig_test)

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_test, LsAuxDirConfig_fixture) {

  // Given
  config_manager.registerConfiguration<LsAuxDirConfig>();

  // When
  auto options = config_manager.closeRegistration();

  // Then
  BOOST_CHECK_NO_THROW(options.find(STR_CONTENT_TYPE, false));
  BOOST_CHECK_NO_THROW(options.find(STR_GROUP, false));
  BOOST_CHECK_NO_THROW(options.find(STR_DATA, false));
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getGroup_test, LsAuxDirConfig_fixture) {

  // Given
  config_manager.registerConfiguration<LsAuxDirConfig>();
  config_manager.closeRegistration();

  std::string expected { "Mer" };
  options_map["group"].value() = boost::any(expected);
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<LsAuxDirConfig>().getGroup();

  BOOST_CHECK_EQUAL(result, expected);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getDatasetToShow_test, LsAuxDirConfig_fixture) {

  // Given
  config_manager.registerConfiguration<LsAuxDirConfig>();
  config_manager.closeRegistration();

  std::string expected { "1. 2." };
  options_map["data"].value() = boost::any(expected);
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<LsAuxDirConfig>().getDatasetToShow();

  BOOST_CHECK_EQUAL(result, expected);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(showData_test, LsAuxDirConfig_fixture) {

  // Given
  config_manager.registerConfiguration<LsAuxDirConfig>();
  config_manager.closeRegistration();

  bool expected { true } ;
  std::string data { "1. 2." };
  options_map["data"].value() = boost::any(data);
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<LsAuxDirConfig>().showData();

  BOOST_CHECK_EQUAL(result, expected);
}

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(nodata_showData_test, LsAuxDirConfig_fixture) {

  // Given
  config_manager.registerConfiguration<LsAuxDirConfig>();
  config_manager.closeRegistration();

  bool expected { false } ;
  config_manager.initialize(options_map);
  auto result = config_manager.getConfiguration<LsAuxDirConfig>().showData();

  BOOST_CHECK_EQUAL(result, expected);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
