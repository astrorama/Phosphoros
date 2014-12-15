/**
 * @file tests/src/ScaleFactorFunctor_test.cpp
 * @date dec 15, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>
#include <vector>
#include <cmath>

#include "ElementsKernel/Real.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"

using namespace std;
using namespace Euclid;
struct ScaleFactorFunctor_Fixture {

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ScaleFactorFunctor_test)

//-----------------------------------------------------------------------------
// Check the behavior for a single filter source
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(signlePoint_test, ScaleFactorFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the individual step computation");
  BOOST_TEST_MESSAGE(" ");

  vector<SourceCatalog::FluxErrorPair> source_List;
  vector<SourceCatalog::FluxErrorPair> model_List;
  source_List.push_back( { 3.0, 5.0 });
  model_List.push_back( { 2.0, 7.0 });

  PhzLikelihood::ScaleFactorFunctor functor { };
  // for a single value the result is source flux/model flux
  for (int i = 1; i < 12; ++i) {
    source_List[0].flux = i;
    double value = functor(source_List.begin(), source_List.end(),
        model_List.begin());
    BOOST_CHECK(
        Elements::isEqual(value, source_List[0].flux / model_List[0].flux));
  }

  // and this value do not change if the errors changes
  for (int i = 1; i < 12; ++i) {
    source_List[0].error = i;
    for (int j = 1; j < 12; ++j) {
      model_List[0].error = j;
      double value = functor(source_List.begin(), source_List.end(),
          model_List.begin());
      BOOST_CHECK(
          Elements::isEqual(value, source_List[0].flux / model_List[0].flux));
    }
  }
}

//-----------------------------------------------------------------------------
// check the behavior for a two filter sources
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(twoPoints_test, ScaleFactorFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the 2 step computation");
  BOOST_TEST_MESSAGE(" ");

  // model error can be 0 as it is not used in the computation.
  vector<SourceCatalog::FluxErrorPair> source_List;
  vector<SourceCatalog::FluxErrorPair> model_List;
  source_List.push_back( { 3.0, 1.0 });
  source_List.push_back( { 5.0, 1.0 });
  model_List.push_back( { 2.0, 0.0 });
  model_List.push_back( { 7.0, 0.0 });

  PhzLikelihood::ScaleFactorFunctor functor { };
  // for equal error value we have (fm1*fs1+fm2*fs2)/(fm1²+fm2²)
  for (int i = 1; i < 12; ++i) {
    source_List[0].flux = i;
    for (int j = 1; j < 12; ++j) {
      source_List[1].flux = j;
      for (int k = 1; k < 12; ++k) {
        model_List[0].flux = k;
        for (int l = 1; l < 12; ++l) {
          model_List[1].flux = l;
          double value = functor(source_List.begin(), source_List.end(),
              model_List.begin());
          double expected = (model_List[0].flux * source_List[0].flux
              + model_List[1].flux * source_List[1].flux)
              / (model_List[0].flux * model_List[0].flux
                  + model_List[1].flux * model_List[1].flux);
          BOOST_CHECK(Elements::isEqual(value, expected));
        }
      }
    }
  }

  // for this particular combination the result ought to be 1
  source_List[0].flux = 1;
  source_List[1].flux = 2;
  source_List[1].error = 2;
  model_List[0].flux = 1;
  model_List[1].flux = 2;
  double value = functor(source_List.begin(), source_List.end(),
      model_List.begin());
  BOOST_CHECK(Elements::isEqual(value, 1.));

  // limit case for extremely large error on the second source one recover the case with 1 source
  source_List[0].flux = 3;
  source_List[1].flux = 5;
  source_List[1].error = 1E20;
  model_List[0].flux = 2;
  model_List[1].flux = 7;
  value = functor(source_List.begin(), source_List.end(), model_List.begin());
  BOOST_CHECK(
      Elements::isEqual(value, source_List[0].flux / model_List[0].flux));
}

//-----------------------------------------------------------------------------
// check the generic behavior (N-filter source)
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(nPoints_test, ScaleFactorFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the 2 step computation");
  BOOST_TEST_MESSAGE(" ");

  vector<SourceCatalog::FluxErrorPair> source_List;
  vector<SourceCatalog::FluxErrorPair> model_List;
  for (int i = 0; i < 20; ++i) {
    source_List.push_back(
        { std::abs(std::sin(0.22 + i * 10.25)), 0.1 * std::abs(std::sin(0.11 + i * 11.27)) });
    model_List.push_back( { std::abs(std::sin(0.25 + i * 10.23)), 0.0 });
  }

  double num = 0;
  double den = 0;
  for (int i = 0; i < 20; ++i) {
    num += model_List[i].flux * source_List[i].flux
        / (source_List[i].error * source_List[i].error);
    den += model_List[i].flux * model_List[i].flux
        / (source_List[i].error * source_List[i].error);
  }
  double expected = num / den;

  PhzLikelihood::ScaleFactorFunctor functor { };
  double value = functor(source_List.begin(), source_List.end(), model_List.begin());
  BOOST_CHECK(Elements::isEqual(value, expected));
}

BOOST_AUTO_TEST_SUITE_END ()
