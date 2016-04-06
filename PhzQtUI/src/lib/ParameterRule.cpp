#include <iostream>
#include <chrono>

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



long long ParameterRule::getModelNumber() const {

  bool is_zero=false;
  map<string, po::variable_value> options;

  options["sed-root-path"].value() = boost::any(FileUtils::getSedRootPath(false));
  XYDataSetTreeModel treeModel_sed;
  treeModel_sed.loadDirectory(FileUtils::getSedRootPath(false), false,"SEDs");
  treeModel_sed.setState(getSedRootObject(),getExcludedSeds());
  auto seds = treeModel_sed.getSelectedLeaf("");

  is_zero |=seds.size()==0;
  options["sed-name"].value() = boost::any(seds);

  options["reddening-curve-root-path"].value() = boost::any(FileUtils::getRedCurveRootPath(false));
  XYDataSetTreeModel treeModel_red;
  treeModel_red.loadDirectory(FileUtils::getRedCurveRootPath(false), false,"Reddening Curves");
  treeModel_red.setState(getReddeningRootObject(),getExcludedReddenings());
  auto reds = treeModel_red.getSelectedLeaf("");

  is_zero |=reds.size()==0;
  options["reddening-curve-name"].value() = boost::any(reds);


  if (m_has_redshift_range){
    vector<string> z_range_vector;
    is_zero |=Elements::isEqual(m_redshift_range.getMin(),m_redshift_range.getMax()) && Elements::isEqual(m_redshift_range.getStep(),0.);
    string z_range=""+to_string(m_redshift_range.getMin())+" "
        +to_string(m_redshift_range.getMax())+" "
        +to_string(m_redshift_range.getStep());
    z_range_vector.push_back(z_range);
    options["z-range"].value() = boost::any(z_range_vector);
  } else {
    vector<string> z_value_vector;
    for (auto& value : m_redshift_value){
      z_value_vector.push_back(std::to_string(value));
    }
    options["z-value"].value() = boost::any(z_value_vector);
  }

  if (m_has_ebv_range){
    vector<string> ebv_range_vector;
    is_zero |=Elements::isEqual(m_ebv_range.getMin(),m_ebv_range.getMax()) && Elements::isEqual(m_ebv_range.getStep(),0.);

    string ebv_range=""+to_string(m_ebv_range.getMin())+" "
        +to_string(m_ebv_range.getMax())+" "
        +to_string(m_ebv_range.getStep());
    ebv_range_vector.push_back(ebv_range);
    options["ebv-range"].value() = boost::any(ebv_range_vector);
  } else {

    vector<string> ebv_value_vector;
    for(auto& value : m_ebv_value){
      ebv_value_vector.push_back(std::to_string(value));
    }
    options["ebv-value"].value() = boost::any(ebv_value_vector);
  }


  if (is_zero){
    return 0;
  }

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

const bool ParameterRule::hasEbvRange() const{
  return m_has_ebv_range;
}

void ParameterRule::setHasEbvRange(bool has_range){
  m_has_ebv_range = has_range;
}

const std::set<double>& ParameterRule::getEbvValues() const{
  return m_ebv_value;
}

void ParameterRule::setEbvValues(std::set<double> values){
  m_ebv_value = move(values);
}

const bool ParameterRule::hasRedshiftRange() const{
  return m_has_redshift_range;
}

void ParameterRule::setHasRedshiftRange(bool has_range){
  m_has_redshift_range = has_range;
}

const std::set<double>& ParameterRule::getRedshiftValues() const{
  return m_redshift_value;
}

void ParameterRule::setRedshiftValues(std::set<double> values){
  m_redshift_value = move(values);
}

const Range& ParameterRule::getEbvRange() const{
    return m_ebv_range;
}
const Range& ParameterRule::getZRange() const{
    return m_redshift_range;
}

void ParameterRule::setEbvRange(Range ebv_range){
    m_ebv_range=move(ebv_range);
}

void ParameterRule::setZRange(Range z_range){
    m_redshift_range=move(z_range);
}

}
}

