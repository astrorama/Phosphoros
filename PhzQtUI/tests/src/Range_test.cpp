/*
 * Range_test.cpp
 *
 *  Created on: Jun 23, 2015
 *      Author: fdubath
 */
#include "ElementsKernel/Real.h"  // isEqual
#include "PhzQtUI/Range.h"
#include <boost/test/unit_test.hpp>  // Gives access to the unit test framework.

using namespace Euclid::PhzQtUI;

struct Range_Fixture {
  double ref_min  = 1.2345;
  double ref_max  = 5.12345;
  double ref_step = 0.512345;
};

// Starts a test suite and name it.
BOOST_AUTO_TEST_SUITE(Range_test)

BOOST_FIXTURE_TEST_CASE(cst_test, Range_Fixture) {
  auto range = Range{ref_min, ref_max, ref_step};
  BOOST_CHECK(Elements::isEqual(range.getMin(), ref_min));
  BOOST_CHECK(Elements::isEqual(range.getMax(), ref_max));
  BOOST_CHECK(Elements::isEqual(range.getStep(), ref_step));
}

BOOST_FIXTURE_TEST_CASE(setter_test, Range_Fixture) {
  auto range = Range{};
  range.setMin(ref_min);
  range.setMax(ref_max);
  range.setStep(ref_step);
  BOOST_CHECK(Elements::isEqual(range.getMin(), ref_min));
  BOOST_CHECK(Elements::isEqual(range.getMax(), ref_max));
  BOOST_CHECK(Elements::isEqual(range.getStep(), ref_step));
}

BOOST_FIXTURE_TEST_CASE(string_test, Range_Fixture) {
  auto        range    = Range{ref_min, ref_max, ref_step};
  std::string expected = "[1.2 , 5.1] step 0.5123";
  BOOST_CHECK_EQUAL(range.getStringRepresentation(), expected);

  std::stringstream stream(range.getConfigStringRepresentation());
  double            val;
  stream >> val;
  BOOST_CHECK_CLOSE(val, ref_min, 1e-6);
  stream >> val;
  BOOST_CHECK_CLOSE(val, ref_max, 1e-6);
  stream >> val;
  BOOST_CHECK_CLOSE(val, ref_step, 1e-6);
}

// Ends the test suite
BOOST_AUTO_TEST_SUITE_END()
