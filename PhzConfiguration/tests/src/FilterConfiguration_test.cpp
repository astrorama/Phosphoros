/*
 * FilterConfiguration_test.cpp
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
#include "PhzConfiguration/FilterConfiguration.h"

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

struct FilterConfiguration_Fixture {

  std::string group { "filter/MER" };
  // Do not forget the "/" at the end of the base directory
  Elements::TempDir temp_dir;
  std::string base_directory { temp_dir.path().native()+"/euclid/" };
  std::string mer_directory    = base_directory + "filter/MER";
  std::string cosmos_directory = base_directory + "filter/COSMOS";

  std::map<std::string, po::variable_value> options_map;
  std::map<std::string, po::variable_value> empty_map;
  std::vector<std::string> group_vector;
  std::vector<std::string> exclude_vector;
  std::vector<std::string> add_vector;

  FilterConfiguration_Fixture() {

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
    options_map["filter-root-path"].value() = boost::any(base_directory);
    options_map["filter-group"].value() = boost::any(group_vector);

  }
  ~FilterConfiguration_Fixture() {
    //removeDir(base_directory);
  }


};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FilterConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for filter-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, FilterConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = Euclid::PhzConfiguration::FilterConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow("filter-root-path", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("filter-group", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("filter-name", false);
  BOOST_CHECK(desc != nullptr);
  desc = option_desc.find_nothrow("filter-exclude", false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------
// Test the getFilterDatasetProvider function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterDatasetProvider_function_test, FilterConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getFilterDatasetProvider function");
  BOOST_TEST_MESSAGE(" ");

  cf::FilterConfiguration fconf(options_map);
  auto fdp     = fconf.getFilterDatasetProvider();
  BOOST_CHECK(fdp != nullptr);
  auto dataset = fdp->getDataset({"filter/MER/file2"});
  BOOST_CHECK_EQUAL(2, dataset->size());
  auto iter = dataset->begin();
  BOOST_CHECK_EQUAL(111.1, iter->first);
  BOOST_CHECK_EQUAL(111.1, iter->second);
  ++iter;
  BOOST_CHECK_EQUAL(222.2, iter->first);
  BOOST_CHECK_EQUAL(222.2, iter->second);

}

//-----------------------------------------------------------------------------
// Test the getFilterDatasetProvider function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterDatasetProvider_exception_function_test, FilterConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the exception of getFilterDatasetProvider function");
  BOOST_TEST_MESSAGE(" ");

  cf::FilterConfiguration fconf(empty_map);

  BOOST_CHECK_THROW(fconf.getFilterDatasetProvider(), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the getFilterList function
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterList_function_test, FilterConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getFilterList function");
  BOOST_TEST_MESSAGE(" ");

  cf::FilterConfiguration fconf(options_map);
  auto list = fconf.getFilterList();

  BOOST_CHECK_EQUAL(list[0].datasetName(), "Dataset_name_for_file1");
  BOOST_CHECK_EQUAL(list[1].datasetName(), "file2");

}

//-----------------------------------------------------------------------------
// Test the getFilterList function excluding a filter
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterList_exclude_function_test, FilterConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getFilterList function with exclude option");
  BOOST_TEST_MESSAGE(" ");

  // Filter to be excluded and a non existant filter
  exclude_vector.push_back("filter/MER/file2");
  exclude_vector.push_back("filter/MER/FILE_DOES_NOT_EXIST");
  options_map["filter-exclude"].value() = boost::any(exclude_vector);

  cf::FilterConfiguration fconf(options_map);
  auto list = fconf.getFilterList();

  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list[0].qualifiedName(), "filter/MER/Dataset_name_for_file1");

}

//-----------------------------------------------------------------------------
// Test the getFilterList function adding a filter
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterList_add_function_test, FilterConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getFilterList function with adding a filter");
  BOOST_TEST_MESSAGE(" ");

  // Filter to be added
  add_vector.push_back("filter/COSMOS/Dataset_name_for_file3");
  options_map["filter-name"].value() = boost::any(add_vector);

  cf::FilterConfiguration fconf(options_map);
  auto list = fconf.getFilterList();

  BOOST_CHECK_EQUAL(list.size(), 3);
  BOOST_CHECK_EQUAL(list[0].qualifiedName(), "filter/COSMOS/Dataset_name_for_file3");

}

//-----------------------------------------------------------------------------
// Test the getFilterList function adding twice a filter
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getFilterList_add_twice_function_test, FilterConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getFilterList function with adding twice a filter");
  BOOST_TEST_MESSAGE(" ");

  cf::FilterConfiguration fconf(options_map);
  auto list = fconf.getFilterList();

  // Add twice the same filter
  add_vector.push_back("filter/MER/Dataset_name_for_file1");
  options_map["filter-name"].value() = boost::any(add_vector);

  cf::FilterConfiguration fconf2(options_map);
  auto list2 = fconf2.getFilterList();

  BOOST_CHECK_EQUAL(list.size(), list2.size());
  BOOST_CHECK_EQUAL(list[0].qualifiedName(), "filter/MER/Dataset_name_for_file1");

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
