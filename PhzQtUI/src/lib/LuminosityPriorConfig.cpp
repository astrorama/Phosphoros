/*
 * LuminosityPriorConfig.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: fdubath
 */

#include <QDir>
#include <QTextStream>
#include <set>

#include "FileUtils.h"
#include "PhzQtUI/LuminosityPriorConfig.h"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {

std::string LuminosityPriorConfig::getName() const {
  return m_name;
}
void LuminosityPriorConfig::setName(std::string name) {
  m_name = name;
}

bool LuminosityPriorConfig::getInMag() const {
  return m_in_mag;
}
void LuminosityPriorConfig::setInMag(bool in_mag) {
  m_in_mag = std::move(in_mag);
}

std::vector<LuminosityPriorConfig::SedGroup> LuminosityPriorConfig::getSedGRoups() const {
  return m_sed_groups;
}
void LuminosityPriorConfig::setSedGroups(std::vector<SedGroup> sed_groups) {
  m_sed_groups = std::move(sed_groups);
}

std::vector<double> LuminosityPriorConfig::getZs() const {
  return m_zs;
}
void LuminosityPriorConfig::setZs(std::vector<double> zs) {
  m_zs = std::move(zs);
}

std::vector<LuminosityFunctionInfo> LuminosityPriorConfig::getLuminosityFunctionList() const {
  return m_luminosity_function_list;
}
void LuminosityPriorConfig::setLuminosityFunctionList(std::vector<LuminosityFunctionInfo> functions) {
  m_luminosity_function_list = std::move(functions);
}

std::vector<std::vector<LuminosityFunctionInfo>> LuminosityPriorConfig::getLuminosityFunctionArray() const {
  std::vector<std::vector<LuminosityFunctionInfo>> result{};

  for (size_t i = 0; i < m_zs.size() - 1; ++i) {
    result.push_back(std::vector<LuminosityFunctionInfo>{});
    for (size_t j = 0; j < m_sed_groups.size(); ++j) {
      result[i].push_back({});
    }
  }

  std::map<double, size_t> z_map;
  for (size_t i = 0; i < m_zs.size() - 1; ++i) {
    z_map[m_zs[i]] = i;
  }

  std::map<std::string, size_t> group_map;
  for (size_t j = 0; j < m_sed_groups.size(); ++j) {
    group_map[m_sed_groups[j].first] = j;
  }

  for (auto info : m_luminosity_function_list) {
    size_t i = z_map[info.z_min];
    size_t j = group_map[info.sedGroupName];

    result[i][j] = info;
  }

  return result;
}

bool LuminosityPriorConfig::isCompatibleWithZ(double z_min, double z_max) const {
  return m_zs[0] == z_min && m_zs[m_zs.size() - 1] == z_max;
}

std::pair<std::vector<std::string>, std::vector<std::string>>
LuminosityPriorConfig::isCompatibleWithSeds(std::vector<std::string> seds) const {
  std::set<std::string> all_seds{};
  for (auto& group : m_sed_groups) {
    for (auto& sed : group.second) {
      all_seds.emplace(sed);
    }
  }

  std::vector<std::string> seds_not_in_the_parameter_space{};
  std::vector<std::string> seds_not_in_the_config{};

  for (auto& sed : seds) {
    if (all_seds.find(sed) != all_seds.end()) {
      all_seds.erase(sed);
    } else {
      seds_not_in_the_config.push_back(sed);
    }
  }

  for (auto& sed : all_seds) {
    seds_not_in_the_parameter_space.push_back(sed);
  }

  return std::make_pair(seds_not_in_the_parameter_space, seds_not_in_the_config);
}

bool LuminosityPriorConfig::isCompatibleWithParameterSpace(double z_min, double z_max,
                                                           std::vector<std::string> seds) const {
  return isCompatibleWithZ(z_min, z_max) && isCompatibleWithSeds(seds).first.size() == 0 &&
         isCompatibleWithSeds(seds).second.size() == 0;
}

