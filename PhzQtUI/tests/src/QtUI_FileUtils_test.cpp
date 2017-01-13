/*
 * FileUtils_test.cpp
 *
 *  Created on: Jun 23, 2015
 *      Author: fdubath
 */
#include <boost/test/unit_test.hpp> // Gives access to the unit test framework.
#include "src/lib/FileUtils.h"
#include "ElementsKernel/Real.h" // isEqual

using namespace Euclid::PhzQtUI;

struct FileUtils_Fixture {
  std::string ending="with";
  std::string starting="my";
  std::string has_ending="mynameendswith";
  std::string has_not_ending="notmynameendswithn";

};

// Starts a test suite and name it.
BOOST_AUTO_TEST_SUITE (QtUI_FileUtils_test)

BOOST_FIXTURE_TEST_CASE(endsWith_test, FileUtils_Fixture) {
  BOOST_CHECK(FileUtils::ends_with(has_ending,ending));
  BOOST_CHECK(!FileUtils::ends_with(has_not_ending,ending));
}

BOOST_FIXTURE_TEST_CASE(removeExt_test, FileUtils_Fixture) {
  BOOST_CHECK_EQUAL(FileUtils::removeExt(has_ending,ending),"mynameends");
  BOOST_CHECK_EQUAL(FileUtils::removeExt(has_not_ending,ending),has_not_ending);
}

BOOST_FIXTURE_TEST_CASE(addExt_test, FileUtils_Fixture) {
  BOOST_CHECK_EQUAL(FileUtils::addExt(has_ending,ending),has_ending);
  BOOST_CHECK_EQUAL(FileUtils::addExt(has_not_ending,ending),has_not_ending+ending);
}

BOOST_FIXTURE_TEST_CASE(startWith_test, FileUtils_Fixture) {
  BOOST_CHECK(FileUtils::starts_with(has_ending,starting));
  BOOST_CHECK(!FileUtils::starts_with(has_not_ending,starting));
}

BOOST_FIXTURE_TEST_CASE(removeStart_test, FileUtils_Fixture) {
  BOOST_CHECK_EQUAL(FileUtils::removeStart(has_ending,starting),"nameendswith");
  BOOST_CHECK_EQUAL(FileUtils::removeStart(has_not_ending,starting),has_not_ending);
}

// Ends the test suite
BOOST_AUTO_TEST_SUITE_END ()



