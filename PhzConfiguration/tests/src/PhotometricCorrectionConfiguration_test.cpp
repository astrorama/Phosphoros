/*
 * PhotometricCorrectionConfiguration_test.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/PhotometricCorrectionConfiguration.h"

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>


namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;


struct PhotometricCorrectionConfiguration_Fixture {


  PhotometricCorrectionConfiguration_Fixture() {

	std::string basedir{"/tmp/base_dir"};

    // Create files
    std::ofstream file1(basedir + "/file_correction.txt");
    // Fill up file

    // Fill up options
    options_map["photometric-correction-file"].value() = boost::any(base_directory);

  }
  ~PhotometricCorrectionConfiguration_Fixture() {
  }


};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometricCorrectionConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for filter-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, PhotometricCorrectionConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = Euclid::PhzConfiguration::PhotometricCorrectionConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow("photometric-correction-file", false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



