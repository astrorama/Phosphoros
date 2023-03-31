/*
 * ModelSet_test.cpp
 *
 *  Created on: Jun 23, 2015
 *      Author: fdubath
 */
#include "ElementsKernel/Real.h"  // isEqual
#include "PhzQtUI/ModelSet.h"
#include <boost/test/unit_test.hpp>  // Gives access to the unit test framework.
#include "PhzQtUI/DatasetRepository.h"

using namespace Euclid::PhzQtUI;
typedef std::shared_ptr<Euclid::PhzQtUI::DatasetRepository<std::unique_ptr<Euclid::XYDataset::FileSystemProvider>>> DatasetRepo;

struct ModelSet_Fixture {
  std::string ref_name        = "Parameter_space_config_name";
  std::string ref_rule_1_name = "Rule_1";

  ParameterRule ref_rule_1;
  Range         ref_z_range   = Range{0.1, 5.5, 0.2};
  Range         ref_ebv_range = Range{1., 3.5, 0.5};

  std::set<double> ref_z_values{0, 6};
  std::set<double> ref_ebv_values{0, 1, 3, 5, 10};

  DatasetSelection ref_red_selection{};
  DatasetSelection ref_sed_selection{};
  DatasetRepo sed_repo = nullptr;
  DatasetRepo red_repo = nullptr;

  ModelSet_Fixture() {

    ref_red_selection.setGroupes({"red_group1"});
    ref_red_selection.setIsolated({"isolated_red_1", "isolated_red_2"});
    ref_red_selection.setExclusions({"red_group1/red_excluded_1"});

    ref_sed_selection.setGroupes({"sed_group1", "sed_group2", "sed_group3"});
    ref_sed_selection.setIsolated({"isolated_sed_1", "isolated_sed_2"});
    ref_sed_selection.setExclusions({"sed_group1/sed_excluded_1"});

    ref_rule_1.setName(ref_rule_1_name);
    ref_rule_1.setZRanges({ref_z_range});
    ref_rule_1.setEbvRanges({ref_ebv_range});

    ref_rule_1.setEbvValues(ref_ebv_values);
    ref_rule_1.setRedshiftValues(ref_z_values);
    ref_rule_1.setRedCurveSelection(ref_red_selection);
    ref_rule_1.setSedSelection(ref_sed_selection);
  }
};

// Starts a test suite and name it.
BOOST_AUTO_TEST_SUITE(ModelSet_test)

BOOST_FIXTURE_TEST_CASE(setter_test, ModelSet_Fixture) {
  auto modelSet = ModelSet{sed_repo, red_repo};
  modelSet.setName(ref_name);

  std::map<int, ParameterRule> ref_map{std::make_pair(0, ref_rule_1)};
  modelSet.setParameterRules(ref_map);

  BOOST_CHECK_EQUAL(modelSet.getName(), ref_name);
  BOOST_CHECK_EQUAL(modelSet.getParameterRules()[0].getName(), ref_rule_1_name);
}

