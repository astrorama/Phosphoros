/**
 * @file tests/src/serialization/QualifiedName_test.cpp
 * @date Sep 24, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/serialization/QualifiedName.h"

struct PhzQualifiedName_Fixture {
  Euclid::XYDataset::QualifiedName qualified_name {"part1/part2/part3/name"};

  PhzQualifiedName_Fixture(){
  }
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (QualifiedNameSerialization_test)

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(serialization_test, PhzQualifiedName_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the serialization of the QualifiedName");
  BOOST_TEST_MESSAGE(" ");

  std::stringstream stream;

  boost::archive::text_oarchive oa(stream);
  Euclid::XYDataset::QualifiedName *original_ptr=&qualified_name;
  oa << original_ptr;

  boost::archive::text_iarchive ia(stream);

  Euclid::XYDataset::QualifiedName *out_qualified_name;
  ia >> out_qualified_name;
  std::unique_ptr<Euclid::XYDataset::QualifiedName> ptr{out_qualified_name};

  BOOST_CHECK_EQUAL(qualified_name.datasetName(), out_qualified_name->datasetName());
  BOOST_CHECK_EQUAL(qualified_name.groups().size(), out_qualified_name->groups().size());
  auto actual=out_qualified_name->groups().cbegin();
  for(auto expected :qualified_name.groups()){
    BOOST_CHECK_EQUAL(expected,*actual);
    ++actual;
  }
}


BOOST_AUTO_TEST_SUITE_END ()
