/*
 * PhotometryGridConfiguration_test.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: Nicolas Morisset
 */


#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"

#include "PhzConfiguration/PhotometryGridConfiguration.h"


struct PhotometryGridConfiguration_Fixture {


  PhotometryGridConfiguration_Fixture() {


  }
  ~PhotometryGridConfiguration_Fixture() {
  }


};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometryGridConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for filter-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, PhotometryGridConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = Euclid::PhzConfiguration::PhotometryGridConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow("photometry-grid-file", false);
  BOOST_CHECK(desc != nullptr);

}


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()


