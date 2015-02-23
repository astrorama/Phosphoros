#include <iostream>
#include <exception>
#include <QFile>
#include <QDir>
#include <set>
#include "QtUI/FileUtils.h"
#include "QtUI/XYDataSetTreeModel.h"
#include "XYDataset/QualifiedName.h"

#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/AsciiParser.h"

#include "ElementsKernel/Logging.h"

using namespace std;
using namespace Euclid;

XYDataSetTreeModel::XYDataSetTreeModel(QObject *parent) :
    QStandardItemModel(parent) {
}

std::string XYDataSetTreeModel::getRelPath(std::string path) const {
  return Euclid::PhosphorosUiDm::FileUtils::removeStart(
      Euclid::PhosphorosUiDm::FileUtils::removeStart(path, m_root_dir),
      QString(QDir::separator()).toStdString());
}

std::string XYDataSetTreeModel::getFullPath(std::string path) const {
  if (Euclid::PhosphorosUiDm::FileUtils::starts_with(path, m_root_dir)) {
    return path;
  }

  return m_root_dir + QString(QDir::separator()).toStdString() + path;
}

void XYDataSetTreeModel::loadDirectory(std::string rootPath,
    bool singleLeafSelection, std::string rootDisplayName) {
  this->setColumnCount(1);
  QStandardItem* root_item = new QStandardItem(
      QString::fromStdString(rootDisplayName));
  root_item->setCheckable(!singleLeafSelection);
  root_item->setBackground(QBrush(QColor(220, 220, 220)));
  this->appendRow(root_item);
  m_map_dir.clear();
  m_map_dir[""] = root_item;

  std::unique_ptr < XYDataset::FileParser > file_parser {new XYDataset::AsciiParser { } };
  XYDataset::FileSystemProvider provider { rootPath, std::move(file_parser) };

  auto unordered = provider.listContents("");
  std::set < XYDataset::QualifiedName, XYDataset::QualifiedName::AlphabeticalComparator
      > ordered_set { unordered.begin(), unordered.end() };

  std::map<std::string, bool> m_structure;
  std::set < std::string > group_set { };

  for (auto& name : ordered_set) {
    std::string parent_group = "";
    for (auto group : name.groups()) {
      std::string current_group = parent_group
          + QString(QDir::separator()).toStdString() + group;
      current_group = PhosphorosUiDm::FileUtils::removeStart(current_group,
          QString(QDir::separator()).toStdString());

      if (group_set.count(current_group) == 0) {
        m_structure[current_group] = true;
        group_set.insert(current_group);
      }
      parent_group = current_group;
    }
    m_structure[name.qualifiedName()] = false;
  }

  for (auto structure_item : m_structure) {
    std::string name = structure_item.first;
    bool is_dir = structure_item.second;
    std::string group = "";

    std::string::size_type n = name.rfind(
        QString(QDir::separator()).toStdString());
    if (n != std::string::npos) {
      group = name.substr(0, n);
    }

    QStandardItem* item = new QStandardItem(QString::fromStdString(name));
    item->setCheckable(!is_dir || !singleLeafSelection);
    if (is_dir) {
      item->setBackground(QBrush(QColor(230, 230, 230)));
    }

    m_map_dir.at(group)->appendRow(item);
    if (is_dir) {
      m_map_dir[name] = item;
    }
  }
}

void XYDataSetTreeModel::setEnabled(bool enable) {
  for (auto it : m_map_dir) {
    it.second->setEnabled(enable);
    for (int i = 0; i < it.second->rowCount(); ++i) {
      it.second->child(i)->setEnabled(enable);
    }
  }

  setEditionStatus(enable);
}

void XYDataSetTreeModel::checkDir(bool checked, std::string dir,
    std::list<std::string> exclusions) {

  if (dir.compare(".") == 0
      || dir.compare(this->item(0)->text().toStdString()) == 0) {
    dir = "";
  }

  Qt::CheckState checked_status = Qt::CheckState::Unchecked;
  if (checked) {
    checked_status = Qt::CheckState::Checked;
  }

  auto root = m_map_dir.at(dir);
  if (std::find(exclusions.begin(), exclusions.end(),
      root->text().toStdString()) == exclusions.end()) {
    if (root->isCheckable()) {
      root->setCheckState(checked_status);
    }
    for (int i = 0; i < root->rowCount(); ++i) {
      auto child = root->child(i);
      if (std::find(exclusions.begin(), exclusions.end(),
          child->text().toStdString()) == exclusions.end()) {
        if (child->isCheckable()) {
          child->setCheckState(checked_status);
        }

        if (child->hasChildren()) {
          checkDir(checked, child->text().toStdString(), exclusions);
        }
      }
    }
  }
}

void XYDataSetTreeModel::setEditionStatus(bool inEdition) {
  m_in_edition = inEdition;
}

void XYDataSetTreeModel::selectRoot() {
  this->item(0)->setCheckState(Qt::CheckState::Checked);
}