LuminosityPriorConfig LuminosityPriorConfig::deserialize(QDomDocument& doc) {
  LuminosityPriorConfig config{};

  QDomElement root_node = doc.documentElement();
  config.setInMag(root_node.attribute("InMag").toStdString() != "0");

  auto groups_node = root_node.firstChildElement("SedGroups");
  auto group_list  = groups_node.childNodes();
  for (int i = 0; i < group_list.count(); ++i) {
    auto        node_group = group_list.at(i).toElement();
    std::string group_name = node_group.attribute("Name").toStdString();
    auto        seds_node  = node_group.firstChildElement("Seds");
    auto        sed_list   = seds_node.childNodes();

    std::vector<std::string> sed_vector;
    for (int j = 0; j < sed_list.count(); ++j) {
      auto node_sed = sed_list.at(j).toElement();
      sed_vector.push_back({node_sed.attribute("Name").toStdString()});
    }

    config.m_sed_groups.emplace_back(group_name, sed_vector);
  }

  auto functions_node = root_node.firstChildElement("LuminosityFunctions");
  auto funct_list     = functions_node.childNodes();
  for (int i = 0; i < funct_list.count(); ++i) {
    auto                   funct_node = funct_list.at(i).toElement();
    LuminosityFunctionInfo info{};
    info.in_mag       = config.m_in_mag;
    info.sedGroupName = funct_node.attribute("SedGroup").toStdString();
    info.z_min        = std::stod(funct_node.attribute("MinZ").toStdString());
    info.z_max        = std::stod(funct_node.attribute("MaxZ").toStdString());
    info.is_custom    = std::stoi(funct_node.attribute("Custom").toStdString()) == 1;
    if (info.is_custom) {
      info.curve_name = funct_node.attribute("CurveName").toStdString();
    } else {
      info.alpha = std::stod(funct_node.attribute("Alpha").toStdString());
      info.phi   = std::stod(funct_node.attribute("Phi0").toStdString());

      if (info.in_mag) {
        info.m = std::stod(funct_node.attribute("M0").toStdString());
      } else {
        info.l = std::stod(funct_node.attribute("L0").toStdString());
      }
    }

    config.m_luminosity_function_list.push_back(info);
  }

  std::set<double> zs{};

  for (auto& info : config.m_luminosity_function_list) {
    if (zs.find(info.z_min) == zs.end()) {
      zs.insert(info.z_min);
    }

    if (zs.find(info.z_max) == zs.end()) {
      zs.insert(info.z_max);
    }
  }

  for (double z : zs) {
    config.m_zs.push_back(z);
  }

  return config;
}

QDomDocument LuminosityPriorConfig::serialize() const {
  QDomDocument doc("LuminosityPrior");
  QDomElement  root = doc.createElement("LuminosityPrior");

  root.setAttribute("InMag", QString::fromStdString(std::to_string(m_in_mag)));
  doc.appendChild(root);

  QDomElement groups_node = doc.createElement("SedGroups");
  root.appendChild(groups_node);

  for (auto& group : m_sed_groups) {
    QDomElement group_node = doc.createElement("SedGroup");
    group_node.setAttribute("Name", QString::fromStdString(group.first));
    groups_node.appendChild(group_node);
    QDomElement seds_node = doc.createElement("Seds");
    group_node.appendChild(seds_node);
    for (auto& sed : group.second) {
      QDomElement sed_Node = doc.createElement("Sed");
      sed_Node.setAttribute("Name", QString::fromStdString(sed));
      seds_node.appendChild(sed_Node);
    }
  }

  QDomElement functions_node = doc.createElement("LuminosityFunctions");
  root.appendChild(functions_node);
  for (auto& function : m_luminosity_function_list) {
    QDomElement funct_node = doc.createElement("LuminosityFunction");
    funct_node.setAttribute("SedGroup", QString::fromStdString(function.sedGroupName));
    funct_node.setAttribute("MinZ", QString::number(function.z_min, 'g', 15));
    funct_node.setAttribute("MaxZ", QString::number(function.z_max, 'g', 15));
    funct_node.setAttribute("Custom", QString::fromStdString(std::to_string(function.is_custom)));

    if (function.is_custom) {
      funct_node.setAttribute("CurveName", QString::fromStdString(function.curve_name));
    } else {

      funct_node.setAttribute("Alpha", QString::number(function.alpha, 'g', 15));
      funct_node.setAttribute("Phi0", QString::number(function.phi, 'g', 15));

      if (m_in_mag) {
        funct_node.setAttribute("M0", QString::number(function.m, 'g', 15));
      } else {
        funct_node.setAttribute("L0", QString::number(function.l, 'g', 15));
      }
    }
    functions_node.appendChild(funct_node);
  }

  return doc;
}

