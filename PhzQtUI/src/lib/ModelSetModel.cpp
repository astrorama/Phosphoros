#include "PhzQtUI/ModelSetModel.h"
#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include "PhzQtUI/ModelSet.h"
#include "PreferencesUtils.h"
#include <dirent.h>
#include <iostream>

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("ModelSetModel");

ModelSetModel::ModelSetModel(DatasetRepo sed_repo, DatasetRepo red_repo) : QStandardItemModel(),
		m_sed_repo{sed_repo},m_red_repo{red_repo}, m_edited_modelSet{m_sed_repo, m_red_repo}{}

const QString ModelSetModel::getValue(int row, int column) const {
  return this->item(row, column)->text();
}

bool ModelSetModel::checkUniqueName(QString new_name, int row) const {
  // TODO Complete by a check that the name can be a name of a file (no reserved char)
  int ref = -1;
  if (row >= 0) {
    ref = getValue(row, 2).toInt();
  }
  for (auto it = m_set_list.begin(); it != m_set_list.end(); ++it) {
    if (it->first == ref) {
      continue;
    }
    if (new_name.toStdString().compare(it->second.getName()) == 0) {
      return false;
    }
  }
  return true;
}

std::string ModelSetModel::getDuplicateName(std::string name) const {
  auto new_name = name + "_Copy";
  if (checkUniqueName(QString::fromStdString(new_name), -1)) {
    return new_name;
  }

  int i = 2;
  while (!checkUniqueName(QString::fromStdString(new_name + "(" + std::to_string(i) + ")"), -1)) {
    ++i;
  }

  return new_name + "(" + std::to_string(i) + ")";
}

void ModelSetModel::loadSets() {
  m_set_list = ModelSet::loadModelSetsFromFolder(m_sed_repo, m_red_repo, FileUtils::getModelRootPath(true));

  this->setColumnCount(3);
  this->setRowCount(m_set_list.size());
  QStringList setHeaders;
  setHeaders << "Name"
             << "Total Size"
             << "Hidden_Id";
  this->setHorizontalHeaderLabels(setHeaders);

  int i = 0;
  for (auto it = m_set_list.begin(); it != m_set_list.end(); ++it) {
    this->setItem(i, 0, new QStandardItem(QString::fromStdString(it->second.getName())));
    this->setItem(i, 1, new QStandardItem(QString::number(it->second.getModelNumber())));
    this->setItem(i, 2, new QStandardItem(QString::number(it->first)));
    ++i;
  }

  auto saved_modelset = PreferencesUtils::getUserPreference("_global_selection_", "parameter_space");
  selectModelSet(QString::fromStdString(saved_modelset));
}

void ModelSetModel::selectModelSet(int row) {
  if (row >= 0 && row < static_cast<int>(m_set_list.size())) {
    m_selected_row    = row;
    m_selected_index  = getValue(row, 2).toInt();
    const auto& current_model = m_set_list.find(m_selected_index)->second;
    m_edited_modelSet = ModelSet{current_model};
    PreferencesUtils::setUserPreference("_global_selection_", "parameter_space", m_edited_modelSet.getName());
  } else {
    m_selected_row    = -1;
    m_selected_index  = -1;
    m_edited_modelSet = ModelSet(m_sed_repo, m_red_repo);
  }
  m_in_edition = false;
}

void ModelSetModel::selectModelSet(QString name) {
  int selected_row = -1;
  for (int row = 0; row < static_cast<int>(m_set_list.size()); ++row) {
    if (getValue(row, 0).compare(name) == 0) {
      selected_row = row;
      break;
    }
  }
  selectModelSet(selected_row);
}

ModelSet& ModelSetModel::getSelectedModelSet() {
  return m_edited_modelSet;
}

int ModelSetModel::getSelectedRow() const {
  return m_selected_row;
}

