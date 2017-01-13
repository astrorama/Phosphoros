/*
 * SpectroscopicRedshiftCatalogConfiguration_test.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: FLorian dubath
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "SourceCatalog/SourceAttributes/SpectroscopicRedshift.h"
#include "PhzConfiguration/SpectroscopicRedshiftCatalogConfiguration.h"
#include "CreateDirectory.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;

struct SpectroscopicRedshiftCatalogConfiguration_Fixture {

  void makeFile(const std::string& file_name, const std::string& content) {
    std::ofstream test_file;
    test_file.open(file_name, std::fstream::out);
    test_file << content;
    test_file.close();
  }

  std::map<std::string, po::variable_value> options_map_min_working;
  std::map<std::string, po::variable_value> options_map_min_working_2;
  std::map<std::string, po::variable_value> options_map_2_z;
  std::map<std::string, po::variable_value> options_map_1_deltaZ_1;
  std::map<std::string, po::variable_value> options_map_1_deltaZ_2;
  std::map<std::string, po::variable_value> options_map_2_deltaZ;

  Elements::TempDir temp_dir;
  std::string base_directory { temp_dir.path().native() + "/euclid_test/" };
  std::string ascii_file_name { base_directory + "test_SpecZ_Catalog.txt" };
  SpectroscopicRedshiftCatalogConfiguration_Fixture() {
    makeDirectory(base_directory);
    makeFile(ascii_file_name, "# ID      Z        DeltaZ\n"
        "# long    double   double\n"
        "\n"
        "1         0.25     0.01\n"
        "2         1.01     0.02\n");

    options_map_min_working["input-catalog-file"].value() = ascii_file_name;
    options_map_min_working["spec-z-column-name"].value() = std::string("Z");

    options_map_min_working_2["input-catalog-file"].value() = ascii_file_name;
    options_map_min_working_2["spec-z-column-index"].value() = 2;

    options_map_2_z["input-catalog-file"].value() = ascii_file_name;
    options_map_2_z["spec-z-column-name"].value() = std::string("Z");
    options_map_2_z["spec-z-column-index"].value() = 2;

    options_map_1_deltaZ_1["input-catalog-file"].value() = ascii_file_name;
    options_map_1_deltaZ_1["spec-z-column-name"].value() = std::string("Z");
    options_map_1_deltaZ_1["spec-z-err-column-name"].value() = std::string(
        "DeltaZ");

    options_map_1_deltaZ_2["input-catalog-file"].value() = ascii_file_name;
    options_map_1_deltaZ_2["spec-z-column-name"].value() = std::string("Z");
    options_map_1_deltaZ_2["spec-z-err-column-index"].value() = 3;

    options_map_2_deltaZ["input-catalog-file"].value() = ascii_file_name;
    options_map_2_deltaZ["spec-z-column-name"].value() = std::string("Z");
    options_map_2_deltaZ["spec-z-err-column-name"].value() = std::string(
        "DeltaZ");
    options_map_2_deltaZ["spec-z-err-column-index"].value() = 3;
  }

  ~SpectroscopicRedshiftCatalogConfiguration_Fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SpectroscopicRedshiftCatalogConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for sed-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, SpectroscopicRedshiftCatalogConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc =
      Euclid::PhzConfiguration::SpectroscopicRedshiftCatalogConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc { };

  desc = option_desc.find_nothrow("spec-z-column-name", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("spec-z-column-index", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("spec-z-err-column-name", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("spec-z-err-column-index", false);
  BOOST_CHECK(desc != nullptr);
}

BOOST_FIXTURE_TEST_CASE(SpectroscopicRedshiftCatalogConfiguration_constructor_test, SpectroscopicRedshiftCatalogConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor");
  BOOST_TEST_MESSAGE(" ");

  // Nominal use-cases without error
  cf::SpectroscopicRedshiftCatalogConfiguration fconf(options_map_min_working);
  fconf.getCatalog();
  fconf = cf::SpectroscopicRedshiftCatalogConfiguration(options_map_min_working_2);
  auto catalog_without_err = fconf.getCatalog();
  //check the error
  auto photo_z =catalog_without_err.find(1)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();
  BOOST_CHECK(Elements::isEqual(0.,photo_z->getError()));

  // Redundant definition of the z column
  BOOST_CHECK_THROW(
      fconf = cf::SpectroscopicRedshiftCatalogConfiguration(options_map_2_z),
      Elements::Exception);

  // Nominal use-cases with error
  fconf = cf::SpectroscopicRedshiftCatalogConfiguration(options_map_1_deltaZ_1);
  fconf.getCatalog();
  fconf = cf::SpectroscopicRedshiftCatalogConfiguration(options_map_1_deltaZ_2);
  auto catalog_with_err = fconf.getCatalog();
  //check the error
  photo_z =catalog_with_err.find(1)->getAttribute<Euclid::SourceCatalog::SpectroscopicRedshift>();
  BOOST_CHECK(Elements::isEqual(0.01,photo_z->getError()));

  // Redundant definition of the error column
  BOOST_CHECK_THROW(
      fconf = cf::SpectroscopicRedshiftCatalogConfiguration(
          options_map_2_deltaZ), Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
