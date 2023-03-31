#include <QDir>
#include <chrono>
#include <iostream>
#include <regex>

#include "ElementsKernel/Exception.h"  // isEqual
#include "ElementsKernel/Real.h"       // isEqual

#include "FileUtils.h"
#include "PhzQtUI/ParameterRule.h"

#include "Configuration/ConfigManager.h"
#include "Configuration/Utils.h"
#include "DefaultOptionsCompleter.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "PhzConfiguration/ReddeningConfig.h"
#include "PhzConfiguration/RedshiftConfig.h"
#include "PhzConfiguration/SedConfig.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/QualifiedName.h"

namespace po = boost::program_options;
using namespace std;

namespace Euclid {
namespace PhzQtUI {

template <typename T>
static std::string nonLocalizedString(const T& v) {
  std::stringstream stream;
  stream.imbue(std::locale("C"));
  stream << v;
  return stream.str();
}

ParameterRule::ParameterRule() {}

ParameterRule::ParameterRule(long long model_number) : m_model_number(model_number) {}

std::string ParameterRule::getName() const {
  return m_name;
}

void ParameterRule::setName(std::string new_name) {
  m_name = new_name;
}

std::string ParameterRule::getRedCurveGroupName() const {
  return m_red_curve_selection.getBaseGroupName();
}

std::pair<long, long> ParameterRule::getRedCurveNumber(DatasetRepo redenig_curves_repository) const {
  return getSelectionNumbers(m_red_curve_selection, redenig_curves_repository);
}

std::string ParameterRule::getSedGroupName() const {
  return m_sed_selection.getBaseGroupName();
}

std::pair<long, long> ParameterRule::getSedNumber(DatasetRepo seds_repository) const {
  return getSelectionNumbers(m_sed_selection, seds_repository);
}

std::pair<long, long> ParameterRule::getSelectionNumbers(DatasetSelection selection, DatasetRepo repository) const {
  std::vector<XYDataset::QualifiedName> empty{};
  const std::vector<XYDataset::QualifiedName>&  full_list = repository!=nullptr ? repository->getContent() : empty;


  std::set<XYDataset::QualifiedName> selected_items{};

  for (auto& item : full_list) {

    // add groups
    for (auto& group : selection.getGroupes()) {
      XYDataset::QualifiedName group_name(group);
      if (item.belongsInGroup(group_name)) {
        selected_items.insert(item);
      }
    }

    // isolated
    for (auto& lone : selection.getIsolated()) {
      XYDataset::QualifiedName lone_name(lone);

      if (item == lone_name) {
        selected_items.insert(item);
      }
    }
  }

  std::vector<XYDataset::QualifiedName> final_items{};
  for (auto& item : selected_items) {
    // excluded
    bool excluded = false;
    for (auto& exclusion : selection.getExclusions()) {
      XYDataset::QualifiedName exclusion_name(exclusion);

      if (item == exclusion_name) {
        excluded = true;
        break;
      }
    }

    if (!excluded) {
      final_items.push_back(item);
    }
  }

  long total = 0;
  if (selection.hasMultipleGroups()) {
    total = full_list.size();
  } else {
    if (selection.getIsolated().size() == 1) {
      total = 1;
    } else {
      if (selection.getGroupes().size() > 0) {
        auto group_name = XYDataset::QualifiedName(selection.getGroupes()[0]);
        for (auto& item : repository->getContent()) {
          if (item.belongsInGroup(group_name)) {
            ++total;
          }
        }
      }
    }
  }

  long selected = final_items.size();
  return std::make_pair<long, long>(std::move(selected), std::move(total));
}

std::string ParameterRule::getStringValueList(const std::set<double>& list) const {
  std::string string_values = "";
  bool        first         = true;
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

std::string ParameterRule::getRedshiftStringValueList() const {
  return getStringValueList(getRedshiftValues());
}

std::string ParameterRule::getEbvStringValueList() const {
  return getStringValueList(getEbvValues());
}

std::set<double> ParameterRule::parseValueList(const std::string& list) {
  auto             tokens = QString::fromStdString(list).split(",", QString::SkipEmptyParts);
  std::set<double> result{};
  for (auto& token : tokens) {
    bool   ok;
    double d = token.toDouble(&ok);
    if (ok) {
      result.insert(d);
    }
  }

  return result;
}

std::map<std::string, boost::program_options::variable_value>
ParameterRule::getConfigOptions(std::string region) const {
  std::string postfix = "";
  if (region.length() > 0) {
    postfix = "-" + region;
  }

  map<string, po::variable_value> options;

  if (m_sed_selection.getGroupes().size() > 0) {
    options["sed-group" + postfix].value() = boost::any(m_sed_selection.getGroupes());
  }
  if (m_sed_selection.getIsolated().size() > 0) {
    options["sed-name" + postfix].value() = boost::any(m_sed_selection.getIsolated());
  }
  if (m_sed_selection.getExclusions().size() > 0) {
    options["sed-exclude" + postfix].value() = boost::any(m_sed_selection.getExclusions());
  }

  if (m_red_curve_selection.getGroupes().size() > 0) {
    options["reddening-curve-group" + postfix].value() = boost::any(m_red_curve_selection.getGroupes());
  }
  if (m_red_curve_selection.getIsolated().size() > 0) {
    options["reddening-curve-name" + postfix].value() = boost::any(m_red_curve_selection.getIsolated());
  }
  if (m_red_curve_selection.getExclusions().size() > 0) {
    options["reddening-curve-exclude" + postfix].value() = boost::any(m_red_curve_selection.getExclusions());
  }

  vector<string> z_range_vector;
  for (auto& range : m_redshift_ranges) {
    z_range_vector.push_back(range.getConfigStringRepresentation());
  }
  if (z_range_vector.size() > 0) {
    options["z-range" + postfix].value() = boost::any(z_range_vector);
  }

  vector<string> z_value_vector;
  for (auto& value : m_redshift_values) {
    z_value_vector.push_back(nonLocalizedString(value));
  }
  if (z_value_vector.size() > 0) {
    options["z-value" + postfix].value() = boost::any(z_value_vector);
  }

  vector<string> ebv_range_vector;
  for (auto& range : m_ebv_ranges) {
    ebv_range_vector.push_back(range.getConfigStringRepresentation());
  }
  if (ebv_range_vector.size() > 0) {
    options["ebv-range" + postfix].value() = boost::any(ebv_range_vector);
  }

  vector<string> ebv_value_vector;
  for (auto& value : m_ebv_values) {
    ebv_value_vector.push_back(nonLocalizedString(value));
  }
  if (ebv_value_vector.size() > 0) {
    options["ebv-value" + postfix].value() = boost::any(ebv_value_vector);
  }

  return options;
}

long long ParameterRule::getModelNumber(DatasetRepo sed_repository, DatasetRepo redenig_curves_repository, bool recompute) {

  if (m_model_number < 0 || recompute) {
	  long long sed_number_map = getSedNumber(sed_repository).first;
	  long long red_number_map = getRedCurveNumber(redenig_curves_repository).first;

	  long long z_number = getRedshiftValues().size();
	  for (const auto& range : getZRanges()) {
		z_number += (range.getMax()-range.getMin())/range.getStep() +1;
	  }

	  long long ebv_number = getEbvValues().size();
	 	  for (const auto& range : getEbvRanges()) {
	 		 ebv_number += (range.getMax()-range.getMin())/range.getStep() +1;
	 	  }
	  m_model_number = sed_number_map * red_number_map * z_number * ebv_number;
	/*
    bool is_zero = false;
    auto options = getConfigOptions("");

    is_zero |= m_sed_selection.isEmpty();
    is_zero |= m_red_curve_selection.isEmpty();
    is_zero |= m_redshift_ranges.size() == 0 && m_redshift_values.size() == 0;
    is_zero |= m_ebv_ranges.size() == 0 && m_ebv_values.size() == 0;

    if (is_zero) {
      m_model_number = 0;
    } else {
      options["sed-root-path"].value()             = boost::any(FileUtils::getSedRootPath(false));
      options["reddening-curve-root-path"].value() = boost::any(FileUtils::getRedCurveRootPath(false));
      completeWithDefaults<PhzConfiguration::ParameterSpaceConfig>(options);
      long  config_manager_id = Configuration::getUniqueManagerId();
      auto& config_manager    = Configuration::ConfigManager::getInstance(config_manager_id);

      try {
        config_manager.registerConfiguration<PhzConfiguration::ParameterSpaceConfig>();
        config_manager.closeRegistration();
        config_manager.initialize(options);

        m_model_number =
            config_manager.getConfiguration<PhzConfiguration::SedConfig>().getSedList().at("").size() *
            config_manager.getConfiguration<PhzConfiguration::ReddeningConfig>().getReddeningCurveList().at("").size() *
            config_manager.getConfiguration<PhzConfiguration::ReddeningConfig>().getEbvList().at("").size() *
            config_manager.getConfiguration<PhzConfiguration::RedshiftConfig>().getZList().at("").size();
      } catch (Elements::Exception&) {
        m_model_number = 0;
      }
    }*/
  }
  return m_model_number;
}

long long ParameterRule::getModelNumber(DatasetRepo sed_repository, DatasetRepo redenig_curves_repository) const {
	long long sed_number_map = getSedNumber(sed_repository).first;
	long long red_number_map = getRedCurveNumber(redenig_curves_repository).first;

	long long z_number = getRedshiftValues().size();
		for (const auto& range : getZRanges()) {
			z_number += (range.getMax()-range.getMin())/range.getStep() +1;
	}

	long long ebv_number = getEbvValues().size();
		for (const auto& range : getEbvRanges()) {
			ebv_number += (range.getMax()-range.getMin())/range.getStep() +1;
	}
	return sed_number_map * red_number_map * z_number * ebv_number;
	/*
	bool is_zero = false;
  is_zero |= m_sed_selection.isEmpty();
  is_zero |= m_red_curve_selection.isEmpty();
  is_zero |= m_redshift_ranges.size() == 0 && m_redshift_values.size() == 0;
  is_zero |= m_ebv_ranges.size() == 0 && m_ebv_values.size() == 0;
  if (is_zero) {
    return 0;
  } else {
    long long number                             = 0;
    auto      options                            = getConfigOptions("");
    options["sed-root-path"].value()             = boost::any(FileUtils::getSedRootPath(false));
    options["reddening-curve-root-path"].value() = boost::any(FileUtils::getRedCurveRootPath(false));
    completeWithDefaults<PhzConfiguration::ParameterSpaceConfig>(options);
    long  config_manager_id = Configuration::getUniqueManagerId();
    auto& config_manager    = Configuration::ConfigManager::getInstance(config_manager_id);

    try {
      config_manager.registerConfiguration<PhzConfiguration::ParameterSpaceConfig>();
      config_manager.closeRegistration();
      config_manager.initialize(options);
      number =
          config_manager.getConfiguration<PhzConfiguration::SedConfig>().getSedList().at("").size() *
          config_manager.getConfiguration<PhzConfiguration::ReddeningConfig>().getReddeningCurveList().at("").size() *
          config_manager.getConfiguration<PhzConfiguration::ReddeningConfig>().getEbvList().at("").size() *
          config_manager.getConfiguration<PhzConfiguration::RedshiftConfig>().getZList().at("").size();
    } catch (Elements::Exception&) {
      number = 0;
    }

    return number;
  }

  */
}

const std::set<double>& ParameterRule::getEbvValues() const {
  return m_ebv_values;
}

void ParameterRule::setEbvValues(std::set<double> values) {
  m_ebv_values = move(values);
}

const std::string ParameterRule::getEbvRangeString() const {
  bool is_zero = false;
  auto options = getConfigOptions("");

  is_zero |= m_red_curve_selection.isEmpty();
  is_zero |= m_ebv_ranges.size() == 0 && m_ebv_values.size() == 0;

  if (is_zero) {
    return "";
  }

  options["reddening-curve-root-path"].value() = boost::any(FileUtils::getRedCurveRootPath(false));
  completeWithDefaults<PhzConfiguration::ReddeningConfig>(options);
  long  config_manager_id = Configuration::getUniqueManagerId();
  auto& config_manager    = Configuration::ConfigManager::getInstance(config_manager_id);
  config_manager.registerConfiguration<PhzConfiguration::ReddeningConfig>();
  config_manager.closeRegistration();
  config_manager.initialize(options);

  auto axis = config_manager.getConfiguration<PhzConfiguration::ReddeningConfig>().getEbvList().at("");

  return getAxisStringValue(axis);
}

const std::string ParameterRule::getRedshiftRangeString() const {
  bool is_zero = false;
  auto options = getConfigOptions("");
  is_zero |= m_redshift_ranges.size() == 0 && m_redshift_values.size() == 0;

  if (is_zero) {
    return "";
  }

  completeWithDefaults<PhzConfiguration::RedshiftConfig>(options);
  long  config_manager_id = Configuration::getUniqueManagerId();
  auto& config_manager    = Configuration::ConfigManager::getInstance(config_manager_id);
  config_manager.registerConfiguration<PhzConfiguration::RedshiftConfig>();
  config_manager.closeRegistration();
  config_manager.initialize(options);

  std::vector<double> axis = config_manager.getConfiguration<PhzConfiguration::RedshiftConfig>().getZList().at("");

  return getAxisStringValue(axis);
}

std::string ParameterRule::getAxisStringValue(std::vector<double> axis) const {
  auto min  = *(axis.begin());
  auto max  = *(--(axis.end()));
  auto size = axis.size();

  if (size == 1) {
    return QString::number(min, 'g', 2).toStdString();
  }

  if (size == 2) {
    return QString::number(min, 'g', 2).toStdString() + ", " + QString::number(max, 'g', 2).toStdString();
  }

  return "[" + QString::number(min, 'g', 2).toStdString() + ", " + QString::number(max, 'g', 2).toStdString() + "] #" +
         nonLocalizedString(size);
}

const std::set<double>& ParameterRule::getRedshiftValues() const {
  return m_redshift_values;
}

void ParameterRule::setRedshiftValues(std::set<double> values) {
  m_redshift_values = move(values);
}

const std::vector<Range>& ParameterRule::getEbvRanges() const {
  return m_ebv_ranges;
}
const std::vector<Range>& ParameterRule::getZRanges() const {
  return m_redshift_ranges;
}

void ParameterRule::setEbvRanges(std::vector<Range> ebv_ranges) {
  m_ebv_ranges = move(ebv_ranges);
}

void ParameterRule::setZRanges(std::vector<Range> z_ranges) {
  m_redshift_ranges = move(z_ranges);
}

void ParameterRule::setRedCurveSelection(DatasetSelection red_curve_selection) {
  m_red_curve_selection = std::move(red_curve_selection);
}

void ParameterRule::setSedSelection(DatasetSelection sed_selection) {
  m_sed_selection = std::move(sed_selection);
}

const DatasetSelection& ParameterRule::getRedCurveSelection() const {
  return m_red_curve_selection;
}

const DatasetSelection& ParameterRule::getSedSelection() const {
  return m_sed_selection;
}

}  // namespace PhzQtUI
}  // namespace Euclid
