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
		return std::vector<std::pair<double, double>>{std::make_pair(10000.,0.004),std::make_pair(12000.,0.002),std::make_pair(14000.,0.001)};
	}
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ExtinctionFunctor_test)

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same length that the input one
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(outputLength_test, ExtinctionFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the length of the returned SED");
  BOOST_TEST_MESSAGE(" ");

  auto output_sed=functor(input_sed,extinction_function,0.);
  BOOST_CHECK_EQUAL(input_sed.size(), output_sed.size());
}

// TODO : more to come


BOOST_AUTO_TEST_SUITE_END ()
