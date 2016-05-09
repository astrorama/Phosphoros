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
  Range ref_z_range_2=Range{0.1,5.5,0.3};
  Range ref_ebv_range=Range{1.,3.5,0.5};

  std::set<double> ref_z_values{0,6};
  std::set<double> ref_ebv_values{0,1,3};

  DatasetSelection ref_red_selection{};
  DatasetSelection ref_sed_selection{};


  ParameterRule_Fixture(){
    ref_red_selection.setGroupes({"red_group1"});
    ref_red_selection.setIsolated({"isolated_red_1","isolated_red_2"});
    ref_red_selection.setExclusions({"red_group1/red_excluded_1"});

    ref_sed_selection.setGroupes({"sed_group1","sed_group2","sed_group3"});
    ref_sed_selection.setIsolated({"isolated_sed_1","isolated_sed_2"});
    ref_sed_selection.setExclusions({"sed_group1/sed_excluded_1"});
  }
};

// Starts a test suite and name it.
BOOST_AUTO_TEST_SUITE (ParameterRule_test)



BOOST_FIXTURE_TEST_CASE(setter_test, ParameterRule_Fixture) {
  auto parameterRule = ParameterRule{};

  parameterRule.setName(ref_name);

  parameterRule.setZRanges({ref_z_range,ref_z_range_2});
  parameterRule.setEbvRanges({ref_ebv_range});

  parameterRule.setEbvValues(ref_ebv_values);
  parameterRule.setRedshiftValues(ref_z_values);

  parameterRule.setRedCurveSelection(ref_red_selection);
  parameterRule.setSedSelection(ref_sed_selection);

  BOOST_CHECK_EQUAL(parameterRule.getName(),ref_name);

  auto z_ranges = parameterRule.getZRanges();
  BOOST_CHECK_EQUAL(z_ranges.size(),2);
  BOOST_CHECK(Elements::isEqual(z_ranges[0].getMin(),ref_z_range.getMin()));

  auto ebv_range = parameterRule.getEbvRanges();
  BOOST_CHECK_EQUAL(ebv_range.size(),1);
  BOOST_CHECK(Elements::isEqual(ebv_range[0].getMin(),ref_ebv_range.getMin()));

  auto z_values = parameterRule.getRedshiftValues();
  BOOST_CHECK_EQUAL(z_values.size(),2);

  auto ebv_values = parameterRule.getEbvValues();
  BOOST_CHECK_EQUAL(ebv_values.size(),3);

  auto sed_selection = parameterRule.getSedSelection();
  BOOST_CHECK_EQUAL(sed_selection.getGroupes().size(),3);
  BOOST_CHECK_EQUAL(sed_selection.getGroupes()[0],"sed_group1");
  BOOST_CHECK_EQUAL(sed_selection.getGroupes()[1],"sed_group2");
  BOOST_CHECK_EQUAL(sed_selection.getGroupes()[2],"sed_group3");
  BOOST_CHECK_EQUAL(sed_selection.getIsolated().size(),2);
  BOOST_CHECK_EQUAL(sed_selection.getExclusions().size(),1);


}



BOOST_AUTO_TEST_SUITE_END ()



