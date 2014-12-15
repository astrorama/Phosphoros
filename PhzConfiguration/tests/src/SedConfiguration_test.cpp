/*
 * SedConfiguration_test.cpp
 *
 *  Created on: Oct 8, 2014
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
#include "PhzConfiguration/SedConfiguration.h"

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>


namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;

// Create a directory on disk
void makeDirectory(const std::string& name) {
  boost::filesystem::path d {name};
  boost::filesystem::create_directories(d);
}

// Remove a directory on disk
void removeDir(const std::string& base_dir) {
  boost::filesystem::path bd {base_dir};
  boost::filesystem::remove_all(bd);
}

struct SedConfiguration_Fixture {

  std::string group { "sed/MER" };
  // Do not forget the "/" at the end of the base directory
  Elements::TempDir temp_dir;
  std::string base_directory { temp_dir.path().native()+"/euclid/" };
  std::string mer_directory    = base_directory + "sed/MER";
  std::string cosmos_directory = base_directory + "sed/COSMOS";

  std::map<std::string, po::variable_value> options_map;
  std::map<std::string, po::variable_value> empty_map;
  std::vector<std::string> group_vector;
  std::vector<std::string> exclude_vector;
  std::vector<std::string> add_vector;

  SedConfiguration_Fixture() {

    group_vector.push_back(group);

    makeDirectory(base_directory);
    makeDirectory(mer_directory);
    makeDirectory(cosmos_directory);
    // Create files
    std::ofstream file1_mer(mer_directory + "/file1.txt");
    std::ofstream file2_mer(mer_directory + "/file2.txt");
    std::ofstream file3_cos(cosmos_directory + "/file3.txt");
    // Fill up file
    file1_mer << "\n";
    file1_mer << "# Dataset_name_for_file1\n";
    file1_mer << "1234. 569.6\n";
    file1_mer.close();
    // Fill up 2nd file
    file2_mer << "\n";
    file2_mer << "111.1 111.1\n";
    file2_mer << "222.2 222.2\n";
    file2_mer.close();
    // Fill up 3rd file
    file3_cos << "\n";
    file3_cos << "# Dataset_name_for_file3\n";
    file3_cos << "333.3 333.3\n";
    file3_cos.close();

    // Fill up options
    options_map["sed-root-path"].value() = boost::any(base_directory);
    options_map["sed-group"].value() = boost::any(group_vector);

  }
  ~SedConfiguration_Fixture() {
    //removeDir(base_directory);
  }


};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (SedConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for sed-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = Euclid::PhzConfiguration::SedConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow("sed-root-path", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("sed-group", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("sed-name", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("sed-exclude", false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------
// Test the getSedDatasetProvider function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedDatasetProvider_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedDatasetProvider function");
  BOOST_TEST_MESSAGE(" ");

  cf::SedConfiguration sconf(options_map);
  auto fdp     = sconf.getSedDatasetProvider();
  auto vec_fdp = fdp->listContents(group);

  BOOST_CHECK_EQUAL(vec_fdp[0].datasetName(), "Dataset_name_for_file1");
  BOOST_CHECK_EQUAL(vec_fdp[1].datasetName(), "file2");

}

//-----------------------------------------------------------------------------
// Test the getSedDatasetProvider function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedDatasetProvider_exception_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the exception of getSedDatasetProvider function");
  BOOST_TEST_MESSAGE(" ");

  cf::SedConfiguration sconf(empty_map);

  BOOST_CHECK_THROW(sconf.getSedDatasetProvider(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the getSedList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedList function");
  BOOST_TEST_MESSAGE(" ");

  cf::SedConfiguration sconf(options_map);
  auto list = sconf.getSedList();

  BOOST_CHECK_EQUAL(list[0].datasetName(), "Dataset_name_for_file1");
  BOOST_CHECK_EQUAL(list[1].datasetName(), "file2");

}

//-----------------------------------------------------------------------------
// Test the getSedList function excluding a sed
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_exclude_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedList function with exclude option");
  BOOST_TEST_MESSAGE(" ");

  // Sed to be excluded and a non existant sed
  exclude_vector.push_back("sed/MER/file2");
  exclude_vector.push_back("sed/MER/FILE_DOES_NOT_EXIST");
  options_map["sed-exclude"].value() = boost::any(exclude_vector);

  cf::SedConfiguration sconf(options_map);
  auto list = sconf.getSedList();

  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list[0].qualifiedName(), "sed/MER/Dataset_name_for_file1");

}

//-----------------------------------------------------------------------------
// Test the getSedList function adding a sed
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_add_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedList function with adding a sed");
  BOOST_TEST_MESSAGE(" ");

  // Sed to be added
  add_vector.push_back("sed/COSMOS/Dataset_name_for_file3");
  options_map["sed-name"].value() = boost::any(add_vector);

  cf::SedConfiguration sconf(options_map);
  auto list = sconf.getSedList();

  BOOST_CHECK_EQUAL(list.size(), 3);
  BOOST_CHECK_EQUAL(list[0].qualifiedName(), "sed/COSMOS/Dataset_name_for_file3");

}

//-----------------------------------------------------------------------------
// Test the getSedList function adding twice a sed
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getSedList_add_twice_function_test, SedConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getSedList function with adding twice a sed");
  BOOST_TEST_MESSAGE(" ");

  cf::SedConfiguration sconf(options_map);
  auto list = sconf.getSedList();

  // Add twice the same sed
  add_vector.push_back("sed/MER/Dataset_name_for_file1");
  options_map["sed-name"].value() = boost::any(add_vector);

  cf::SedConfiguration sconf2(options_map);
  auto list2 = sconf2.getSedList();

  BOOST_CHECK_EQUAL(list.size(), list2.size());
  BOOST_CHECK_EQUAL(list[0].qualifiedName(), "sed/MER/Dataset_name_for_file1");

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
