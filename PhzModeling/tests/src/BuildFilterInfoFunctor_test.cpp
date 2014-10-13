/**
 * @file tests/src/BuildFilterInfoFunctor_test.cpp
 * @date Oct 10, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "XYDataset/XYDataset.h"
#include "ElementsKernel/PhysConstants.h"

struct BuildFilterInfoFunctor_Fixture {


  Euclid::XYDataset::XYDataset filter_sample;
  BuildFilterInfoFunctor_Fixture():filter_sample(makeInputVector()){
  }

   std::vector<std::pair<double, double>> makeInputVector(){
       return std::vector<std::pair<double, double>>{
         std::make_pair(1.,1.),
         std::make_pair(2.,4.),
         std::make_pair(3.,9.),
         std::make_pair(4.,16.),
         std::make_pair(5.,25.)
       };
   }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (BuildFilterInfoFunctor_test)

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same length that the input one
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(BuildFilterInfoFunctor_test, BuildFilterInfoFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the BuildFilterInfoFunctor");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzModeling::BuildFilterInfoFunctor functor{};

  auto filter_info = functor(filter_sample);

  // check the range
  BOOST_CHECK(Elements::isEqual(1.,filter_info.getRange().first));
  BOOST_CHECK(Elements::isEqual(5.,filter_info.getRange().second));

  // check the function
  for (auto pair:filter_sample){
    BOOST_CHECK(Elements::isEqual(pair.second,filter_info.getFilter()(pair.first)));
  }

  // check the normalization we have define the filter to have int(c) between 1 and 5
  auto expected_value= Elements::Units::c_light/ Elements::Units::angstrom*4.;
  BOOST_CHECK(Elements::isEqual(expected_value,filter_info.getNormalization()));

}

BOOST_AUTO_TEST_SUITE_END ()
