/*
 * SedParamUtils_test.cpp
 *
 *  Created on: 2023-03-27
 *      Author: fdubath
 */
#include "ElementsKernel/Real.h"  // isEqual
#include "PhzQtUI/SedParamUtils.h"
#include <boost/test/unit_test.hpp>  // Gives access to the unit test framework.
#include "ElementsKernel/Temporary.h" // for TempDir
#include <iostream>
#include <fstream>

using namespace std;
using namespace Euclid::PhzQtUI;

struct SedParamUtils_Fixture {
	Elements::TempDir m_top_dir{};
};

// Starts a test suite and name it.
BOOST_AUTO_TEST_SUITE(SedParamUtils_test)

BOOST_FIXTURE_TEST_CASE(getParameter_test, SedParamUtils_Fixture) {
    // GIVEN
	auto file_path = (m_top_dir.path()/ "with.txt").string();
	ofstream sed_file;
    sed_file.open (file_path);
    sed_file << "#KEYWORD:Value\n";
    sed_file << "  # TEST : Va l_u-e    \n";
    sed_file << "#NAME:Name\n";
    sed_file << "#KEY:Value1\n";
    sed_file << "#KEY:Value2\n";
    sed_file.close();

    // WHEN
    SedParamUtils su{};
    auto param_value = su.getParameter(file_path, "KEYWORD");
    auto test_value = su.getParameter(file_path, "TEST");
    auto multiple_value = su.getParameter(file_path, "KEY");
    auto name_value = su.getName(file_path);

    // THEN
    BOOST_CHECK_EQUAL(param_value, "Value");
    BOOST_CHECK_EQUAL(test_value, "Va l_u-e");
    BOOST_CHECK_EQUAL(multiple_value, "Value1;Value2");
    BOOST_CHECK_EQUAL(name_value, "Name");


}


// Ends the test suite
BOOST_AUTO_TEST_SUITE_END()
