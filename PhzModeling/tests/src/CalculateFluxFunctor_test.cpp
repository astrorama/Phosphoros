/**
 * @file tests/src/CalculateFluxtFunctor_test.cpp
 * @date Oct 3, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzModeling/CalculateFluxFunctor.h"
#include "XYDataset/XYDataset.h"

struct CalculateFluxtFunctor_Fixture {

   Euclid::PhzModeling::CalculateFluxFunctor functor {Euclid::MathUtils::InterpolationType::LINEAR};

   CalculateFluxtFunctor_Fixture(){
    }
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CalculateFluxtFunctor_test)

//-----------------------------------------------------------------------------
// Check the normalization factor
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(normalization_test, CalculateFluxtFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the length of the returned model");
  BOOST_TEST_MESSAGE(" ");

  Euclid::XYDataset::XYDataset flat_input {std::vector<std::pair<double, double>>{
    std::make_pair(10000.,0.001),
    std::make_pair(12000.,0.001),
    std::make_pair(14000.,0.001),
    std::make_pair(16000.,0.001),
    std::make_pair(17000.,0.001)
  }};

  auto non_normalized_flux=functor(flat_input,1.0);

  for(auto normalization_compter=0;normalization_compter!=10;++normalization_compter){
    auto normalized_flux=functor(flat_input,normalization_compter*0.1+1.0);
    BOOST_CHECK(Elements::isEqual(non_normalized_flux/(normalization_compter*0.1+1.0),normalized_flux));
  }
}

//-----------------------------------------------------------------------------
// Check the integration
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(outputLength_test, CalculateFluxtFunctor_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the length of the returned model");
  BOOST_TEST_MESSAGE(" ");

  Euclid::XYDataset::XYDataset flat_null_input {std::vector<std::pair<double, double>>{
     std::make_pair(10000.,0.),
     std::make_pair(12000.,0.),
     std::make_pair(14000.,0.),
     std::make_pair(16000.,0.),
     std::make_pair(17000.,0.),
     std::make_pair(18000.,0.),
     std::make_pair(19000.,0.),
     std::make_pair(20000.,0.)
   }};

  Euclid::XYDataset::XYDataset flat_full_input {std::vector<std::pair<double, double>>{
    std::make_pair(10000.,0.001),
    std::make_pair(12000.,0.001),
    std::make_pair(14000.,0.001),
    std::make_pair(16000.,0.001),
    std::make_pair(17000.,0.001),
    std::make_pair(18000.,0.001),
    std::make_pair(19000.,0.001),
    std::make_pair(20000.,0.001)
  }};

  Euclid::XYDataset::XYDataset flat_half_input {std::vector<std::pair<double, double>>{
      std::make_pair(10000.,0.001),
      std::make_pair(12000.,0.001),
      std::make_pair(14000.,0.001),
      std::make_pair(15000.,0.001),
      std::make_pair(15000.0000001,0.000),
      std::make_pair(16000.,0.000),
      std::make_pair(17000.,0.000),
      std::make_pair(18000.,0.000),
      std::make_pair(19000.,0.000),
      std::make_pair(20000.,0.000)
    }};

  auto no_flux=functor(flat_null_input,1.0);
  BOOST_CHECK(Elements::isEqual(0.,no_flux));
  auto full_flux=functor(flat_full_input,1.0);
  BOOST_CHECK(Elements::isEqual(10.,full_flux));
  auto half_flux=functor(flat_half_input,1.0);
  BOOST_CHECK(Elements::isEqual(full_flux/2.0,half_flux));


}

BOOST_AUTO_TEST_SUITE_END ()
