/*
 * RedshiftConfiguration_test.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"
#include "PhzConfiguration/RedshiftConfiguration.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;

struct RedshiftConfiguration_Fixture {

  std::map<std::string, po::variable_value> options_map;
  std::vector<std::string> z_range_vector;
  std::vector<std::string> z_value_vector;

  RedshiftConfiguration_Fixture() {

    z_range_vector.push_back("0. 2. 0.5");
    // Fill up the map
    options_map["z-range"].value() = boost::any(z_range_vector);

  }
  ~RedshiftConfiguration_Fixture() {
  }


};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (RedshiftConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = Euclid::PhzConfiguration::RedshiftConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow("z-range", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("z-value", false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------
// Test the getZList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_function_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getZList function");
  BOOST_TEST_MESSAGE(" ");

  cf::RedshiftConfiguration rconf(options_map);
  auto z_list = rconf.getZList();

  BOOST_CHECK_EQUAL(z_list[1], 0.5);
  BOOST_CHECK_EQUAL(z_list[3], 1.5);
  BOOST_CHECK_EQUAL(z_list[4], 2);

}

//-----------------------------------------------------------------------------
// Test the getZList function and add z_value
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_added_zvalue_function_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getZList function in addition an z-value");
  BOOST_TEST_MESSAGE(" ");

  z_value_vector.push_back("1.8");
  z_value_vector.push_back("1.1");
  options_map["z-value"].value() = boost::any(z_value_vector);

  cf::RedshiftConfiguration rconf(options_map);
  auto z_list = rconf.getZList();

  BOOST_CHECK_EQUAL(z_list[1], 0.5);
  BOOST_CHECK_EQUAL(z_list[3], 1.1);
  BOOST_CHECK_EQUAL(z_list[4], 1.5);
  BOOST_CHECK_EQUAL(z_list[5], 1.8);

}

//-----------------------------------------------------------------------------
// Test the getZList function and add several ranges
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_more_ranges_function_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getZList function and more ranges");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> z_ranges_vector;

  z_ranges_vector.push_back("0. 2 0.5");
  z_ranges_vector.push_back("3. 6. 1.");


  options_map["z-range"].value() = boost::any(z_ranges_vector);

  cf::RedshiftConfiguration rconf(options_map);
  auto z_list = rconf.getZList();

  BOOST_CHECK_EQUAL(z_list[1], 0.5);
  BOOST_CHECK_EQUAL(z_list[3], 1.5);
  BOOST_CHECK_EQUAL(z_list[5], 3);
  BOOST_CHECK_EQUAL(z_list[8], 6);

}

//-----------------------------------------------------------------------------
// Test the getZList function and forbidden ranges
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_forbidden_ranges_function_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getZList function and forbidden ranges");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> z_ranges_vector;

  // No overlap allowed
  z_ranges_vector.push_back("0. 2 0.5");
  z_ranges_vector.push_back("1.5 6. 1.");

  options_map["z-range"].value() = boost::any(z_ranges_vector);

  cf::RedshiftConfiguration rconf(options_map);

  BOOST_CHECK_THROW(rconf.getZList(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the getZList function and 2 ranges and at boundaries
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getZList_boundaries_function_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getZList function with boundaries");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> z_ranges_vector;
  std::vector<std::string> z_values_vector;

  z_ranges_vector.push_back("0. 2 0.5");
  z_ranges_vector.push_back("2. 6. 1.");

  z_values_vector.push_back("3.");
  z_values_vector.push_back("5.5");

  options_map["z-range"].value() = boost::any(z_ranges_vector);
  options_map["z-value"].value() = boost::any(z_values_vector);

  cf::RedshiftConfiguration rconf(options_map);
  auto z_list = rconf.getZList();

  BOOST_CHECK_EQUAL(z_list[1], 0.5);
  BOOST_CHECK_EQUAL(z_list[3], 1.5);
  BOOST_CHECK_EQUAL(z_list[4], 2);
  BOOST_CHECK_EQUAL(z_list[5], 3);
  BOOST_CHECK_EQUAL(z_list[8], 5.5);
  BOOST_CHECK_EQUAL(z_list[9], 6);

}

//-----------------------------------------------------------------------------
// Test the getZList function and wrong z-range parameter
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_z_range_function_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the wrong z-range parameter ");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> z_ranges_vector;

  z_ranges_vector.push_back("0. 2 0.5 1");

  options_map["z-range"].value() = boost::any(z_ranges_vector);

  cf::RedshiftConfiguration rconf(options_map);

 BOOST_CHECK_THROW(rconf.getZList(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the getZList function and 2 ranges and at the boundaries
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_z_value_function_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the wrong z-value parameter ");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> z_values_vector;

  z_values_vector.push_back("3. 4 6");

  options_map["z-value"].value() = boost::any(z_values_vector);

  cf::RedshiftConfiguration rconf(options_map);

  BOOST_CHECK_THROW(rconf.getZList(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the bad characters in the z-range option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_characters_zrange_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the wrong character(s) for z-range ");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> z_ranges_vector;
  std::vector<std::string> z2_ranges_vector;

  z_ranges_vector.push_back("z3. 4 6");

  options_map["z-range"].value() = boost::any(z_ranges_vector);

  cf::RedshiftConfiguration rconf(options_map);

  BOOST_CHECK_THROW(rconf.getZList(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the bad characters in the z-value option
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(wrong_characters_zvalue_test, RedshiftConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the wrong character(s) for z-value ");
  BOOST_TEST_MESSAGE(" ");

  std::vector<std::string> z_values_vector;

  z_values_vector.push_back("3.w");

  options_map["z-value"].value() = boost::any(z_values_vector);

  cf::RedshiftConfiguration rconf(options_map);

  BOOST_CHECK_THROW(rconf.getZList(), Elements::Exception);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
