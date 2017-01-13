/**
 * @file tests/src/PhzModel_test.cpp
 * @date Sep 23, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/PhzModel.h"

struct PhzModel_Fixture {

  std::vector<double> zs{0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
  std::vector<double> ebvs{0.0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009,0.01};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"},{"reddeningCurves/Curve2"},{"reddeningCurves/Curve3"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve1"},{"sed/Curve2"},{"sed/Curve3"},{"sed/Curve4"},{"sed/Curve5"},{"sed/Curve6"},{"sed/Curve7"}};

  PhzModel_Fixture(){
    }
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhzModel_test)

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(createAxesTuple_test, PhzModel_Fixture) {
   BOOST_TEST_MESSAGE(" ");
   BOOST_TEST_MESSAGE("--> Testing the Axes tuple creation");
   BOOST_TEST_MESSAGE(" ");

   auto axes= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);


  auto axe_to_test_double= std::get<0>(axes);
  BOOST_CHECK("Z"==axe_to_test_double.name());
  BOOST_CHECK_EQUAL(zs.size(), axe_to_test_double.size());
  auto axe_iterator = axe_to_test_double.begin();
  for(auto ref_z :zs){
    BOOST_CHECK(Elements::isEqual(ref_z,*axe_iterator));
    ++axe_iterator;
  }

  axe_to_test_double= std::get<1>(axes);
  BOOST_CHECK("E(B-V)"==axe_to_test_double.name());
  BOOST_CHECK_EQUAL(ebvs.size(), axe_to_test_double.size());
  axe_iterator = axe_to_test_double.begin();
  for(auto ref_ebv :ebvs){
    BOOST_CHECK(Elements::isEqual(ref_ebv,*axe_iterator));
    ++axe_iterator;
  }

  auto axe_to_test_qualified_name= std::get<2>(axes);
  BOOST_CHECK("Reddening Curve"==axe_to_test_qualified_name.name());
  BOOST_CHECK_EQUAL(reddeing_curves.size(), axe_to_test_qualified_name.size());
  auto axe_iterator_qualified_name = axe_to_test_qualified_name.begin();
  for(auto ref_reddening :reddeing_curves){
    BOOST_CHECK(ref_reddening == *axe_iterator_qualified_name);
    ++axe_iterator_qualified_name;
  }

    axe_to_test_qualified_name= std::get<3>(axes);
  BOOST_CHECK("SED"==axe_to_test_qualified_name.name());
  BOOST_CHECK_EQUAL(seds.size(), axe_to_test_qualified_name.size());
    axe_iterator_qualified_name = axe_to_test_qualified_name.begin();
  for(auto ref_reddening :seds){
    BOOST_CHECK(ref_reddening == *axe_iterator_qualified_name);
    ++axe_iterator_qualified_name;
  }
}

BOOST_AUTO_TEST_SUITE_END ()
