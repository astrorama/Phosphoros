#include "PhzQtUI/SedTreeModel.h"
#include "FileUtils.h"
#include "XYDataset/QualifiedName.h"
#include <QDir>
#include <QFile>
#include <QPushButton>
#include <exception>
#include <iostream>
#include <set>

#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"

#include "ElementsKernel/Logging.h"

using namespace std;
namespace Euclid {
namespace PhzQtUI {

SedTreeModel::SedTreeModel(DatasetRepo repository, QObject* parent)
    : QStandardItemModel(parent), m_repository(repository) {}

std::string SedTreeModel::getGroupName(XYDataset::QualifiedName qualified_name) {
  auto        separator  = QString(QDir::separator()).toStdString();
  std::string group_name = FileUtils::removeExt(qualified_name.qualifiedName(), qualified_name.datasetName());
  if (group_name.length()>0 && group_name[group_name.length() - 1] == separator[0]) {
    group_name = group_name.substr(0, group_name.length() - 1);
  }
  return group_name;
}

void SedTreeModel::load(bool selectable, bool onlyLeaves) {
  m_map_dir.clear();
  auto& unordered = m_repository->getContent();

  std::vector<std::set<std::string>> group_sets{};

  for (auto& name : unordered) {
    std::string group_name = SedTreeModel::getGroupName(name);
    if (group_name.empty()) {
      continue;
    }
    size_t depth = std::count(group_name.begin(), group_name.end(), '/');
    for (auto i = group_sets.size(); i <= depth; ++i) {
      group_sets.emplace_back();
    }
    group_sets[depth].insert(group_name);
  }

  // create the groups
  std::set<std::string> parent_group_set{};
  for (auto& group_set : group_sets) {
    for (auto& group : group_set) {
      auto        group_qualifiedName = XYDataset::QualifiedName(group);
      std::string parent_group        = "";

      for (auto& possible_parent : parent_group_set) {
        auto parent_qualifiedName = XYDataset::QualifiedName(possible_parent);
        if (group_qualifiedName.belongsInGroup(parent_qualifiedName)) {
          parent_group = possible_parent;
          break;
        }
      }

      QStandardItem* item = new QStandardItem(QString::fromStdString(group_qualifiedName.datasetName()));
      item->setBackground(QBrush(QColor(230, 230, 230)));
      item->setCheckable(selectable && !onlyLeaves);
      item->setAutoTristate(selectable && !onlyLeaves);

      QStandardItem* item_void  = new QStandardItem("");
      QStandardItem* item_void2 = new QStandardItem("");
      if (parent_group.length() > 0) {
        m_map_dir.at(parent_group)->appendRow({item, item_void, item_void2});
      } else {
        this->appendRow({item, item_void, item_void2});
      }

      m_map_dir[group] = item;
    }
    parent_group_set = group_set;
  }

  // put the items
  for (auto& name : unordered) {

    QStandardItem* item = new QStandardItem(QString::fromStdString(name.datasetName()));
    item->setCheckable(selectable);
    auto group_name = SedTreeModel::getGroupName(name);
    if (group_name.length() > 0) {
      m_map_dir.at(group_name)->appendRow(item);
    } else {
      this->appendRow(item);
    }
  }
}

bool SedTreeModel::canOpenInfo(QStandardItem* item) const {
  return item->rowCount() == 0;
}

bool SedTreeModel::canAddEmissionLineToGroup(QStandardItem* item) const {
  if (item->rowCount() > 0) {
    QString name = getFullGroupName(item);

    if (name.endsWith("_el") || name.endsWith("_lpel")) {
      return false;
    }

    for (auto& group : m_map_dir) {
      if ((name + "_el") == QString::fromStdString(group.first)) {
        return false;
      }
    }

    for (int i = 0; i < item->rowCount(); ++i) {
      auto sub_item = item->child(i);
      if (sub_item->text().endsWith("_el") || sub_item->text().endsWith("_lpel")) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool SedTreeModel::canAddLpEmissionLineToGroup(QStandardItem* item) const {
  if (item->rowCount() > 0) {
    QString name = getFullGroupName(item);

    if (name.endsWith("_el") || name.endsWith("_lpel")) {
      return false;
    }

    for (auto& group : m_map_dir) {
      if ((name + "_lpel") == QString::fromStdString(group.first)) {
        return false;
      }
    }

    for (int i = 0; i < item->rowCount(); ++i) {
      auto sub_item = item->child(i);
      if (sub_item->text().endsWith("_el") || sub_item->text().endsWith("_lpel")) {
        return false;
      }
    }

    return true;
  }

  return false;
}

QString SedTreeModel::getFullGroupName(QStandardItem* item) const {
  if (item->rowCount() > 0) {
    for (auto& pair : m_map_dir) {
      if (pair.second == item) {
        return QString::fromStdString(pair.first);
      }
    }
  } else {
    for (auto& pair : m_map_dir) {
      if (pair.second == item->parent()) {
        return QString::fromStdString(pair.first);
      }
    }
  }

  return "";
}

void SedTreeModel::setEditionStatus(bool inEdition) {
  m_in_edition = inEdition;
}

void SedTreeModel::setEnabled(bool enable) {
  // root level
  for (int i = 0; i < this->rowCount(); ++i) {
    this->item(i)->setEnabled(enable);
  }

  // groups and their direct children
  for (auto it : m_map_dir) {
    it.second->setEnabled(enable);
    for (int i = 0; i < it.second->rowCount(); ++i) {
      it.second->child(i)->setEnabled(enable);
    }
  }

  setEditionStatus(enable);
}

void SedTreeModel::onItemChanged(QStandardItem* item) {
  if (m_in_edition) {

    auto item_state = item->checkState();
    if (item_state != Qt::CheckState::PartiallyChecked) {
      // Check/uncheck the children
      for (int i = 0; i < item->rowCount(); ++i) {
        if (item->child(i)->isCheckable()) {
          item->child(i)->setCheckState(item_state);
        }
      }
    }

    // look for the parent state
    if (item->parent()) {
      bool has_checked   = false;
      bool has_partial   = false;
      bool has_unchecked = false;
      for (int i = 0; i < item->parent()->rowCount(); ++i) {
        auto child_state = item->parent()->child(i)->checkState();
        has_checked |= child_state == Qt::CheckState::Checked;
        has_partial |= child_state == Qt::CheckState::PartiallyChecked;
        has_unchecked |= child_state == Qt::CheckState::Unchecked;
      }

      if (has_partial | (has_checked && has_unchecked)) {
        if (item->parent()->isCheckable()) {
          item->parent()->setCheckState(Qt::CheckState::PartiallyChecked);
        }
      } else if (has_checked && !has_partial && !has_unchecked) {
        if (item->parent()->isCheckable()) {
          item->parent()->setCheckState(Qt::CheckState::Checked);
        }
      } else if (has_unchecked && !has_partial && !has_checked) {
        if (item->parent()->isCheckable()) {
          item->parent()->setCheckState(Qt::CheckState::Unchecked);
        }
      }
    }
  }
}

void SedTreeModel::onItemChangedSingleLeaf(QStandardItem* item) {
  if (m_in_edition && !m_bypass) {
    m_bypass = true;
    if (item->checkState() == Qt::CheckState::Checked) {
      if (item->rowCount() == 0) {
        clearState();
      }
      item->setCheckState(Qt::CheckState::Checked);
      m_bypass = false;

    } else {
      clearState();
      m_bypass = false;
    }

  } else {
    onItemChanged(item);
  }
}

void SedTreeModel::clearState() {
  for (auto& group : m_map_dir) {
    if (group.second->isCheckable()) {
      group.second->setCheckState(Qt::CheckState::Unchecked);
    }
    for (int i = 0; i < group.second->rowCount(); ++i) {
      auto sub_item = group.second->child(i);
      if (sub_item->isCheckable()) {
        sub_item->setCheckState(Qt::CheckState::Unchecked);
      }
    }
  }

  for (int i = 0; i < this->rowCount(); ++i) {
    auto root_level_item = this->item(i);
    if (root_level_item->isCheckable()) {
      root_level_item->setCheckState(Qt::CheckState::Unchecked);
    }
  }
}

void SedTreeModel::checkGroup(XYDataset::QualifiedName name) {
  for (auto& group : m_map_dir) {
    if (XYDataset::QualifiedName(group.first).belongsInGroup(name) || name.qualifiedName() == group.first) {
      if (group.second->isCheckable()) {
        group.second->setCheckState(Qt::CheckState::Checked);
      }
      for (int i = 0; i < group.second->rowCount(); ++i) {
        auto sub_item = group.second->child(i);
        if (sub_item->isCheckable()) {
          sub_item->setCheckState(Qt::CheckState::Checked);
        }
      }
    }
  }
}

void SedTreeModel::partialCheckParentGroups(XYDataset::QualifiedName child) {
  for (auto& group : m_map_dir) {
    if (child.belongsInGroup(XYDataset::QualifiedName(group.first))) {
      if (group.second->isCheckable()) {
        group.second->setCheckState(Qt::CheckState::PartiallyChecked);
      }
    }
  }
}

void SedTreeModel::setState(const DatasetSelection& selection) {
  clearState();

  // groupes
  for (auto& group : selection.getGroupes()) {
    checkGroup(XYDataset::QualifiedName(group));
  }

  // isolated
  for (auto& lone : selection.getIsolated()) {
    for (int i = 0; i < this->rowCount(); ++i) {
      auto root_level_item = this->item(i);
      if (root_level_item->text().toStdString() == lone && root_level_item->rowCount() == 0) {
        if (root_level_item->isCheckable()) {
          root_level_item->setCheckState(Qt::CheckState::Checked);
        }
      }
    }
  }

  // exclusions
  for (auto& exclusion : selection.getExclusions()) {
    XYDataset::QualifiedName exclusion_qualified_name(exclusion);
    partialCheckParentGroups(exclusion_qualified_name);
    auto group_name = SedTreeModel::getGroupName(exclusion_qualified_name);
    auto item_name  = exclusion_qualified_name.datasetName();
    if (m_map_dir.count(group_name) == 1) {
      for (int i = 0; i < m_map_dir.at(group_name)->rowCount(); ++i) {
        auto item = m_map_dir.at(group_name)->child(i);
        if (item->text().toStdString() == item_name && item->rowCount() == 0) {
          if (item->isCheckable()) {
            item->setCheckState(Qt::CheckState::Unchecked);
          }
        }
      }
    }
  }
}

void SedTreeModel::setState(const std::vector<std::string>& selected_leaves) {
  for (const std::string& leaf : selected_leaves) {
    auto group_name = SedTreeModel::getGroupName(leaf);
    for (int i = 0; i < m_map_dir.at(group_name)->rowCount(); ++i) {
      auto        item = m_map_dir.at(group_name)->child(i);
      std::string name = item->text().toStdString();
      if (std::equal(name.rbegin(), name.rend(), leaf.rbegin())) {
        if (item->isCheckable()) {
          item->setCheckState(Qt::CheckState::Checked);
        }
      }
    }
  }
}

std::vector<std::string> SedTreeModel::getSelectedLeaves() const {
  std::vector<std::string> leaves{};
  // inside group
  auto separator = QString(QDir::separator()).toStdString();
  for (auto& group_tuple : m_map_dir) {
    auto group_item = group_tuple.second;
    auto base_name  = group_tuple.first;
    for (int i = 0; i < group_item->rowCount(); ++i) {
      auto child_item = group_item->child(i);
      if (child_item->rowCount() == 0 && child_item->checkState() == Qt::CheckState::Checked) {
        leaves.push_back(base_name + separator + child_item->text().toStdString());
      }
    }
  }

  // root level
  for (int i = 0; i < this->rowCount(); ++i) {
    auto root_level_item = this->item(i);
    if (root_level_item->checkState() == Qt::CheckState::Checked && root_level_item->rowCount() == 0) {
      leaves.push_back(root_level_item->text().toStdString());
    }
  }

  return leaves;
}

DatasetSelection SedTreeModel::getState() const {
  auto             separator = QString(QDir::separator()).toStdString();
  DatasetSelection current_state;

  // groupes
  std::vector<std::string> groups{};
  for (auto& group_tuple : m_map_dir) {
    if (group_tuple.second->checkState() != Qt::CheckState::Unchecked) {
      groups.push_back(group_tuple.first);
    }
  }

  // isolated
  std::vector<std::string> isolated{};
  for (int i = 0; i < this->rowCount(); ++i) {
    auto root_level_item = this->item(i);
    if (root_level_item->checkState() == Qt::CheckState::Checked && root_level_item->rowCount() == 0) {
      isolated.push_back(root_level_item->text().toStdString());
    }
  }

  // exclusions
  std::vector<std::string> exclusions{};
  for (auto& group_tuple : m_map_dir) {
    auto group_item = group_tuple.second;
    auto base_name  = group_tuple.first;

    bool                     has_selected_parent = false;
    XYDataset::QualifiedName group_qualified_name(base_name);
    for (auto& added_group : groups) {
      XYDataset::QualifiedName parent_group(added_group);
      if (group_qualified_name.belongsInGroup(parent_group)) {
        has_selected_parent = true;
        break;
      }
    }
    if (group_item->checkState() == Qt::CheckState::PartiallyChecked ||
        (group_item->checkState() == Qt::CheckState::Unchecked && has_selected_parent)) {
      for (int i = 0; i < group_item->rowCount(); ++i) {
        auto child_item = group_item->child(i);
        if (child_item->rowCount() == 0 && child_item->checkState() == Qt::CheckState::Unchecked) {
          exclusions.push_back(base_name + separator + child_item->text().toStdString());
        }
      }
    }
  }

  current_state.setGroupes(groups);
  current_state.setIsolated(isolated);
  current_state.setExclusions(exclusions);

  return current_state;
}

bool SedTreeModel::hasLeave() const {
  return this->rowCount() > 0;
}

}  // namespace PhzQtUI
}  // namespace Euclid
