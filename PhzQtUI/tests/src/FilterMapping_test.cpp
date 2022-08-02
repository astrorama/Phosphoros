/*
 * FilterMapping_test.cpp
 *
 *  Created on: Jun 23, 2015
 *      Author: fdubath
 */
#include "ElementsKernel/Real.h"  // isEqual
#include "PhzQtUI/FilterMapping.h"
#include <boost/test/unit_test.hpp>  // Gives access to the unit test framework.

using namespace Euclid::PhzQtUI;

struct FilterMapping_Fixture {
  std::string flux  = "flux_column";
  std::string error = "error_column";
  std::string file  = "transmission_file";
};

// Starts a test suite and name it.
BOOST_AUTO_TEST_SUITE(FilterMapping_test)

BOOST_FIXTURE_TEST_CASE(setter_test, FilterMapping_Fixture) {
  auto filterMapping = FilterMapping{};
  filterMapping.setFluxColumn(flux);
  filterMapping.setErrorColumn(error);
  filterMapping.setFilterFile(file);

  BOOST_CHECK_EQUAL(filterMapping.getFluxColumn(), flux);
  BOOST_CHECK_EQUAL(filterMapping.getErrorColumn(), error);
  BOOST_CHECK_EQUAL(filterMapping.getFilterFile(), file);
}

// Ends the test suite
BOOST_AUTO_TEST_SUITE_END()
