/*
 * CreatePhotometryGridConfiguration_test.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"
#include "PhzConfiguration/CreatePhotometryGridConfiguration.h"

namespace po = boost::program_options;
namespace cf = Euclid::PhzConfiguration;


struct CreatePhotometryGridConfiguration_Fixture {

  std::vector<double> zs{0.0,0.1};
  std::vector<double> ebvs{0.0,0.001};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve1"}};

  std::shared_ptr<std::vector<std::string>> filter_1 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2"});
  std::shared_ptr<std::vector<std::string>> filter_2 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2","filter3"});
  std::shared_ptr<std::vector<std::string>> filter_3 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter3"});
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_1{{1.1,2.1},{3.1,4.1}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_2{{1.2,2.2},{3.2,4.2}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_3{{1.3,2.3},{3.3,4.3}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_4{{1.4,2.4},{3.4,4.4}};

  Euclid::SourceCatalog::Photometry photometry_1{filter_1,values_1};
  Euclid::SourceCatalog::Photometry photometry_2{filter_1,values_2};
  Euclid::SourceCatalog::Photometry photometry_3{filter_1,values_3};
  Euclid::SourceCatalog::Photometry photometry_4{filter_1,values_4};

  std::string path_filename = "/tmp/binary_file.dat";
  std::map<std::string, po::variable_value> options_map;

  CreatePhotometryGridConfiguration_Fixture() {

  }
  ~CreatePhotometryGridConfiguration_Fixture() {
  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CreatePhotometryGridConfiguration_test)

//-----------------------------------------------------------------------------
// Test the getProgramOptions function for filter-root-path
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getProgramOptions_function_test, CreatePhotometryGridConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getProgramOptions function");
  BOOST_TEST_MESSAGE(" ");

  auto option_desc = cf::CreatePhotometryGridConfiguration::getProgramOptions();
  const boost::program_options::option_description* desc{};

  desc = option_desc.find_nothrow("output-photometry-grid", false);
  BOOST_CHECK(desc != nullptr);

}

//-----------------------------------------------------------------------------
// Test the contructor
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(constructor_test, CreatePhotometryGridConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor");
  BOOST_TEST_MESSAGE(" ");

  // Location not allowed
  path_filename ="/etc/zzz_test_writing_binary_file.dat";
  options_map["output-photometry-grid"].value() = boost::any(path_filename);

  BOOST_CHECK_THROW(cf::CreatePhotometryGridConfiguration cpgc(options_map), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Test the getOutputFunction
//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_CASE(getOutputFunction_test, CreatePhotometryGridConfiguration_Fixture) {

  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the getOutputFunction function");
  BOOST_TEST_MESSAGE(" ");

  // Create a binary file
  path_filename ="/tmp/test_writing_binary_file.dat";
  options_map["output-photometry-grid"].value() = boost::any(path_filename);

  cf::CreatePhotometryGridConfiguration cpgc(options_map);
  auto output_func = cpgc.getOutputFunction();

  auto axes=Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid{axes};
  original_grid(0,0,0,0)=photometry_1;
  original_grid(1,0,0,0)=photometry_2;
  original_grid(0,1,0,0)=photometry_3;
  original_grid(1,1,0,0)=photometry_4;

  output_func(original_grid);

  // Read the binary file created
  std::ifstream ifs;
  ifs.open (path_filename, std::ios::binary);
  boost::archive::binary_iarchive ia(ifs);
  Euclid::PhzDataModel::PhotometryGrid *retrieved_grid_ptr;
  ia >> retrieved_grid_ptr;

  BOOST_CHECK_EQUAL(original_grid.size(),retrieved_grid_ptr->size());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()



