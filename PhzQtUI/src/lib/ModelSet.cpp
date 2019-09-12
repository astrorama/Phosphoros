#include <iostream>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "FileUtils.h"

#include "Configuration/ConfigManager.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "PhzQtUI/ModelSet.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "DefaultOptionsCompleter.h"
#include "Configuration/Utils.h"

namespace po = boost::program_options;
namespace Euclid {
namespace PhzQtUI {

ModelSet::ModelSet(){}

ModelSet::ModelSet(std::string root_path):m_root_path(root_path)
{

}


std::map<int,ModelSet> ModelSet::loadModelSetsFromFolder(std::string root_path){

    QDir root_dir(QString::fromStdString(root_path));

    std::map<int,ModelSet> map ;

    QStringList fileNames = root_dir.entryList(QDir::Files |  QDir::NoDotAndDotDot );
    int count=0;
    foreach (const QString &fileName, fileNames) {
      try{
        auto set = PhzQtUI::ModelSet::loadModelSetFromFile(fileName.toStdString(),root_path);
        map[count]=set;
        ++count;
      } catch(...){} //if a file do not open correctly: just skip it...
    }

    return map;
}

long long ModelSet::getModelNumber(bool recompute ) {
    long long result=0;
    for(auto it = m_parameter_rules.begin(); it != m_parameter_rules.end(); ++it ){
        result+=it->second.getModelNumber(recompute);
    }

    return result;
}


std::map<std::string, boost::program_options::variable_value> ModelSet::getModelNameConfigOptions() const{
  std::map<std::string, po::variable_value> options;
    options["parameter-space-model-name"].value() = boost::any(getName());
  return options;
}
std::map<std::string, po::variable_value> ModelSet::getConfigOptions() const{
  std::map<std::string, po::variable_value> options;
  for (auto& param_rule : getParameterRules()){
    for (auto& option : param_rule.second.getConfigOptions(param_rule.second.getName())){
      options[option.first] = option.second ;
    }
  }

  return options;
}

std::map<std::string,PhzDataModel::ModelAxesTuple> ModelSet::getAxesTuple() const{
  auto options = getConfigOptions();
  completeWithDefaults<PhzConfiguration::ParameterSpaceConfig>(options);

  long config_manager_id = Configuration::getUniqueManagerId();
  auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
  config_manager.registerConfiguration<PhzConfiguration::ParameterSpaceConfig>();
  config_manager.closeRegistration();
  config_manager.initialize(options);

  return config_manager.getConfiguration<PhzConfiguration::ParameterSpaceConfig>().getParameterSpaceRegions();
}


ModelSet ModelSet::deserialize(QDomDocument& doc, ModelSet& model) {
    QDomElement root_node = doc.documentElement();
    model.setName(root_node.attribute("Name").toStdString());

    auto rules_node = root_node.firstChildElement("ParameterRules");
    auto list = rules_node.childNodes();
    for (int i = 0; i < list.count(); ++i) {
      auto node_rule = list.at(i).toElement();
      bool ok;
      auto number = node_rule.attribute("ModelNumber").toLongLong(&ok);
      if (!ok){
        number=-1;
      }

      ParameterRule rule{number};
      rule.setName(node_rule.attribute("Name").toStdString());


      std::set<double> ebv_value_set { };
      auto ebv_sub_list = node_rule.firstChildElement("EbvValues").childNodes();
      for (int j = 0; j < ebv_sub_list.count(); ++j) {
        auto sub_node = ebv_sub_list.at(j).toElement();
        if (sub_node.hasChildNodes()) {
          ebv_value_set.insert(sub_node.text().toDouble());
        }
      }

      if (ebv_value_set.size() > 0) {
        rule.setEbvValues(std::move(ebv_value_set));
      }

      std::vector<Range> ebv_ranges { };
      /*for backward compatibility */
      auto ebv_node = node_rule.firstChildElement("EbvRange");
      if (!ebv_node.isNull()){
        Range range;
        range.setMin(ebv_node.attribute("Min").toDouble());
        range.setMax(ebv_node.attribute("Max").toDouble());
        range.setStep(ebv_node.attribute("Step").toDouble());
        ebv_ranges.push_back(std::move(range));
      }
      /******************************/


      auto ebv_range_node_list = node_rule.firstChildElement("EbvRanges").childNodes();
      for (int j = 0; j < ebv_range_node_list.count(); ++j) {
        auto range_node = ebv_range_node_list.at(j).toElement();
        Range range;
        range.setMin(range_node.attribute("Min").toDouble());
        range.setMax(range_node.attribute("Max").toDouble());
        range.setStep(range_node.attribute("Step").toDouble());
        ebv_ranges.push_back(std::move(range));
      }

      if (ebv_ranges.size() > 0) {
        rule.setEbvRanges(ebv_ranges);
      }

      std::set<double> z_value_set { };
      auto z_sub_list = node_rule.firstChildElement("ZValues").childNodes();
      for (int j = 0; j < z_sub_list.count(); ++j) {
        auto sub_node = z_sub_list.at(j).toElement();
        if (sub_node.hasChildNodes()) {
          z_value_set.insert(sub_node.text().toDouble());
        }
      }

      if (z_value_set.size() > 0) {
        rule.setRedshiftValues(std::move(z_value_set));
      }

      std::vector<Range> z_ranges { };
      /*for backward compatibility */;
      auto z_node = node_rule.firstChildElement("ZRange");
      if (!z_node.isNull()){
        Range range;
        range.setMin(z_node.attribute("Min").toDouble());
        range.setMax(z_node.attribute("Max").toDouble());
        range.setStep(z_node.attribute("Step").toDouble());
        z_ranges.push_back(std::move(range));
      }
      /******************************/

      auto z_range_node_list = node_rule.firstChildElement("ZRanges").childNodes();
      for (int j = 0; j < z_range_node_list.count(); ++j) {
        auto range_node = z_range_node_list.at(j).toElement();
        Range range;
        range.setMin(range_node.attribute("Min").toDouble());
        range.setMax(range_node.attribute("Max").toDouble());
        range.setStep(range_node.attribute("Step").toDouble());
        z_ranges.push_back(std::move(range));
      }

      if (z_ranges.size() > 0) {
        rule.setZRanges(z_ranges);
      }

      auto sed_node_list = node_rule.firstChildElement("SedsSelection");
      DatasetSelection sed_selection{};
      if (!sed_node_list.isNull()){
        std::vector<std::string> groups{};
        auto group_nodes = sed_node_list.firstChildElement("SedGroups").childNodes();
         for (int j = 0; j < group_nodes.count(); ++j) {
           auto group_node = group_nodes.at(j).toElement();
           groups.push_back(group_node.text().toStdString());
        }

        std::vector<std::string> isolated{};
        auto isolated_nodes = sed_node_list.firstChildElement("SedIsolated").childNodes();
         for (int j = 0; j < isolated_nodes.count(); ++j) {
           auto isolated_node = isolated_nodes.at(j).toElement();
           isolated.push_back(isolated_node.text().toStdString());
        }

        std::vector<std::string> excluded{};
        auto excluded_nodes = sed_node_list.firstChildElement("SedExcluded").childNodes();
         for (int j = 0; j < excluded_nodes.count(); ++j) {
           auto excluded_node = excluded_nodes.at(j).toElement();
           excluded.push_back(excluded_node.text().toStdString());
        }
        sed_selection.setGroupes(groups);
        sed_selection.setIsolated(isolated);
        sed_selection.setExclusions(excluded);
      }
      rule.setSedSelection(sed_selection);

      auto red_curve_node_list = node_rule.firstChildElement("RedCurvesSelection");
      DatasetSelection red_selection{};
      if (!red_curve_node_list.isNull()){
        std::vector<std::string> groups{};
        auto group_nodes = red_curve_node_list.firstChildElement("RedCurveGroups").childNodes();
         for (int j = 0; j < group_nodes.count(); ++j) {
           auto group_node = group_nodes.at(j).toElement();
           groups.push_back(group_node.text().toStdString());
        }

        std::vector<std::string> isolated{};
        auto isolated_nodes = red_curve_node_list.firstChildElement("RedCurveIsolated").childNodes();
         for (int j = 0; j < isolated_nodes.count(); ++j) {
           auto isolated_node = isolated_nodes.at(j).toElement();
           isolated.push_back(isolated_node.text().toStdString());
        }

        std::vector<std::string> excluded{};
        auto excluded_nodes = red_curve_node_list.firstChildElement("RedCurveExcluded").childNodes();
         for (int j = 0; j < excluded_nodes.count(); ++j) {
           auto excluded_node = excluded_nodes.at(j).toElement();
           excluded.push_back(excluded_node.text().toStdString());
        }
        red_selection.setGroupes(groups);
        red_selection.setIsolated(isolated);
        red_selection.setExclusions(excluded);
      }
      rule.setRedCurveSelection(red_selection);

      model.m_parameter_rules[i] = rule;
    }

    return model;
  }

QDomDocument ModelSet::serialize(){
    QDomDocument doc("ParameterSpace");
    QDomElement root = doc.createElement("ParameterSpace");
    root.setAttribute("Name", QString::fromStdString(getName()));
    doc.appendChild(root);

    QDomElement rules_Node = doc.createElement("ParameterRules");
    root.appendChild(rules_Node);
    for (auto& rule_pair : m_parameter_rules) {
      auto& rule = rule_pair.second;
      QDomElement rule_node = doc.createElement("ParameterRule");
      rule_node.setAttribute("Name", QString::fromStdString(rule.getName()));
      rule_node.setAttribute("ModelNumber", QString::number(rule.getModelNumber()));

      QDomElement ebv_values_node = doc.createElement("EbvValues");
      for (auto& value : rule.getEbvValues()) {
        QDomElement value_element = doc.createElement("Value");
        value_element.appendChild(doc.createTextNode(QString::number(value,'g',20)));
        ebv_values_node.appendChild(value_element);
      }
      rule_node.appendChild(ebv_values_node);

      QDomElement ebv_ranges_node = doc.createElement("EbvRanges");
      for (auto& range : rule.getEbvRanges()) {
        QDomElement range_element = doc.createElement("Range");
        range_element.setAttribute("Min",range.getMin());
        range_element.setAttribute("Max",range.getMax());
        range_element.setAttribute("Step",range.getStep());
        ebv_ranges_node.appendChild(range_element);
      }
      rule_node.appendChild(ebv_ranges_node);

      QDomElement z_value_node = doc.createElement("ZValues");
      for (auto& values : rule.getRedshiftValues()) {
        QDomElement value_element = doc.createElement("Value");
        value_element.appendChild(doc.createTextNode(QString::number(values,'g',20)));
        z_value_node.appendChild(value_element);
      }
      rule_node.appendChild(z_value_node);

      QDomElement z_ranges_node = doc.createElement("ZRanges");
      for (auto& range : rule.getZRanges()) {
        QDomElement range_element = doc.createElement("Range");
        range_element.setAttribute("Min", range.getMin());
        range_element.setAttribute("Max", range.getMax());
        range_element.setAttribute("Step", range.getStep());
        z_ranges_node.appendChild(range_element);
      }
      rule_node.appendChild(z_ranges_node);

      QDomElement seds_node = doc.createElement("SedsSelection");
      QDomElement sed_groups_node = doc.createElement("SedGroups");
      for (auto& group : rule.getSedSelection().getGroupes()){
        QDomElement text_element = doc.createElement("SedGroup");
        text_element.appendChild(doc.createTextNode(QString::fromStdString(group)));
        sed_groups_node.appendChild(text_element);
      }

      QDomElement sed_isolated_node = doc.createElement("SedIsolated");
      for (auto& lone : rule.getSedSelection().getIsolated()){
        QDomElement text_element = doc.createElement("SedLone");
        text_element.appendChild(doc.createTextNode(QString::fromStdString(lone)));
        sed_isolated_node.appendChild(text_element);
      }

      QDomElement sed_excluded_node = doc.createElement("SedExcluded");
      for (auto& excluded : rule.getSedSelection().getExclusions()){
        QDomElement text_element = doc.createElement("SedExclud");
        text_element.appendChild(doc.createTextNode(QString::fromStdString(excluded)));
        sed_excluded_node.appendChild(text_element);
      }

      seds_node.appendChild(sed_groups_node);
      seds_node.appendChild(sed_isolated_node);
      seds_node.appendChild(sed_excluded_node);
      rule_node.appendChild(seds_node);

      QDomElement reds_node = doc.createElement("RedCurvesSelection");
      QDomElement red_groups_node = doc.createElement("RedCurveGroups");
      for (auto& group : rule.getRedCurveSelection().getGroupes()){
        QDomElement text_element = doc.createElement("RedCurveGroup");
        text_element.appendChild(doc.createTextNode(QString::fromStdString(group)));
        red_groups_node.appendChild(text_element);
      }

      QDomElement red_isolated_node = doc.createElement("RedCurveIsolated");
      for (auto& lone : rule.getRedCurveSelection().getIsolated()){
        QDomElement text_element = doc.createElement("RedCurveLone");
        text_element.appendChild(doc.createTextNode(QString::fromStdString(lone)));
        red_isolated_node.appendChild(text_element);
      }

      QDomElement red_excluded_node = doc.createElement("RedCurveExcluded");
      for (auto& excluded : rule.getRedCurveSelection().getExclusions()){
        QDomElement text_element = doc.createElement("RedCurveExclud");
        text_element.appendChild(doc.createTextNode(QString::fromStdString(excluded)));
        red_excluded_node.appendChild(text_element);
      }

      reds_node.appendChild(red_groups_node);
      reds_node.appendChild(red_isolated_node);
      reds_node.appendChild(red_excluded_node);
      rule_node.appendChild(reds_node);

      rules_Node.appendChild(rule_node);
    }

    return doc;
  }


ModelSet ModelSet::loadModelSetFromFile(std::string fileName,std::string root_path){

    ModelSet model(root_path);
    model.setName(FileUtils::removeExt(fileName,".xml"));


    QDomDocument doc("ParameterSpace");
    QFile file(QString::fromStdString(root_path)+QDir::separator()+QString::fromStdString(fileName));
    if (!file.open(QIODevice::ReadOnly))
         return model;
    if (!doc.setContent(&file)) {
        file.close();
        return model;
    }
    file.close();

    return deserialize(doc,model);
}

void ModelSet::deleteModelSet() {
  QString file_name = QString::fromStdString(m_root_path) + QDir::separator()+ QString::fromStdString(getName()+".xml");
  if (QFile::exists(file_name)) {
    QFile(file_name).remove();
  }
}

void ModelSet::saveModelSet(std::string oldName){
    QFile(QString::fromStdString(m_root_path) + QDir::separator()+ QString::fromStdString(oldName+".xml")).remove();
    QFile file(QString::fromStdString(m_root_path) + QDir::separator()+ QString::fromStdString(getName()+".xml"));
    file.open(QIODevice::WriteOnly );
    QTextStream stream(&file);

    QString xml = serialize().toString();

    stream<<xml;
    file.close();
}


std::string ModelSet::getName() const{
    return m_name;
}

void ModelSet::setName(std::string name){
    m_name=name;
}

std::map<int,ParameterRule> ModelSet::getParameterRules() const{
    return m_parameter_rules;
}

void ModelSet::setParameterRules( std::map<int,ParameterRule> parameter_rules){
    m_parameter_rules=std::move(parameter_rules);
}


template<typename ReturnType, int I>
 std::vector<ReturnType> getCompleteList(const std::map<std::string, PhzDataModel::ModelAxesTuple>& grid_axis_map) {
   std::vector<ReturnType> all_item { };
   for (auto& sub_grid : grid_axis_map) {
     for (auto& item : std::get<I>(sub_grid.second)) {
       if (std::find(all_item.begin(), all_item.end(), item) == all_item.end())
         all_item.push_back(item);
     }
   }

   return all_item;
 }

std::vector<std::string> ModelSet::getSeds() const{
  auto tuples = getAxesTuple();

  auto seds= getCompleteList<XYDataset::QualifiedName,PhzDataModel::ModelParameter::SED>(tuples);

  std::vector<std::string>  result{};

  for (auto& sed : seds){
    result.push_back(sed.qualifiedName());
  }

  return result;

}

}
}

