
#include <string>
#include <set>
#include "PhzQtUI/ModelSet.h"

#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <fstream>
#include <list>
#include <vector>
#include <QStandardItemModel>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include "ElementsKernel/Exception.h"
#include "PhzExecutables/BuildPPConfig.h"
#include "PhzConfiguration/BuildPPConfigConfig.h"
#include "Configuration/Utils.h"

#include "FileUtils.h"
#include "SedParamUtils.h"
#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("SedParamUtils");

SedParamUtils::SedParamUtils() {}

std::map<std::string, std::string> SedParamUtils::getParameterList(const std::string& file) {
  std::map<std::string, std::string> result {};

  std::ifstream sfile(file);
   if (!sfile) {
     throw Elements::Exception() << "File does not exist : " << file;
   }

   std::string  value{};
   std::string  line{};
   std::string  dataset_name{};
   std::string  reg_ex_str = "^\\s*#\\s*PARAMETER\\s*:\\s*([-_\\w]+)\\s*=.+\\s*$";
   boost::regex expression(reg_ex_str);
   std::string  reg_ex_str_unit = "^\\s*#\\s*PARAMETER\\s*:\\s*[-_\\w]+\\s*=.+\\[\\s*(\\w*)\\s*\\]\\s*$";
   boost::regex unit_regex(reg_ex_str_unit);

   while (sfile.good()) {
     std::getline(sfile, line);
     boost::smatch s_match;
     boost::smatch s_unit_match;
     if (!line.empty() && boost::regex_match(line, s_match, expression)) {

       std::string unit = "";
       if (boost::regex_match(line, s_unit_match, unit_regex)) {
         unit = s_unit_match[1].str();
       }
       result.insert(std::make_pair(s_match[1].str(), unit));
       //logger.info() << " INSERTING PARAM " << s_match[1].str() << " With units " << unit;
     }
   }
   return result;
}


std::string SedParamUtils::getParameter(const std::string& file, const std::string& key_word) {
  std::ifstream sfile(file);
  if (!sfile) {
    throw Elements::Exception() << "File does not exist : " << file;
  }

  std::string  value{};
  std::string  line{};
  std::string  dataset_name{};
  std::string  reg_ex_str = "^\\s*#\\s*" + key_word + "\\s*:\\s*(.+)\\s*$";
  boost::regex expression(reg_ex_str);

  while (sfile.good()) {
    std::getline(sfile, line);
    boost::smatch s_match;
    if (!line.empty() && boost::regex_match(line, s_match, expression)) {
      if (value != "") {
         value +=";";
      }
      std::string new_val = s_match[1].str();
      boost::trim(new_val);
      value += new_val;

    }
  }
  return value;
}

std::string SedParamUtils::getName(const std::string& file) {
  // The data set name can be a parameter with keyword NAME
  std::string dataset_name = getParameter(file, "NAME");

  if (dataset_name == "") {
    // IF not present chack the first non-empty line (Backward comatibility)
    std::ifstream sfile(file);
    std::string line{};
    // Check dataset name is in the file
    // Convention: read until found first non empty line, removing empty lines.
    while (line.empty() && sfile.good()) {
      std::getline(sfile, line);
    }

    boost::regex  expression("^\\s*#\\s*(\\w+)\\s*$");
    boost::smatch s_match;
    if (boost::regex_match(line, s_match, expression)) {
      dataset_name = s_match[1].str();
    } else {
      dataset_name = "";
    }
  }

  return dataset_name;
}




std::string SedParamUtils::getFile(const XYDataset::QualifiedName& sed) {
  std::string root_path = FileUtils::getSedRootPath(false) + "/";

  for (std::string group : sed.groups()) {
    root_path = root_path + group + "/";
  }

  QDir directory(QString::fromStdString(root_path));
  QStringList seds_in_group = directory.entryList(QStringList() << "*.*", QDir::Files);

  for (QString filename : seds_in_group) {
     if (QFileInfo(filename).completeBaseName().toStdString() == sed.datasetName()) {
        return root_path + filename.toStdString();
     }
  }

  // name in the file
  for (QString filename : seds_in_group) {
      std::string name = getName(root_path + filename.toStdString());
      if (name == sed.datasetName()) {
         return root_path + filename.toStdString();
      }
   }

  return "";
}


std::set<std::string> SedParamUtils::listAvailableParam(const ModelSet& model) {
  std::map<std::string, std::string> params{};
  bool firstSED = true;
  for (auto& sed : model.getSeds()) {
    auto file_name = SedParamUtils::getFile(sed);
    auto sed_param = SedParamUtils::getParameterList(file_name);
     logger.info() << "SED NAME : "<< sed << " FILE NAME : " << file_name;

    if (firstSED) {
      params = sed_param;
      firstSED = false;
    } else {
      std::vector<std::string> missing_param = {};
      for (auto& existing_param : params) {
        if (sed_param.find(existing_param.first) == sed_param.end()) {
          missing_param.push_back(existing_param.first);
        } else {
          if (sed_param.at(existing_param.first) != existing_param.second) {
            // Missmatch in the units
            missing_param.push_back(existing_param.first);
          }
        }
      }

      for (auto& to_be_removed : missing_param) {
        params.erase(to_be_removed);
      }
    }
  }

  // convert to set
  std::set<std::string> ret;
  std::pair<std::string, std::string> param_pair;
  for (auto it = params.begin(); it != params.end(); ++it) {
    ret.insert(it->first);
  }

  return ret;
}


bool SedParamUtils::createPPConfig(const ModelSet& model, std::set<std::string> params, std::string out_path) {

  logger.info() << " createPPConfig " << out_path;
  auto options = model.getConfigOptions();
  std::vector<std::string> v_params {};
  for (auto& p : params) {
    v_params.push_back(p);
  }
  options["physical-parameter"].value() = boost::any(v_params);
  options["output-file"].value() = boost::any(out_path);


  auto config_manager_id = Configuration::getUniqueManagerId();

  auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
  config_manager.registerConfiguration<PhzConfiguration::BuildPPConfigConfig>();
  config_manager.closeRegistration();

  config_manager.initialize(options);
  PhzExecutables::BuildPPConfig().run(config_manager);


  return true;
}


}
}
