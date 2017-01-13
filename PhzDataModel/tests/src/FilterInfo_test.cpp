/**
 * @file tests/src/FilterInfo_test.cpp
 * @date Oct 03, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/FilterInfo.h"

struct FilterInfo_Fixture {


    class squareFunction:public Euclid::MathUtils::Function{
     public:
      squareFunction()=default;

      virtual ~squareFunction() = default;

      double operator()(const double x) const {
        return x*x;
      }

      std::unique_ptr<Function> clone() const{
         return std::unique_ptr<Function>{new squareFunction()};
      }
    };

  FilterInfo_Fixture(){




  }
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FilterInfo_test)

//-----------------------------------------------------------------------------
// Check the function and range getter
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(test_test, FilterInfo_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> test range and function getter ");
  BOOST_TEST_MESSAGE(" ");

  auto const_function = squareFunction();
  auto info = Euclid::PhzDataModel::FilterInfo(std::make_pair(10000.,20000.),const_function,20.);

  BOOST_CHECK(Elements::isEqual(10000.,info.getRange().first));
  BOOST_CHECK(Elements::isEqual(20000.,info.getRange().second));
  BOOST_CHECK(Elements::isEqual(20.,info.getNormalization()));


  for(auto x=10000; x!=20000; ++x){
    BOOST_CHECK(Elements::isEqual(squareFunction()(x),info.getFilter()(x)));
  }


}

BOOST_AUTO_TEST_SUITE_END ()