void ModelSetModel::newModelSet(bool duplicate_from_selected) {
  // get the new ref
  int max_ref = 0;
  for (auto it = m_set_list.begin(); it != m_set_list.end(); ++it) {
    if (it->first > max_ref) {
      max_ref = it->first;
    }
  }
  ++max_ref;

  QString text_1      = "New_Parameter_Space";
  QString text_2      = "0";
  auto model_set = ModelSet(m_sed_repo, m_red_repo, FileUtils::getModelRootPath(true));
  model_set.setParameterRules(std::map<int, ParameterRule>{});


  if (duplicate_from_selected) {
    text_1 = QString::fromStdString(getDuplicateName(m_edited_modelSet.getName()));
    text_2 = QString::number(m_edited_modelSet.getModelNumber(false));
    model_set.setParameterRules(m_edited_modelSet.getParameterRules());

    model_set.setZRange(m_edited_modelSet.getZRanges());
    model_set.setZValues(m_edited_modelSet.getZValues());
    model_set.setEbvRange(m_edited_modelSet.getEbvRanges());
    model_set.setEbvValues(m_edited_modelSet.getEbvValues());
  }
  model_set.setName(text_1.toStdString());

  m_set_list.emplace(max_ref, model_set);

  QList<QStandardItem*> items;
  items.push_back(new QStandardItem(text_1));
  items.push_back(new QStandardItem(text_2));
  items.push_back(new QStandardItem(QString::number(max_ref)));
  this->appendRow(items);

  selectModelSet(items[0]->row());
}

void ModelSetModel::deleteSelected() {
  if (m_selected_row >= 0) {
    logger.info() << "Deleting the selected Model '" << m_edited_modelSet.getName() << "'.";
    m_set_list.at(m_selected_index).deleteModelSet();
    m_set_list.erase(m_selected_index);
    this->removeRow(m_selected_row);
    selectModelSet(-1);
    m_in_edition  = false;
    m_need_reload = true;
  }
}

bool ModelSetModel::isInEdition() {
  return m_in_edition;
}

bool ModelSetModel::saveSelected() {

  bool pre_tests = checkUniqueName(QString::fromStdString(m_edited_modelSet.getName()), m_selected_row);

  if (pre_tests) {
	auto old_name = m_set_list.find(m_selected_index)->second.getName();
	auto new_name =  m_edited_modelSet.getName();
	logger.info() << "Model old name '" << old_name <<
			         "' saved to new name '" <<  new_name << "'";
    m_edited_modelSet.saveModelSet(old_name);

    this->setItem(m_selected_row, 0, new QStandardItem(QString::fromStdString(new_name)));
    this->setItem(m_selected_row, 1, new QStandardItem(QString::number(m_edited_modelSet.getModelNumber(true))));
    m_set_list.find(m_selected_index)->second = m_edited_modelSet;

    m_in_edition  = false;
    m_need_reload = true;
    return true;
  } else {
    return false;
  }
}

void ModelSetModel::cancelSelected() {
  logger.info() << "Cancel edition on the selected Model '" << m_edited_modelSet.getName() << "'.";

  const auto& edited_model = m_set_list.find(m_selected_index)->second;
  m_edited_modelSet = ModelSet{edited_model};
  m_in_edition      = false;
}

const std::vector<QString> ModelSetModel::getModelSetList() const {
  std::vector<QString> result = {};
  for (auto it = m_set_list.begin(); it != m_set_list.end(); ++it) {
    result.push_back(QString::fromStdString(it->second.getName()));
  }
  return result;
}

void ModelSetModel::setNameToSelected(const QString& value) {
  m_edited_modelSet.setName(value.toStdString());
  m_in_edition = true;
}

void ModelSetModel::setParameterRulesToSelected(const std::map<int, ParameterRule>& value) {
  m_edited_modelSet.setParameterRules(value);
  m_in_edition = true;
}

void ModelSetModel::setGlobalRedshiftRangeToSelected(const std::vector<Range>& ranges, const std::set<double>& values) {
  m_edited_modelSet.setZRange(ranges);
  m_edited_modelSet.setZValues(values);
  m_in_edition = true;
}

void ModelSetModel::setGlobalEbvRangeToSelected(const std::vector<Range>& ranges, const std::set<double>& values) {
  m_edited_modelSet.setEbvRange(ranges);
  m_edited_modelSet.setEbvValues(values);
  m_in_edition = true;
}

bool ModelSetModel::doNeedReload() const {
  return m_need_reload;
}

void ModelSetModel::reloaded() {
   m_need_reload = false;
}


}  // namespace PhzQtUI
}  // namespace Euclid