void XYDataSetTreeModel::onItemChangedSingleLeaf(QStandardItem* item) {
  if (m_in_edition && !m_bypass_item_changed) {
    m_bypass_item_changed = true;
    if (item->checkState() == Qt::CheckState::Checked) {
      checkDir(false, m_root_dir);
      item->setCheckState(Qt::CheckState::Checked);
    }
    m_bypass_item_changed = false;
  }
}

void XYDataSetTreeModel::onItemChangedUniqueSelection(QStandardItem* item) {
  if (m_in_edition && !m_bypass_item_changed) {
    m_bypass_item_changed = true;
    if (item->checkState() == Qt::CheckState::Checked) {
      checkDir(false, m_root_dir);
      item->setCheckState(Qt::CheckState::Checked);
    } else {
      this->item(0)->setCheckState(Qt::CheckState::Checked);
    }
    m_bypass_item_changed = false;
  }
}

void XYDataSetTreeModel::onItemChanged(QStandardItem* item) {
  if (m_in_edition && !m_bypass_item_changed) {
    m_bypass_item_changed = true;

    bool checked = item->checkState() == Qt::CheckState::Checked;

    if (item->parent() && item->parent()->checkState() > 0) {
      // parent checked: exclusion
      if (item->hasChildren()) {
        checkDir(checked, item->text().toStdString());
      }
    } else {
      // inclusion
      checkDir(false, m_root_dir);
      if (checked) {
        if (item->hasChildren()) {
          checkDir(true, item->text().toStdString());
        } else {
          item->setCheckState(Qt::CheckState::Checked);
        }
      }
    }

    m_bypass_item_changed = false;
  }
}

void XYDataSetTreeModel::setState(std::string root,
    const std::list<std::string>& exclusions) {
  m_bypass_item_changed = true;

  checkDir(false, "");


  if (root.compare("") == 0 ||root.compare(".") == 0 || root.compare(item(0)->text().toStdString()) == 0
      || root.compare(m_root_dir) == 0) {
    if (item(0)->isCheckable()){
      checkDir(true, root, exclusions);
    }
  } else {
    root = getFullPath(root);

    if (m_map_dir.count(root)) {
      checkDir(true, root, exclusions);
    } else {
      std::list<QStandardItem *> item_to_explore { this->item(0) };

      while (item_to_explore.size() > 0) {
        auto item = item_to_explore.front();

        for (int i = 0; i < item->rowCount(); ++i) {
          auto child = item->child(i);
          if (child->text().toStdString().compare(getRelPath(root)) == 0) {
            child->setCheckState(Qt::CheckState::Checked);
            break;
          }

          if (child->hasChildren()) {
            item_to_explore.push_back(child);
          }
        }

        item_to_explore.remove(item);
      }
    }
  }

  m_bypass_item_changed = false;
}

std::pair<bool, std::string> XYDataSetTreeModel::getRootSelection(
    std::string from) const {

  auto root_item = item(0);
  if (from.compare(".") == 0 || from.compare(item(0)->text().toStdString()) == 0
      || from.compare(m_root_dir) == 0) {
    if (root_item->checkState() == Qt::CheckState::Checked) {
      return std::make_pair(true, m_root_dir);
    }
  } else {
    from = getFullPath(from);
    root_item = m_map_dir.at(from);
  }

  if (root_item->checkState() == Qt::CheckState::Checked) {
    return std::make_pair(true, root_item->text().toStdString());
  }

  for (int i = 0; i < root_item->rowCount(); ++i) {
    auto child = root_item->child(i);
    if (child->checkState() == Qt::CheckState::Checked) {
      return std::make_pair(true, child->text().toStdString());
    }

    if (child->hasChildren()) {
      auto res = getRootSelection(child->text().toStdString());
      if (res.first) {
        return res;
      }
    }
  }

  return std::make_pair(false, "");
}

std::string XYDataSetTreeModel::getGroup() const {
  auto res = getRootSelection();
  if (!res.first) {
    return "";
  }

  if (m_map_dir.count(res.second) == 1 || res.second.compare(m_root_dir) == 0) {
    return res.second;
  }

  int index = res.second.find_last_of("/", std::string::npos);
  if (index >= 0) {
    return res.second.substr(0, index);
  } else {
    return "";
  }
}

std::list<std::string> XYDataSetTreeModel::XYDataSetTreeModel::getExclusions(
    std::string root) const {
  std::list < std::string > list;

  auto root_item = item(0);
  if (root.compare(".") != 0 && root.compare(item(0)->text().toStdString()) != 0
      && root.compare(m_root_dir) != 0) {
    root = getFullPath(root);
    if (m_map_dir.count(root)) {
      root_item = m_map_dir.at(root);
    } else {
      return list;
    }
  }

  for (int i = 0; i < root_item->rowCount(); ++i) {
    auto child = root_item->child(i);
    if (child->hasChildren()) {
      list.merge(getExclusions(child->text().toStdString()));
    } else if (child->checkState() != Qt::CheckState::Checked) {
      list.push_back(child->text().toStdString());
    }
  }

  return list;
}
