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
#include "PhzConfiguration/PhotometricCorrectionConfiguration.h"

#include <boost/test/unit_test.hpp>
#include "CreateDirectory.h"


namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;

struct PhotometricCorrectionConfiguration_Fixture {

  Elements::TempDir temp_dir;
  std::string base_directory { temp_dir.path().native()+"/base_dir/" };
  std::string cor_filename { base_directory + "/file_correction.txt" };
  std::string cor_nofile { base_directory + "/NOFILE.txt" };

  std::vector<std::string> filter_qualified_name {"mer/filter1 1. 0.1", "mer/filter2 2. 0.2"};

  std::map<std::string, po::variable_value> options_map_nofile;
  std::map<std::string, po::variable_value> options_map_data;
  std::map<std::string, po::variable_value> options_map3;

  PhotometricCorrectionConfiguration_Fixture() {

	makeDirectory(base_directory);
    // Create files
    std::ofstream correction_file(cor_filename);
    // Fill up file
    correction_file << "#             Filter Correction\n";
    correction_file << "#             string     double\n";
    correction_file << "mer/filter_name1 1.1 \n";
    correction_file << "mer/filter_name2 2.2 \n";
    correction_file << "mer/filter_name3 3.3 \n";
    correction_file.close();

    // Fill up options
    options_map_nofile["photometric-correction-file"].value() = boost::any(cor_nofile);
    options_map_data["photometric-correction-file"].value()   = boost::any(cor_filename);
    options_map3["filter-name-mapping"].value() = boost::any(filter_qualified_name);

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
// Test the exception from the getPhotometricCorrectionMap function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getPhotometricCorrectionMap_exception_test, PhotometricCorrectionConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getPhotometricCorrectionMap exception(no correction file)");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzConfiguration::PhotometricCorrectionConfiguration pcc(options_map_nofile);
  BOOST_CHECK_THROW(pcc.getPhotometricCorrectionMap(), Elements::Exception);
}

//-----------------------------------------------------------------------------
// Test the exception from the getPhotometricCorrectionMap function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getPhotometricCorrectionMap_test, PhotometricCorrectionConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getPhotometricCorrectionMap function, reading a file with data");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzConfiguration::PhotometricCorrectionConfiguration pcc(options_map_data);
  auto correction_data_map = pcc.getPhotometricCorrectionMap();

  BOOST_CHECK_EQUAL(correction_data_map.size(), 3);
  auto itmap = correction_data_map.begin();
  BOOST_CHECK_EQUAL(itmap->first.datasetName(), std::string{"filter_name1"});
  BOOST_CHECK_EQUAL(itmap->second, 1.1);
}

//-----------------------------------------------------------------------------
// Test the filter_name_mapping option from the getPhotometricCorrectionMap function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getPhotometricCorrectionMap_filter_name_mapping_test, PhotometricCorrectionConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getPhotometricCorrectionMap with filter-name-mapping, all result must be set to 1");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzConfiguration::PhotometricCorrectionConfiguration pcc(options_map3);
  auto correction_data_map = pcc.getPhotometricCorrectionMap();

  BOOST_CHECK_EQUAL(correction_data_map.size(), 2);
  auto itmap = correction_data_map.begin();
  BOOST_CHECK_EQUAL(itmap->first.datasetName(), std::string{"filter2"});
  BOOST_CHECK_EQUAL(itmap->second, 1.);

}

BOOST_AUTO_TEST_SUITE_END ()