BOOST_FIXTURE_TEST_CASE(serialize_test, ModelSet_Fixture) {
  auto modelSet = ModelSet{sed_repo, red_repo};
  modelSet.setName(ref_name);
  BOOST_CHECK_EQUAL(1,1);
  std::map<int, ParameterRule> ref_map{std::make_pair(0, ref_rule_1)};
  modelSet.setParameterRules(ref_map);
  BOOST_CHECK_EQUAL(1,1);

  auto        doc       = modelSet.serialize();
  QDomElement root_node = doc.documentElement();

  BOOST_CHECK_EQUAL(root_node.attribute("Name").toStdString(), ref_name);

  auto rules_node   = root_node.firstChildElement("ParameterRules");
  auto rule_element = rules_node.childNodes().at(0).toElement();

  BOOST_CHECK_EQUAL(rule_element.attribute("Name").toStdString(), ref_rule_1_name);

  auto z_range_node_list = rule_element.firstChildElement("ZRanges").childNodes();
  for (int j = 0; j < z_range_node_list.count(); ++j) {
    auto range_node = z_range_node_list.at(j).toElement();
    BOOST_CHECK(Elements::isEqual(range_node.attribute("Min").toDouble(), 0.1));
    BOOST_CHECK(Elements::isEqual(range_node.attribute("Max").toDouble(), 5.5));
    BOOST_CHECK(Elements::isEqual(range_node.attribute("Step").toDouble(), 0.2));
  }

  auto ebv_range_node_list = rule_element.firstChildElement("EbvRanges").childNodes();
  for (int j = 0; j < ebv_range_node_list.count(); ++j) {
    auto range_node = ebv_range_node_list.at(j).toElement();
    BOOST_CHECK(Elements::isEqual(range_node.attribute("Min").toDouble(), 1.));
    BOOST_CHECK(Elements::isEqual(range_node.attribute("Max").toDouble(), 3.5));
    BOOST_CHECK(Elements::isEqual(range_node.attribute("Step").toDouble(), 0.5));
  }

  std::set<double> z_value_set{};
  auto             z_sub_list = rule_element.firstChildElement("ZValues").childNodes();
  for (int j = 0; j < z_sub_list.count(); ++j) {
    auto sub_node = z_sub_list.at(j).toElement();
    if (sub_node.hasChildNodes()) {
      z_value_set.insert(sub_node.text().toDouble());
    }
  }

  BOOST_CHECK_EQUAL(z_value_set.size(), ref_z_values.size());
  for (auto& value : z_value_set) {
    BOOST_CHECK(ref_z_values.find(value) != ref_z_values.end());
  }

  std::set<double> ebv_value_set{};
  auto             ebv_sub_list = rule_element.firstChildElement("EbvValues").childNodes();
  for (int j = 0; j < ebv_sub_list.count(); ++j) {
    auto sub_node = ebv_sub_list.at(j).toElement();
    if (sub_node.hasChildNodes()) {
      ebv_value_set.insert(sub_node.text().toDouble());
    }
  }

  BOOST_CHECK_EQUAL(ebv_value_set.size(), ref_ebv_values.size());
  for (auto& value : ebv_value_set) {
    BOOST_CHECK(ref_ebv_values.find(value) != ref_ebv_values.end());
  }

  auto sed_node_list = rule_element.firstChildElement("SedsSelection");
  auto group_nodes   = sed_node_list.firstChildElement("SedGroups").childNodes();
  BOOST_CHECK_EQUAL(group_nodes.count(), ref_sed_selection.getGroupes().size());
  BOOST_CHECK_EQUAL(group_nodes.at(0).toElement().text().toStdString(), ref_sed_selection.getGroupes()[0]);

  auto isolated_nodes = sed_node_list.firstChildElement("SedIsolated").childNodes();
  BOOST_CHECK_EQUAL(isolated_nodes.count(), ref_sed_selection.getIsolated().size());
  BOOST_CHECK_EQUAL(isolated_nodes.at(0).toElement().text().toStdString(), ref_sed_selection.getIsolated()[0]);

  auto excluded_nodes = sed_node_list.firstChildElement("SedExcluded").childNodes();
  BOOST_CHECK_EQUAL(excluded_nodes.count(), ref_sed_selection.getExclusions().size());
  BOOST_CHECK_EQUAL(excluded_nodes.at(0).toElement().text().toStdString(), ref_sed_selection.getExclusions()[0]);

  auto red_node_list = rule_element.firstChildElement("RedCurvesSelection");
  group_nodes        = red_node_list.firstChildElement("RedCurveGroups").childNodes();
  BOOST_CHECK_EQUAL(group_nodes.count(), ref_red_selection.getGroupes().size());
  BOOST_CHECK_EQUAL(group_nodes.at(0).toElement().text().toStdString(), ref_red_selection.getGroupes()[0]);

  isolated_nodes = red_node_list.firstChildElement("RedCurveIsolated").childNodes();
  BOOST_CHECK_EQUAL(isolated_nodes.count(), ref_red_selection.getIsolated().size());
  BOOST_CHECK_EQUAL(isolated_nodes.at(0).toElement().text().toStdString(), ref_red_selection.getIsolated()[0]);

  excluded_nodes = red_node_list.firstChildElement("RedCurveExcluded").childNodes();
  BOOST_CHECK_EQUAL(excluded_nodes.count(), ref_red_selection.getExclusions().size());
  BOOST_CHECK_EQUAL(excluded_nodes.at(0).toElement().text().toStdString(), ref_red_selection.getExclusions()[0]);
}

// Ends the test suite
BOOST_AUTO_TEST_SUITE_END()
