/*
 * ColumnUtils_test.cpp
 *
 *  Created on: 2025/03/27
 *      Author: fdubath
 */
#include "ElementsKernel/Real.h"  // isEqual
#include "PhzQtUI/ColumnUtils.h"
#include <QString>
#include <vector>
#include <boost/test/unit_test.hpp>  // Gives access to the unit test framework.

using namespace Euclid::PhzQtUI;

struct ColumnUtils_Fixture {
  ColumnUtils columnUtils{{}};
};

// Starts a test suite and name it.
BOOST_AUTO_TEST_SUITE(QtUI_ColumnUtils_test)

BOOST_FIXTURE_TEST_CASE(changeColumnList_test, ColumnUtils_Fixture) {
  QString dummy_filter{"Filter"};
  auto list = columnUtils.getOrderedList(dummy_filter);
  BOOST_CHECK(list.size()==0);
  std::vector<QString> new_list{{"String_1"}, {"String_2"}};
  columnUtils.changeColumnList(new_list);
  list = columnUtils.getOrderedList(dummy_filter);
  BOOST_CHECK(list.size()==2);
  BOOST_CHECK(list[0]=="String_2");
  BOOST_CHECK(list[1]=="String_1");
}

BOOST_FIXTURE_TEST_CASE(categoryWeight_test, ColumnUtils_Fixture) {
  // Flux in the name
  QString column_Flux_begin{"Flux_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_Flux_begin),5.0);
  QString column_flux_begin{"flux_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_flux_begin),5.0);
  QString column_FLUX_begin{"FLUX_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_FLUX_begin),5.0);
  QString column_flux_middle{"hsc_flux_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_flux_middle),5.0);
  QString column_flux_end{"hsc_g_flux"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_flux_end),5.0);
  
  // f at the begining or the end
  QString column_f_begin{"f_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_f_begin),5.0);
  QString column_f_middle{"hsc_f_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_f_middle),4.0);
  QString column_f_end{"hsc_g_f"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_f_end),5.0);
  
  // MAG in the name
  QString column_mag_begin{"mag_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_mag_begin),5.0);
  QString column_mag_middle{"hsc_mag_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_mag_middle),5.0);
  QString column_mag_end{"hsc_g_mag"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_mag_end),5.0);
  
  // m at the begining or the end
  QString column_m_begin{"m_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_m_begin),5.0);
  QString column_m_middle{"hsc_m_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_m_middle),4.0);
  QString column_m_end{"hsc_g_m"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_m_end),5.0);

  // Err in the name
  QString column_fluxerr_begin{"Fluxerr_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_fluxerr_begin, ColumnUtils::ERROR),5.0);
  QString column_fluxerr_middle{"hsc_fluxerr_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_fluxerr_middle,  ColumnUtils::ERROR),5.0);
  QString column_fluxerr_end{"hsc_g_fluxerr"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_fluxerr_end,  ColumnUtils::ERROR),5.0);
  

  // Shiftin the name
  QString column_shift_begin{"shift_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_shift_begin, ColumnUtils::SHIFT),5.0);
  QString column_shift_middle{"hsc_shift_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_shift_middle,  ColumnUtils::SHIFT),5.0);
  QString column_shift_end{"hsc_g_shift"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_shift_end,  ColumnUtils::SHIFT),5.0);
  
   // special case: grouping of other category columns
  QString column_flag_end{"flag_hsc_g_f"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_flag_end),1.0);
  
  QString column_fluxErr{"FLUXERR_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_fluxErr),3.0);
  
  QString column_shift{"SHIFT_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_shift),2.0);
  
  QString column_nothing{"nothing_hsc_g"};
  BOOST_CHECK_EQUAL(columnUtils.categoryWeight(column_nothing),4.0);
   
}


BOOST_FIXTURE_TEST_CASE(similarity_test, ColumnUtils_Fixture) {
  // Empty filter must return 1
  QString column_e{"e"};
  QString column_efg{"efg"};
  QString filter_empty{""};
  BOOST_CHECK_EQUAL(columnUtils.similarity(filter_empty, column_e),0.0);

  // 1 leter filter
  QString filter_e{"e"};
  BOOST_CHECK_EQUAL(columnUtils.similarity(filter_e, column_e),1.0);
  BOOST_CHECK_EQUAL(columnUtils.similarity(filter_e, column_efg),1.0);
  QString filter_a{"a"};
  BOOST_CHECK_EQUAL(columnUtils.similarity(filter_a, column_e),0.0);
  BOOST_CHECK_EQUAL(columnUtils.similarity(filter_a, column_efg),0.0);
  
  // 2 letter filter : test "e", "f", and "ef" with weight 1,1 and 2
  QString filter_ef{"ef"};
  BOOST_CHECK_EQUAL(columnUtils.similarity(filter_ef, column_e), 0.25);
  BOOST_CHECK_EQUAL(columnUtils.similarity(filter_ef, column_efg),1.0);
  
  // from Python prototype
  QString filter_test{"IA484.SuprimeCam"};
  QString col_test{"SC_IA484_FLUX_APER2"};
  BOOST_CHECK_CLOSE(columnUtils.similarity(filter_test, col_test),43.0/816, 0.001);
  
  
}

BOOST_FIXTURE_TEST_CASE(getOrderedList_test, ColumnUtils_Fixture) {
   std::vector<QString> new_list{{"FLUX_HSC_g"}, {"FLUXERR_HSC_g"},{"FLUX_EUCLID_VIS"}, {"FLUXERR_EUCLID_VIS"}};
   columnUtils.changeColumnList(new_list);
   QString filter_vis{"Euclid_VIS"};
   auto list_flux = columnUtils.getOrderedList(filter_vis);
   BOOST_CHECK(list_flux.size()==4);
   BOOST_CHECK(list_flux[0]=="FLUX_EUCLID_VIS");
   BOOST_CHECK(list_flux[1]=="FLUX_HSC_g");
   BOOST_CHECK(list_flux[2]=="FLUXERR_EUCLID_VIS");
   BOOST_CHECK(list_flux[3]=="FLUXERR_HSC_g");
   auto list_err = columnUtils.getOrderedList(filter_vis,  ColumnUtils::ERROR);
   BOOST_CHECK(list_err.size()==4);
   BOOST_CHECK(list_err[0]=="FLUXERR_EUCLID_VIS");
   BOOST_CHECK(list_err[1]=="FLUXERR_HSC_g");
   BOOST_CHECK(list_err[2]=="FLUX_EUCLID_VIS");
   BOOST_CHECK(list_err[3]=="FLUX_HSC_g");
}

// Ends the test suite
BOOST_AUTO_TEST_SUITE_END()
