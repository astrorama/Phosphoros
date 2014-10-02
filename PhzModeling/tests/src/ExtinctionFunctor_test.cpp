/**
 * @file tests/src/ExtinctionFunctor_test.cpp
 * @date Sep 26, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzModeling/ExtinctionFunctor.h"
#include "XYDataset/XYDataset.h"
#include "MathUtils/function/Function.h"
#include "MathUtils/interpolation/interpolation.h"

struct ExtinctionFunctor_Fixture {

  class DummyExtinctionFunction:public Euclid::MathUtils::Function{

    public:
    DummyExtinctionFunction()=default;

    virtual ~DummyExtinctionFunction() = default;

    double operator()(const double x) const {
      if (x<1100.){
        return 2.;
      }

      if (x<1300.){
        return 1.5;
      }
      return 1.;
    }

    std::unique_ptr<Function> clone() const{
       return std::unique_ptr<Function>{new DummyExtinctionFunction()};
    }
  };

  Euclid::XYDataset::XYDataset input_sed;
  Euclid::PhzModeling::ExtinctionFunctor functor;

  DummyExtinctionFunction extinction_function;

  ExtinctionFunctor_Fixture(): input_sed(makeInputVector()){



    }
private:
	std::vector<std::pair<double, double>> makeInputVector(){
		return std::vector<std::pair<double, double>>{
		  std::make_pair(10000.,0.004),
		  std::make_pair(12000.,0.002),
		  std::make_pair(14000.,0.001)};
	}
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ExtinctionFunctor_test)

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same length
//  that the input one
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(outputLength_test, ExtinctionFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the fixed point");
  BOOST_TEST_MESSAGE(" ");

  auto output_sed=functor(input_sed,extinction_function,0.);
  BOOST_CHECK_EQUAL(input_sed.size(), output_sed.size());
}

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same values if EVB=0
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(fixPoint_test, ExtinctionFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the fix point");
  BOOST_TEST_MESSAGE(" ");

  auto output_sed=functor(input_sed,extinction_function,0.);
  auto output_iterator = output_sed.begin();
  for (auto& input_pair : input_sed) {
    BOOST_CHECK(Elements::isEqual(input_pair.first,output_iterator->first));
    BOOST_CHECK(Elements::isEqual(input_pair.second,output_iterator->second));
    output_iterator++;
  }
}

//-----------------------------------------------------------------------------
// Check that the functor has the right dependence in EVB
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(evbDependency_test, ExtinctionFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the dependence in EVB");
  BOOST_TEST_MESSAGE(" ");
  auto evb_ref=0.7;

  auto output_sed=functor(input_sed,extinction_function,evb_ref);
  for(auto evb=1;evb!=4;++evb){

    auto output_sed_2=functor(input_sed,extinction_function,evb);
    auto output_iterator = output_sed.begin();
    auto output_iterator_2 = output_sed_2.begin();
    for (auto& input_pair : input_sed) {
      auto factor_for_output_1 = output_iterator->second/input_pair.second;
      auto factor_for_output_2 = output_iterator_2->second/input_pair.second;
      BOOST_CHECK(Elements::isEqual(
          std::pow(factor_for_output_1,evb/evb_ref),factor_for_output_2)
      );

      ++output_iterator;
      ++output_iterator_2;
    }
  }
}

//-----------------------------------------------------------------------------
// Check that the functor has the right dependence in the extinction function
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(extinctionFunctionDependency_test,
    ExtinctionFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the dependence in the extinction function");
  BOOST_TEST_MESSAGE(" ");

  auto output_sed=functor(input_sed,extinction_function,0.7);
  auto output_iterator = output_sed.begin();
  for (auto& input_pair : input_sed) {
    auto factor = output_iterator->second/input_pair.second;
    auto k_lambda=extinction_function(input_pair.first);

    BOOST_CHECK(Elements::isEqual(
        std::pow(10,-0.4*0.7),
        std::pow(factor,1/k_lambda)));
    ++output_iterator;
  }
}


BOOST_AUTO_TEST_SUITE_END ()
