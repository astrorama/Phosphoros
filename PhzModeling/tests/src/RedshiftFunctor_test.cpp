/** 
 * @file tests/src/RedshiftFunctor_test.cpp
 * @date Sep 16, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "XYDataset/XYDataset.h"

struct RedshiftFunctor_Fixture {
	 Euclid::XYDataset::XYDataset input_sed;
	 std::vector<double> redshift_values={0.1,0.3,0.5,0.7,0.9,1.2,1.5,1.7,1.9,2.0};
	 Euclid::PhzModeling::RedshiftFunctor functor;

	RedshiftFunctor_Fixture(): input_sed(makeInputVector()){
    }

	std::vector<std::pair<double, double>> makeInputVector(){
		return std::vector<std::pair<double, double>>{std::make_pair(10000.,0.004),std::make_pair(12000.,0.002),std::make_pair(14000.,0.001)};
	}
};
  
  
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (RedshiftFunctor_test)

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same length that the input one
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(outputLength_test, RedshiftFunctor_Fixture) {
	 BOOST_TEST_MESSAGE(" ");
	 BOOST_TEST_MESSAGE("--> Testing the length of the returned SED");
	 BOOST_TEST_MESSAGE(" ");

	 auto output_sed=functor(input_sed,0.);
	 BOOST_CHECK_EQUAL(input_sed.size(), output_sed.size());
}

//-----------------------------------------------------------------------------
// Check that the functor returns the same values XYDataSet for a redshift of 0
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(fixedPoint_test, RedshiftFunctor_Fixture) {
	 BOOST_TEST_MESSAGE(" ");
	 BOOST_TEST_MESSAGE("--> Testing the fixed point (z=0)");
	 BOOST_TEST_MESSAGE(" ");

	 auto output_sed=functor(input_sed,0.);

	 auto output_iterator = output_sed.begin();
	 for (auto& input_pair : input_sed) {

		 BOOST_CHECK(Elements::isEqual(input_pair.first,output_iterator->first));

		 BOOST_CHECK(Elements::isEqual(input_pair.second,output_iterator->second));

		 output_iterator++;
	 }
}

//-----------------------------------------------------------------------------
// Check that the functor returns the expected values for redshift 1 (\lambda->2\lambda, flux->flux/4)
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(redshift1_test, RedshiftFunctor_Fixture) {
	 BOOST_TEST_MESSAGE(" ");
	 BOOST_TEST_MESSAGE("--> Testing the values for z=1");
	 BOOST_TEST_MESSAGE(" ");

	 auto output_sed=functor(input_sed,1.);

	 auto output_iterator = output_sed.begin();
	 for (auto& input_pair : input_sed) {

		 BOOST_CHECK(Elements::isEqual(input_pair.first*2,output_iterator->first));

		 BOOST_CHECK(Elements::isEqual(input_pair.second/4,output_iterator->second));

		 output_iterator++;
	 }
}

//-----------------------------------------------------------------------------
// Check that the functor returns the expected values for higher redshift (\lambda->(z+1)\lambda, flux->flux/(z+1)²)
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(redshiftsValues_test, RedshiftFunctor_Fixture) {
	 BOOST_TEST_MESSAGE(" ");
	 BOOST_TEST_MESSAGE("--> Testing the values for z=2");
	 BOOST_TEST_MESSAGE(" ");


	 for (auto z :redshift_values){
		auto output_sed=functor(input_sed,z);
		auto output_iterator = output_sed.begin();
		for (auto& input_pair : input_sed) {

			BOOST_CHECK(Elements::isEqual(input_pair.first*(z+1),output_iterator->first));

			BOOST_CHECK(Elements::isEqual(input_pair.second/((z+1)*(z+1)),output_iterator->second));

			output_iterator++;
		}
	 }
}



BOOST_AUTO_TEST_SUITE_END ()
