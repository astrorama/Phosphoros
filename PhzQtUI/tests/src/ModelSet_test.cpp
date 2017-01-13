/*
 * ModelSet_test.cpp
 *
 *  Created on: Jun 23, 2015
 *      Author: fdubath
 */
#include <boost/test/unit_test.hpp> // Gives access to the unit test framework.
#include "PhzQtUI/ModelSet.h"
#include "ElementsKernel/Real.h" // isEqual

using namespace Euclid::PhzQtUI;

struct ModelSet_Fixture {
  std::string ref_name = "Parameter_space_config_name";
  std::string ref_rule_1_name = "Rule_1";

  ParameterRule ref_rule_1;
  Range ref_z_range = Range { 0.1, 5.5, 0.2 };
  Range ref_ebv_range = Range { 1., 3.5, 0.5 };
  std::string ref_sed_root = "group1/sed_root";
  std::string ref_red_root = "group2/red_root";
  std::vector<std::string> ref_excl_sed { "EX_SED_1", "EX_SED_2" };
  std::vector<std::string> ref_excl_red { "EX_RED_1", "EX_RED_2", "EX_RED_3" };

  ModelSet_Fixture() {
    ref_rule_1.setName(ref_rule_1_name);
    ref_rule_1.setZRange(ref_z_range);
    ref_rule_1.setEbvRange(ref_ebv_range);
    ref_rule_1.setSedRootObject(ref_sed_root);
    ref_rule_1.setReddeningRootObject(ref_red_root);
    ref_rule_1.setExcludedSeds(ref_excl_sed);
    ref_rule_1.setExcludedReddenings(ref_excl_red);

  }

};

// Starts a test suite and name it.
BOOST_AUTO_TEST_SUITE (ModelSet_test)

BOOST_FIXTURE_TEST_CASE(setter_test, ModelSet_Fixture) {
  auto modelSet = ModelSet { };
  modelSet.setName(ref_name);

  std::map<int, ParameterRule> ref_map { std::make_pair(0, ref_rule_1) };
  modelSet.setParameterRules(ref_map);

  BOOST_CHECK_EQUAL(modelSet.getName(), ref_name);
  BOOST_CHECK_EQUAL(modelSet.getParameterRules()[0].getName(), ref_rule_1_name);
}

BOOST_FIXTURE_TEST_CASE(serialize_test, ModelSet_Fixture) {
  auto modelSet = ModelSet { };
  modelSet.setName(ref_name);

  std::map<int, ParameterRule> ref_map { std::make_pair(0, ref_rule_1) };
  modelSet.setParameterRules(ref_map);

  auto doc = modelSet.serialize();
  QDomElement root_node = doc.documentElement();

  BOOST_CHECK_EQUAL(root_node.attribute("Name").toStdString(), ref_name);

  auto rules_node = root_node.firstChildElement("ParameterRules");
  auto rule_element = rules_node.childNodes().at(0).toElement();

  BOOST_CHECK_EQUAL(rule_element.attribute("Name").toStdString(),
      ref_rule_1_name);
  BOOST_CHECK_EQUAL(rule_element.attribute("SedRootObject").toStdString(),
      ref_sed_root);
  BOOST_CHECK_EQUAL(
      rule_element.attribute("ReddeningCurveRootObject").toStdString(),
      ref_red_root);

  auto z_node = rule_element.firstChildElement("ZRange");
  BOOST_CHECK(Elements::isEqual(z_node.attribute("Min").toDouble(), 0.1));
  BOOST_CHECK(Elements::isEqual(z_node.attribute("Max").toDouble(), 5.5));
  BOOST_CHECK(Elements::isEqual(z_node.attribute("Step").toDouble(), 0.2));

  auto ebv_node = rule_element.firstChildElement("EbvRange");
  BOOST_CHECK(Elements::isEqual(ebv_node.attribute("Min").toDouble(), 1.));
  BOOST_CHECK(Elements::isEqual(ebv_node.attribute("Max").toDouble(), 3.5));
  BOOST_CHECK(Elements::isEqual(ebv_node.attribute("Step").toDouble(), 0.5));

  auto excl_sed_list =
      rule_element.firstChildElement("ExcludedSeds").childNodes();
  for (int j = 0; j < excl_sed_list.count(); ++j) {
    auto sub_node = excl_sed_list.at(j).toElement();
    if (sub_node.hasChildNodes()) {
      bool found = false;
      std::string txt = sub_node.text().toStdString();
      for (auto& excl : ref_excl_sed) {
        if (txt.compare(excl) == 0) {
          found = true;
        }
      }

      BOOST_CHECK(found);

    }
  }

  auto excl_red_list =
      rule_element.firstChildElement("ExcludedReddeningCurves").childNodes();
  for (int j = 0; j < excl_red_list.count(); ++j) {
    auto sub_node = excl_red_list.at(j).toElement();
    if (sub_node.hasChildNodes()) {
      bool found = false;
      std::string txt = sub_node.text().toStdString();
      for (auto& excl : ref_excl_red) {
        if (txt.compare(excl) == 0) {
          found = true;
        }
      }

      BOOST_CHECK(found);
    }
  }

}

// Ends the test suite
BOOST_AUTO_TEST_SUITE_END ()