std::map<std::string, boost::program_options::variable_value>
LuminosityPriorConfig::getBasicConfigOptions(bool input) const {
  std::map<std::string, po::variable_value> options;

  std::string mag_value = "YES";
  if (!m_in_mag) {
    mag_value = "NO";
  }

  options["luminosity-function-expressed-in-magnitude"].value() = boost::any(mag_value);

  return options;
}

std::map<std::string, boost::program_options::variable_value> LuminosityPriorConfig::getConfigOptions() const {

  auto options = getBasicConfigOptions();

  options["luminosity-prior"].value() = boost::any(std::string("YES"));

  for (auto& group : m_sed_groups) {
    std::string name = group.first;

    std::string parameter = "";

    std::vector<std::string> seds = group.second;
    size_t                   max  = seds.size() - 1;
    for (size_t i = 0; i < seds.size(); ++i) {
      parameter += seds[i];
      if (i < max) {
        parameter += ",";
      }
    }

    options["luminosity-sed-group-" + name].value() = boost::any(parameter);
  }

  int item_id = 1;
  for (auto& function : m_luminosity_function_list) {

    auto name = std::to_string(item_id);

    options["luminosity-function-sed-group-" + name].value() = boost::any(function.sedGroupName);
    options["luminosity-function-min-z-" + name].value()     = boost::any(function.z_min);
    options["luminosity-function-max-z-" + name].value()     = boost::any(function.z_max);

    if (function.is_custom) {

      options["luminosity-function-curve-" + name].value() = boost::any(function.curve_name);
    } else {
      options["luminosity-function-schechter-alpha-" + name].value() = boost::any(function.alpha);

      if (m_in_mag) {
        options["luminosity-function-schechter-m0-" + name].value() = boost::any(function.m);
      } else {
        options["luminosity-function-schechter-l0-" + name].value() = boost::any(function.l);
      }
      options["luminosity-function-schechter-phi0-" + name].value() = boost::any(function.phi);
    }

    ++item_id;
  }

  return options;
}

std::map<std::string, LuminosityPriorConfig> LuminosityPriorConfig::readFolder(std::string folder) {

  QStringList nameFilter("*.xml");
  QDir        directory(QString::fromStdString(folder));
  QStringList files = directory.entryList(nameFilter);

  std::map<std::string, LuminosityPriorConfig> result{};
  for (auto& file_name : files) {
    QDomDocument doc("LuminosityPrior");
    QFile        file(QString::fromStdString(folder) + QDir::separator() + file_name);
    if (!file.open(QIODevice::ReadOnly)) {
      continue;
    }

    if (!doc.setContent(&file)) {
      file.close();
      continue;
    }

    file.close();

    auto prior = deserialize(doc);

    prior.setName(FileUtils::removeExt(file_name.toStdString(), ".xml"));

    result[prior.getName()] = std::move(prior);
  }

  return result;
}

void LuminosityPriorConfig::writeFolder(std::string folder, std::map<std::string, LuminosityPriorConfig> configs) {

  QDir dir(QString::fromStdString(folder) + QDir::separator());
  dir.setNameFilters(QStringList() << "*.*");
  dir.setFilter(QDir::Files);
  foreach (QString dirFile, dir.entryList()) { dir.remove(dirFile); }

  for (auto& prior_pair : configs) {
    QString file_name =
        QString::fromStdString(folder) + QDir::separator() + QString::fromStdString(prior_pair.first + ".xml");
    QFile file(file_name);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    stream << prior_pair.second.serialize().toString();
    file.close();
  }
}

}  // namespace PhzQtUI
}  // namespace Euclid
