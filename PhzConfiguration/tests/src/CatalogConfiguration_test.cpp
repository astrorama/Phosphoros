/*
 * CatalogConfiguration_test.cpp
 *
 *  Created on: Jan 13, 2015
 *      Author: FLorian dubath
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Temporary.h"
#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/CatalogConfiguration.h"
#include "CreateDirectory.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;

struct CatalogConfiguration_fixture {

  void makeFile(const std::string& file_name, const std::string& content) {
    std::ofstream test_file;
    test_file.open(file_name, std::fstream::out);
    test_file << content;
    test_file.close();
  }

  std::map<std::string, po::variable_value> options_map_name;
  std::map<std::string, po::variable_value> options_map_id;
  std::map<std::string, po::variable_value> options_map_fits;

  std::map<std::string, po::variable_value> options_map_missing_file;
  std::map<std::string, po::variable_value> options_map_wrong_file;
  std::map<std::string, po::variable_value> options_map_2_columns;

  std::map<std::string, po::variable_value> options_map_missing_type;
  std::map<std::string, po::variable_value> options_map_missing_column;

  Elements::TempDir temp_dir;
  std::string base_directory { temp_dir.path().native() + "/euclid_test/" };
  std::string ascii_file_name { base_directory + "test_catalog.txt" };
  std::string ascii_strange_file_name { base_directory
      + "test_strange_catalog.toto" };
  std::string fits_file_name { base_directory + "test_catalog.fits" };

  CatalogConfiguration_fixture() {
    makeDirectory(base_directory);
    makeFile(ascii_file_name, "# ID     Info\n"
        "# long    string\n"
        "\n"
        "1       Source_1\n"
        "2       source_2\n");

    makeFile(ascii_strange_file_name, "# source_id     Info\n"
        "# long    string\n"
        "\n"
        "1       Source_1\n"
        "2       source_2\n");

    makeFile(fits_file_name, "SIMPLE  =");

    options_map_name["input-catalog-file"].value() = ascii_file_name;
    options_map_name["input-catalog-format"].value() = std::string("ASCII");
    options_map_name["source-id-column-name"].value() = std::string("ID");

    options_map_id["input-catalog-file"].value() = ascii_file_name;
    options_map_id["input-catalog-format"].value() = std::string("ASCII");
    options_map_id["source-id-column-index"].value() = 1;

    options_map_fits["input-catalog-file"].value() = ascii_file_name;
    options_map_fits["input-catalog-format"].value() = std::string("FITS");
    options_map_fits["source-id-column-name"].value() = std::string("ID");

    options_map_missing_file["input-catalog-format"].value() = std::string(
        "ASCII");
    options_map_missing_file["source-id-column-name"].value() = std::string(
        "ID");

    options_map_wrong_file["input-catalog-file"].value() = base_directory;
    options_map_wrong_file["input-catalog-format"].value() = std::string(
        "ASCII");
    options_map_wrong_file["source-id-column-name"].value() = std::string("ID");

    options_map_missing_type["input-catalog-file"].value() = ascii_file_name;
    options_map_missing_type["source-id-column-name"].value() = std::string(
        "ID");

    options_map_missing_column["input-catalog-file"].value() = ascii_file_name;
    options_map_missing_column["input-catalog-format"].value() = std::string(
        "ASCII");

    options_map_2_columns["input-catalog-file"].value() = ascii_file_name;
    options_map_2_columns["input-catalog-format"].value() = std::string(
        "ASCII");
    options_map_2_columns["source-id-column-name"].value() = std::string("ID");
    options_map_2_columns["source-id-column-index"].value() = 1;
  }

  ~CatalogConfiguration_fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CatalogConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, CatalogConfiguration_fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = cf::CatalogConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc { };

  desc = option_desc.find_nothrow("input-catalog-file", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("input-catalog-format", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("source-id-column-name", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("source-id-column-index", false);
  BOOST_CHECK(desc != nullptr);
}

//-----------------------------------------------------------------------------
// Test the constructor
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(CatalogConfiguration_ctr_test, CatalogConfiguration_fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the CatalogConfiguration constructor");
  BOOST_TEST_MESSAGE(" ");

  // Nominal use-cases
  cf::CatalogConfiguration fconf(options_map_name);
  fconf.getCatalog();

  fconf = cf::CatalogConfiguration(options_map_id);
  fconf.getCatalog();

  //Default column name
  fconf = cf::CatalogConfiguration(options_map_missing_column);
  auto catalog = fconf.getCatalog();
  BOOST_CHECK(catalog.find(1) != nullptr);
  options_map_missing_column["input-catalog-file"].value() =
      ascii_strange_file_name;
  fconf = cf::CatalogConfiguration(options_map_missing_column);
  BOOST_CHECK_THROW(fconf.getCatalog(), std::exception);

  // Other file type
  fconf = cf::CatalogConfiguration(options_map_fits);

  // Auto-detect type
  fconf = cf::CatalogConfiguration(options_map_missing_type);
  //   ASCII
  options_map_missing_type["input-catalog-file"].value() =
      ascii_strange_file_name;
  fconf = cf::CatalogConfiguration(options_map_missing_type);
  //   FITS
  options_map_missing_type["input-catalog-file"].value() = fits_file_name;
  fconf = cf::CatalogConfiguration(options_map_missing_type);
  // Incomplete ?

  // Use-cases causing error throw
  BOOST_CHECK_THROW(fconf = cf::CatalogConfiguration(options_map_missing_file),
      Elements::Exception);
  BOOST_CHECK_THROW(fconf = cf::CatalogConfiguration(options_map_wrong_file),
      Elements::Exception);
  BOOST_CHECK_THROW(fconf = cf::CatalogConfiguration(options_map_2_columns),
      Elements::Exception);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
