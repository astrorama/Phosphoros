/*
 * ParameterRule_test.cpp
 *
 *  Created on: Jun 23, 2015
 *      Author: fdubath
 */
#include <boost/test/unit_test.hpp> // Gives access to the unit test framework.
#include "PhzQtUI/ParameterRule.h"
#include "ElementsKernel/Real.h" // isEqual

using namespace Euclid::PhzQtUI;

struct ParameterRule_Fixture {
  std::string ref_name="ref_rule_name";
  Range ref_z_range=Range{0.1,5.5,0.2};
  Range ref_ebv_range=Range{1.,3.5,0.5};
  std::string ref_sed_root="group1/sed_root";
  std::string ref_red_root="group2/red_root";
  std::vector<std::string> ref_excl_sed{"EX_SED_1","EX_SED_2"};
  std::vector<std::string> ref_excl_red{"EX_RED_1","EX_RED_2","EX_RED_3"};



};

// Starts a test suite and name it.
BOOST_AUTO_TEST_SUITE (ParameterRule_test)

////////////////////////////////////////
// EXCLUDED AS ACCESSING FILE SYSTEM:
// long  getRedCurveNumber() const;
// long  getSedNumber() const;
// long long getModelNumber() const;


BOOST_FIXTURE_TEST_CASE(setter_test, ParameterRule_Fixture) {
  auto parameterRule = ParameterRule{};

  parameterRule.setName(ref_name);

  parameterRule.setZRange(ref_z_range);
  parameterRule.setEbvRange(ref_ebv_range);

  parameterRule.setSedRootObject(ref_sed_root);
  parameterRule.setReddeningRootObject(ref_red_root);

  parameterRule.setExcludedSeds(ref_excl_sed);
  parameterRule.setExcludedReddenings(ref_excl_red);


  BOOST_CHECK_EQUAL(parameterRule.getName(),ref_name);

  auto z_range = parameterRule.getZRange();
  BOOST_CHECK(Elements::isEqual(z_range.getMin(),ref_z_range.getMin()));

  auto ebv_range = parameterRule.getEbvRange();
  BOOST_CHECK(Elements::isEqual(ebv_range.getMin(),ref_ebv_range.getMin()));


  BOOST_CHECK_EQUAL(parameterRule.getSedRootObject(),ref_sed_root);
  BOOST_CHECK_EQUAL(parameterRule.getReddeningRootObject(),ref_red_root);

  BOOST_CHECK_EQUAL(parameterRule.getSedRootObject("group1"),"/sed_root");
  BOOST_CHECK_EQUAL(parameterRule.getReddeningRootObject("group2/"),"red_root");

  auto excl_sed = parameterRule.getExcludedSeds();
  BOOST_CHECK_EQUAL(excl_sed.size(),ref_excl_sed.size());
  for (size_t i=0; i<ref_excl_sed.size();i++){
    BOOST_CHECK_EQUAL(excl_sed[i],ref_excl_sed[i]);
  }

  auto excl_red = parameterRule.getExcludedReddenings();
  BOOST_CHECK_EQUAL(excl_red.size(),ref_excl_red.size());
  for (size_t i=0; i<ref_excl_red.size();i++){
    BOOST_CHECK_EQUAL(excl_red[i],ref_excl_red[i]);
  }
}



BOOST_AUTO_TEST_SUITE_END ()



