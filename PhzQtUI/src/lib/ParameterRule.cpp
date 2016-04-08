#include <iostream>
#include <chrono>
#include <regex>
#include <QDir>

#include "ElementsKernel/Real.h" // isEqual

#include "PhzQtUI/ParameterRule.h"
#include "FileUtils.h"
#include "PhzQtUI/XYDataSetTreeModel.h"

#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/AsciiParser.h"
#include "Configuration/ConfigManager.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "PhzConfiguration/SedConfig.h"
#include "PhzConfiguration/ReddeningConfig.h"
#include "PhzConfiguration/RedshiftConfig.h"
#include "DefaultOptionsCompleter.h"
#include "Configuration/Utils.h"

namespace po = boost::program_options;
using namespace std;

namespace Euclid {
namespace PhzQtUI {


ParameterRule::ParameterRule()
{
}

std::string ParameterRule::getName() const{
  return m_name;
}

void ParameterRule::setName(std::string new_name){
   m_name=new_name;
 }

string ParameterRule::getSedRootObject(string rootPath) const{

   return FileUtils::removeStart(m_sed_root_object,rootPath);
}

string ParameterRule::getReddeningRootObject(string rootPath) const{

    return FileUtils::removeStart(m_reddening_root_object,rootPath);

}

long  ParameterRule::getRedCurveNumber() const{
  long red_factor = 1;
     unique_ptr < XYDataset::FileParser >  red_file_parser {new XYDataset::AsciiParser { } };
     XYDataset::FileSystemProvider red_provider { FileUtils::getRedCurveRootPath(false), move(red_file_parser) };
     auto unordered = red_provider.listContents(m_reddening_root_object);
     if (red_factor<long(unordered.size())){
       red_factor=unordered.size();
     }
    return red_factor-getExcludedReddenings().size();
}

long  ParameterRule::getSedNumber() const{
  long sed_factor = 1;
    unique_ptr < XYDataset::FileParser > file_parser {new XYDataset::AsciiParser { } };
    XYDataset::FileSystemProvider provider { FileUtils::getSedRootPath(false), move(file_parser) };
     auto unordered = provider.listContents(m_sed_root_object);
     if (sed_factor<long(unordered.size())){
       sed_factor=unordered.size();
     }
     return sed_factor-getExcludedSeds().size();
}

std::string ParameterRule::getStringValueList(const std::set<double>& list) const{
  std::string string_values = "";
  bool first = true;
  for (auto value : list) {
    if (!first) {
      string_values += ", ";
    } else {
      first = false;
    }

    string_values += QString::number(value).toStdString();
  }
  return string_values;
}

std::string ParameterRule::getRedshiftStringValueList() const{
  return getStringValueList(getRedshiftValues());
}

std::string ParameterRule::getEbvStringValueList() const{
  return getStringValueList(getEbvValues());
}

std::set<double> ParameterRule::parseValueList(const std::string& list){
  auto tokens = QString::fromStdString(list).split(",",QString::SkipEmptyParts);
  std::set<double> result{};
  for (auto& token : tokens){
    bool ok;
    double d = token.toDouble(&ok);
    if (ok){
    result.insert(d);
    }
  }

  return result;
}

std::map<std::string, boost::program_options::variable_value> ParameterRule::getConfigOptions(std::string region) const{
  std::string postfix = "";
  if (region.length()>0){
    postfix = "-"+region;
  }

  map<string, po::variable_value> options;
  XYDataSetTreeModel treeModel_sed;
  treeModel_sed.loadDirectory(FileUtils::getSedRootPath(false), false,"SEDs");
  treeModel_sed.setState(getSedRootObject(),getExcludedSeds());
  auto seds = treeModel_sed.getSelectedLeaf("");
  options["sed-name"+postfix].value() = boost::any(seds);

  XYDataSetTreeModel treeModel_red;
  treeModel_red.loadDirectory(FileUtils::getRedCurveRootPath(false), false,"Reddening Curves");
  treeModel_red.setState(getReddeningRootObject(),getExcludedReddenings());
  auto reds = treeModel_red.getSelectedLeaf("");
  options["reddening-curve-name"+postfix].value() = boost::any(reds);

  vector<string> z_range_vector;
  for (auto& range : m_redshift_ranges){
    z_range_vector.push_back(range.getConfigStringRepresentation());
  }
  if (z_range_vector.size()>0){
    options["z-range"+postfix].value() = boost::any(z_range_vector);
  }

  vector<string> z_value_vector;
  for (auto& value : m_redshift_values){
    z_value_vector.push_back(std::to_string(value));
  }
  if (z_value_vector.size()>0){
    options["z-value"+postfix].value() = boost::any(z_value_vector);
  }

  vector<string> ebv_range_vector;
  for (auto& range : m_ebv_ranges){
    ebv_range_vector.push_back(range.getConfigStringRepresentation());
  }
  if (ebv_range_vector.size()>0){
    options["ebv-range"+postfix].value() = boost::any(ebv_range_vector);
  }

  vector<string> ebv_value_vector;
  for(auto& value : m_ebv_values){
    ebv_value_vector.push_back(std::to_string(value));
  }
  if (ebv_value_vector.size()>0){
    options["ebv-value"+postfix].value() = boost::any(ebv_value_vector);
  }

  return options;
}


long long ParameterRule::getModelNumber() const {

  bool is_zero=false;
  auto options = getConfigOptions("");

  XYDataSetTreeModel treeModel_sed;
  treeModel_sed.loadDirectory(FileUtils::getSedRootPath(false), false,"SEDs");
  treeModel_sed.setState(getSedRootObject(),getExcludedSeds());
  is_zero |= treeModel_sed.getSelectedLeaf("").size()==0;

  XYDataSetTreeModel treeModel_red;
  treeModel_red.loadDirectory(FileUtils::getRedCurveRootPath(false), false,"Reddening Curves");
  treeModel_red.setState(getReddeningRootObject(),getExcludedReddenings());
  is_zero |= treeModel_red.getSelectedLeaf("").size()==0;

  is_zero |= m_redshift_ranges.size()==0 && m_redshift_values.size()==0;
  is_zero |= m_ebv_ranges.size()==0 && m_ebv_values.size()==0;


  if (is_zero){
    return 0;
  }

  options["sed-root-path"].value() = boost::any(FileUtils::getSedRootPath(false));
  options["reddening-curve-root-path"].value() = boost::any(FileUtils::getRedCurveRootPath(false));
  completeWithDefaults<PhzConfiguration::ParameterSpaceConfig>(options);
  long config_manager_id = Configuration::getUniqueManagerId();
  auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
  config_manager.registerConfiguration<PhzConfiguration::ParameterSpaceConfig>();
  config_manager.closeRegistration();
  config_manager.initialize(options);

  return config_manager.getConfiguration<PhzConfiguration::SedConfig>().getSedList().at("").size() *
      config_manager.getConfiguration<PhzConfiguration::ReddeningConfig>().getReddeningCurveList().at("").size() *
      config_manager.getConfiguration<PhzConfiguration::ReddeningConfig>().getEbvList().at("").size() *
      config_manager.getConfiguration<PhzConfiguration::RedshiftConfig>().getZList().at("").size();
}


void ParameterRule::setSedRootObject(string sed_root_object){
    m_sed_root_object=sed_root_object;
}

void ParameterRule::setReddeningRootObject(string reddening_root_object){
    m_reddening_root_object=reddening_root_object;
}

const vector<string>& ParameterRule::getExcludedSeds() const{
    return m_excluded_sed;
}
const vector<string>& ParameterRule::getExcludedReddenings() const{
    return m_excluded_reddening;
}

void ParameterRule::setExcludedSeds( vector<string> excluded_sed){
    m_excluded_sed=move(excluded_sed);
}

void ParameterRule::setExcludedReddenings( vector<string> excluded_reddening){
    m_excluded_reddening=move(excluded_reddening);
}

const std::set<double>& ParameterRule::getEbvValues() const{
  return m_ebv_values;
}

void ParameterRule::setEbvValues(std::set<double> values){
  m_ebv_values = move(values);
}

// TODO
const std::string ParameterRule::getEbvRangeString() const {
  bool is_zero = false;
  auto options = getConfigOptions("");

  XYDataSetTreeModel treeModel_sed;
  treeModel_sed.loadDirectory(FileUtils::getSedRootPath(false), false, "SEDs");
  treeModel_sed.setState(getSedRootObject(), getExcludedSeds());
  is_zero |= treeModel_sed.getSelectedLeaf("").size() == 0;

  XYDataSetTreeModel treeModel_red;
  treeModel_red.loadDirectory(FileUtils::getRedCurveRootPath(false), false,
      "Reddening Curves");
  treeModel_red.setState(getReddeningRootObject(), getExcludedReddenings());
  is_zero |= treeModel_red.getSelectedLeaf("").size() == 0;

  is_zero |= m_redshift_ranges.size() == 0 && m_redshift_values.size() == 0;
  is_zero |= m_ebv_ranges.size() == 0 && m_ebv_values.size() == 0;

  if (is_zero) {
    return "";
  }

  options["sed-root-path"].value() = boost::any(
      FileUtils::getSedRootPath(false));
  options["reddening-curve-root-path"].value() = boost::any(
      FileUtils::getRedCurveRootPath(false));
  completeWithDefaults<PhzConfiguration::ParameterSpaceConfig>(options);
  long config_manager_id = Configuration::getUniqueManagerId();
  auto& config_manager = Configuration::ConfigManager::getInstance(
      config_manager_id);
  config_manager.registerConfiguration<PhzConfiguration::ParameterSpaceConfig>();
  config_manager.closeRegistration();
  config_manager.initialize(options);

  auto axis =
      config_manager.getConfiguration<PhzConfiguration::ReddeningConfig>().getEbvList().at(
          "");

  return getAxisStringValue(axis);

}

const std::string ParameterRule::getRedshiftRangeString() const {
  bool is_zero = false;
  auto options = getConfigOptions("");

  XYDataSetTreeModel treeModel_sed;
  treeModel_sed.loadDirectory(FileUtils::getSedRootPath(false), false, "SEDs");
  treeModel_sed.setState(getSedRootObject(), getExcludedSeds());
  is_zero |= treeModel_sed.getSelectedLeaf("").size() == 0;

  XYDataSetTreeModel treeModel_red;
  treeModel_red.loadDirectory(FileUtils::getRedCurveRootPath(false), false,
      "Reddening Curves");
  treeModel_red.setState(getReddeningRootObject(), getExcludedReddenings());
  is_zero |= treeModel_red.getSelectedLeaf("").size() == 0;

  is_zero |= m_redshift_ranges.size() == 0 && m_redshift_values.size() == 0;
  is_zero |= m_ebv_ranges.size() == 0 && m_ebv_values.size() == 0;

  if (is_zero) {
    return "";
  }

  options["sed-root-path"].value() = boost::any(
      FileUtils::getSedRootPath(false));
  options["reddening-curve-root-path"].value() = boost::any(
      FileUtils::getRedCurveRootPath(false));
  completeWithDefaults<PhzConfiguration::ParameterSpaceConfig>(options);
  long config_manager_id = Configuration::getUniqueManagerId();
  auto& config_manager = Configuration::ConfigManager::getInstance(
      config_manager_id);
  config_manager.registerConfiguration<PhzConfiguration::ParameterSpaceConfig>();
  config_manager.closeRegistration();
  config_manager.initialize(options);

  std::vector<double> axis =
      config_manager.getConfiguration<PhzConfiguration::RedshiftConfig>().getZList().at(
          "");

  return getAxisStringValue(axis);
}




std::string ParameterRule::getAxisStringValue(std::vector<double> axis) const{
   auto min = *(axis.begin());
   auto max = *(--(axis.end()));
   auto size = axis.size();

   if (size == 1) {
     return QString::number(min, 'g', 2).toStdString();
   }

   if (size == 2) {
     return QString::number(min, 'g', 2).toStdString() + ", "
         + QString::number(max, 'g', 2).toStdString();
   }

   return "[" + QString::number(min, 'g', 2).toStdString() + ", "
       + QString::number(max, 'g', 2).toStdString() + "] size "
       + std::to_string(size);
}


const std::set<double>& ParameterRule::getRedshiftValues() const{
  return m_redshift_values;
}

void ParameterRule::setRedshiftValues(std::set<double> values){
  m_redshift_values = move(values);
}

const std::vector<Range>& ParameterRule::getEbvRanges() const{
    return m_ebv_ranges;
}
const std::vector<Range>& ParameterRule::getZRanges() const{
    return m_redshift_ranges;
}

void ParameterRule::setEbvRanges(std::vector<Range> ebv_ranges){
    m_ebv_ranges=move(ebv_ranges);
}

void ParameterRule::setZRanges(std::vector<Range> z_ranges){
    m_redshift_ranges=move(z_ranges);
}

}
}

