/**
 * @file tests/src/ChiSquareFunctor_test.cpp
 * @date dec 15, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>
#include <vector>

#include "ElementsKernel/Real.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzLikelihood/ChiSquareFunctor.h"

using namespace std;
using namespace Euclid;
struct ChiSquareFunctor_Fixture {

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ChiSquareFunctor_test)

//-----------------------------------------------------------------------------
// Check the behavior for a single filter source
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(signlePoint_test, ChiSquareFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the individual step computation");
  BOOST_TEST_MESSAGE(" ");

  vector<SourceCatalog::FluxErrorPair> source_List;
  vector<SourceCatalog::FluxErrorPair> model_List;
  source_List.push_back( { 3.0, 5.0 });
  model_List.push_back( { 2.0, 0.0 });
  double scale = 11.0;

  PhzLikelihood::ChiSquareFunctor functor { };

  for (int i = 1; i < 12; ++i) {
    source_List[0].flux = i;
    for (int j = 1; j < 12; ++j) {
      source_List[0].error = i;
      for (int k = 1; k < 12; ++k) {
        model_List[0].flux = k;
        for (int l = 1; l < 12; ++l) {
          scale = l;
          double expected_chi2 = pow(
              scale * model_List[0].flux -source_List[0].flux, 2.0)
              / pow(source_List[0].error, 2.0);

          double value = functor(source_List.begin(), source_List.end(),
              model_List.begin(), scale);
          BOOST_CHECK(Elements::isEqual(value, exp(-expected_chi2 / 2.0)));
        }
      }
    }
  }

}

//-----------------------------------------------------------------------------
// Check the behavior for a multiple filter source
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(multiplePoint_test, ChiSquareFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the individual step computation");
  BOOST_TEST_MESSAGE(" ");

  vector<SourceCatalog::FluxErrorPair> source_List;
  vector<SourceCatalog::FluxErrorPair> model_List;
  source_List.push_back( { 5.0, 1.0 });
  model_List.push_back( { 3.0, 0.0 });
  // chi^2 for this item = (2*3-5)^2/1^2=1
  double scale = 2.0;

  PhzLikelihood::ChiSquareFunctor functor { };

  double prev_Value=functor(source_List.begin(), source_List.end(),
      model_List.begin(), scale);

  BOOST_CHECK(Elements::isEqual(prev_Value, exp(- 0.5)));

  source_List.push_back( { 7.0, 1.0 });
  model_List.push_back( { 3.0, 0.0 });
  // chi^2 for this item = (2*3-7)^2/1^2=1

  double value=functor(source_List.begin(), source_List.end(),
        model_List.begin(), scale);

  BOOST_CHECK(Elements::isEqual(value, prev_Value*exp(- 0.5)));
  prev_Value=value;

  source_List.push_back( { 1.0, 2.0 });
  model_List.push_back( { 2.0, 0.0 });
  value=functor(source_List.begin(), source_List.end(),
         model_List.begin(), scale);
  // chi^2 for this item = (2*2-1)^2/2^2=9/4
  BOOST_CHECK(Elements::isEqual(value, prev_Value*exp(- 9.0/8.0)));
  prev_Value=value;

  source_List.push_back( { 5.0, 7.0 });
  model_List.push_back( { 11.0, 0.0 });
  value=functor(source_List.begin(), source_List.end(),
         model_List.begin(), scale);
  // chi^2 for this item = (2*11-5)^2/7^2=289/49
  BOOST_CHECK(Elements::isEqual(value, prev_Value*exp(- 289.0/98.0)));
  prev_Value=value;

}

BOOST_AUTO_TEST_SUITE_END ()
