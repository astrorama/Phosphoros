/**
 * @file tests/src/ApplyFilterFunctor_test.cpp
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "XYDataset/XYDataset.h"
#include "MathUtils/function/Function.h"

struct ApplyFilterFunctor_Fixture {

  class DummyFilterFunction:public Euclid::MathUtils::Function{
   public:
    DummyFilterFunction()=default;

    virtual ~DummyFilterFunction() = default;

    double operator()(const double x) const {
      if (x<11000.){
        return 1.;
      }

      if (x<15000.){
        return (x-11000.)/4000.;
      }
      return 1.;
    }

    std::unique_ptr<Function> clone() const{
       return std::unique_ptr<Function>{new DummyFilterFunction()};
    }
  };


   Euclid::XYDataset::XYDataset input_model;
   Euclid::PhzModeling::ApplyFilterFunctor functor;
   std::pair<double,double> range= std::make_pair(11000.,15000.);
   DummyFilterFunction filter;
   ApplyFilterFunctor_Fixture(): input_model(makeInputVector()){
    }

  std::vector<std::pair<double, double>> makeInputVector(){
    return std::vector<std::pair<double, double>>{
      std::make_pair(9000.,0.005),
      std::make_pair(10000.,0.004),
      std::make_pair(12000.,0.003),
      std::make_pair(14000.,0.002),
      std::make_pair(16000.,0.001),
      std::make_pair(17000.,0.001)
    };
  }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ApplyFilterFunctor_test)

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same length that the input one
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(outputLength_test, ApplyFilterFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the length of the returned model");
  BOOST_TEST_MESSAGE(" ");

  auto output_model=functor(input_model,range,filter);
  BOOST_CHECK_EQUAL(4, output_model.size()); //2 in the range + 1 before + 1 after
}

//-----------------------------------------------------------------------------
// Check that the functor returns the expected values
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(values_test, ApplyFilterFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the computed values");
  BOOST_TEST_MESSAGE(" ");

  auto output_model=functor(input_model,range,filter);
  auto output_iterator=output_model.begin();
  auto input_iterator=input_model.begin();
  // first record is not in the output skip it in the input
  ++input_iterator;

  // first values is out of the range must be 0 regardless of the filter value  (which is 1)
  BOOST_CHECK(Elements::isEqual(input_iterator->first,output_iterator->first));
  BOOST_CHECK(Elements::isEqual(0.,output_iterator->second));
  //std::cout<<output_iterator->second<<';'<<input_iterator->first;

  ++output_iterator;
  ++input_iterator;
  // second value must be multiplied by 1/4
  BOOST_CHECK(Elements::isEqual(input_iterator->first,output_iterator->first));
  BOOST_CHECK(Elements::isEqual(input_iterator->second/4.,output_iterator->second));

  //std::cout<<output_iterator->second<<';'<<input_iterator->first;
  ++output_iterator;
  ++input_iterator;
  // third value must be multiplied by 3/4
  BOOST_CHECK(Elements::isEqual(input_iterator->first,output_iterator->first));
  BOOST_CHECK(Elements::isEqual(input_iterator->second*3./4.,output_iterator->second));

  //std::cout<<output_iterator->second<<';'<<input_iterator->first;
  ++output_iterator;
  ++input_iterator;
  // last values is out of the range must be 0 regardless of the filter value (which is 1)
  BOOST_CHECK(Elements::isEqual(input_iterator->first,output_iterator->first));
  BOOST_CHECK(Elements::isEqual(0.,output_iterator->second));
  //std::cout<<output_iterator->second<<';'<<input_iterator->first;
}


//-----------------------------------------------------------------------------
// Check that the functor behave correctly when the range meet the border of the model
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(border_test, ApplyFilterFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the computed values");
  BOOST_TEST_MESSAGE(" ");

  auto output_model=functor(input_model,std::make_pair(8000.,18000.),filter);
  BOOST_CHECK_EQUAL(input_model.size(), output_model.size());

  auto output_iterator = output_model.begin();
  for (auto& input_pair : input_model) {
    BOOST_CHECK(Elements::isEqual(input_pair.first,output_iterator->first));
    BOOST_CHECK(Elements::isEqual(input_pair.second*filter(input_pair.first),output_iterator->second));

       output_iterator++;
     }
}

BOOST_AUTO_TEST_SUITE_END ()
