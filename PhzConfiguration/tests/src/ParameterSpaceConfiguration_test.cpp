/*
 * ParameterSpaceConfiguration_test.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <vector>
#include <algorithm>

#include <boost/test/unit_test.hpp>

#include "PhzConfiguration/ParameterSpaceConfiguration.h"

BOOST_AUTO_TEST_SUITE (ParameterSpaceConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(getProgramOptions_test) {

  auto option_desc = Euclid::PhzConfiguration::ParameterSpaceConfiguration::getProgramOptions();

  // Sed options check
  auto sed_options = Euclid::PhzConfiguration::SedConfiguration::getProgramOptions();
  for(auto option : sed_options.options()) {
    auto desc = option_desc.find_nothrow(option->long_name(), false);
    BOOST_CHECK(desc != nullptr);
  }

  // Redshift options check
  auto redshift_options = Euclid::PhzConfiguration::RedshiftConfiguration::getProgramOptions();
  for(auto option : redshift_options.options()) {
    auto desc = option_desc.find_nothrow(option->long_name(), false);
    BOOST_CHECK(desc != nullptr);
  }

  // Reddening options check
  auto reddening_options = Euclid::PhzConfiguration::ReddeningConfiguration::getProgramOptions();
  for(auto option : reddening_options.options()) {
    auto desc = option_desc.find_nothrow(option->long_name(), false);
    BOOST_CHECK(desc != nullptr);
  }

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



