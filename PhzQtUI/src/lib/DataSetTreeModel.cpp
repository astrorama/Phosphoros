#include <iostream>
#include <exception>
#include <QFile>
#include <QDir>
#include <set>
#include "FileUtils.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "XYDataset/QualifiedName.h"

#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/AsciiParser.h"

#include "ElementsKernel/Logging.h"

using namespace std;
namespace Euclid {
namespace PhzQtUI {

DataSetTreeModel::DataSetTreeModel(DatasetRepo repository, QObject *parent) :
    QStandardItemModel(parent) ,
    m_repository(repository){
}

std::string DataSetTreeModel::getGroupName(XYDataset::QualifiedName qualified_name) {
  auto separator = QString(QDir::separator()).toStdString();
  std::string group_name = FileUtils::removeExt(qualified_name.qualifiedName(), qualified_name.datasetName());
  if (group_name[group_name.length() - 1] ==  separator[0]) {
    group_name = group_name.substr(0, group_name.length() - 1);
  }
  return group_name;
}



void DataSetTreeModel::load() {
  m_map_dir.clear();
  auto& unordered = m_repository->getContent();


  size_t max_depth=0;
  std::vector<std::set < std::string >> group_sets {};
  auto separator = QString(QDir::separator()).toStdString();

  for (auto& name : unordered) {
     std::string group_name = DataSetTreeModel::getGroupName(name);
     if (group_name.length()>0){
       size_t depth = std::count(group_name.begin(), group_name.end(),separator[0]);

       if (depth>max_depth){
         for (size_t curr_depth= max_depth+1; curr_depth<=depth;++curr_depth){
           group_sets.push_back(std::set < std::string >{});
         }

         max_depth=depth;
       }

       group_sets[depth].insert(group_name);
     }
  }

  // create the groups
  std::set < std::string > parent_group_set {};
  for (auto& group_set : group_sets){
    for (auto& group : group_set){
      auto group_qualifiedName = XYDataset::QualifiedName(group);
      std::string parent_group ="";

      for (auto& possible_parent : parent_group_set){
        auto parent_qualifiedName = XYDataset::QualifiedName(possible_parent);
        if (group_qualifiedName.belongsInGroup(parent_qualifiedName)){
          parent_group=possible_parent;
          break;
        }
      }

      QStandardItem* item = new QStandardItem(QString::fromStdString(group_qualifiedName.datasetName()));
      item->setBackground(QBrush(QColor(230, 230, 230)));
      item->setCheckable(true);
      item->setTristate(true);


      if (parent_group.length()>0){
        m_map_dir.at(parent_group)->appendRow(item);
      } else {
        this->appendRow(item);
      }

      m_map_dir[group] = item;
    }
    parent_group_set = group_set;
  }

  // put the items
  for (auto& name : unordered) {

    QStandardItem* item = new QStandardItem(QString::fromStdString(name.datasetName()));
    item->setCheckable(true);
    auto group_name = DataSetTreeModel::getGroupName(name);
    if (group_name.length()>0){
      m_map_dir.at(group_name)->appendRow(item);
    } else {
      this->appendRow(item);
    }
  }
}

void DataSetTreeModel::setEnabled(bool enable) {
  // root level
  for (int i=0; i<this->rowCount();++i){
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


void DataSetTreeModel::onItemChanged(QStandardItem* item) {
  if (m_in_edition) {

    auto item_state = item->checkState();
    if (item_state!=Qt::CheckState::PartiallyChecked){
      // Check/uncheck the children
      for (int i = 0; i < item->rowCount(); ++i) {
        item->child(i)->setCheckState(item_state);
      }
    }

    // look for the parent state
    if (item->parent()){
      bool has_checked=false;
      bool has_partial=false;
      bool has_unchecked=false;
      for (int i = 0; i < item->parent()->rowCount(); ++i){
        auto child_state =  item->parent()->child(i)->checkState();
        has_checked |= child_state==Qt::CheckState::Checked;
        has_partial |= child_state==Qt::CheckState::PartiallyChecked;
        has_unchecked |= child_state==Qt::CheckState::Unchecked;
      }

      if (has_partial | (has_checked && has_unchecked)){
        item->parent()->setCheckState(Qt::CheckState::PartiallyChecked);
      } else if (has_checked && !has_partial && !has_unchecked){
        item->parent()->setCheckState(Qt::CheckState::Checked);
      } else if (has_unchecked && !has_partial && !has_checked){
        item->parent()->setCheckState(Qt::CheckState::Unchecked);
      }
    }

  }
}



void DataSetTreeModel::setEditionStatus(bool inEdition) {
  m_in_edition = inEdition;
}


void DataSetTreeModel::clearState(){
  for (auto & group : m_map_dir){
    group.second->setCheckState(Qt::CheckState::Unchecked);
    for (int i=0; i<group.second->rowCount();++i){
       auto sub_item = group.second->child(i);
       sub_item->setCheckState(Qt::CheckState::Unchecked);
   }
  }

  for (int i=0; i<this->rowCount();++i){
        auto root_level_item = this->item(i);
        root_level_item->setCheckState(Qt::CheckState::Unchecked);
  }
}


void DataSetTreeModel::checkGroup(XYDataset::QualifiedName name) {
  for (auto & group : m_map_dir) {
    if (XYDataset::QualifiedName(group.first).belongsInGroup(name) || name.qualifiedName() == group.first) {
      group.second->setCheckState(Qt::CheckState::Checked);
      for (int i = 0; i < group.second->rowCount(); ++i) {
        auto sub_item = group.second->child(i);
        sub_item->setCheckState(Qt::CheckState::Checked);
      }

    }
  }
}


void DataSetTreeModel::partialCheckParentGroups(XYDataset::QualifiedName child ){
  for (auto & group : m_map_dir) {
      if (child.belongsInGroup(XYDataset::QualifiedName(group.first))){
        group.second->setCheckState(Qt::CheckState::PartiallyChecked);
      }
  }
}

void DataSetTreeModel::setState(const DatasetSelection& selection) {
  clearState();

  // groupes
  for (auto & group : selection.getGroupes()){
    checkGroup(XYDataset::QualifiedName(group));
  }

  // isolated
  for (auto & lone : selection.getIsolated()){
    for (int i=0; i<this->rowCount();++i){
       auto root_level_item = this->item(i);
       if (root_level_item->text().toStdString()==lone && root_level_item->rowCount()==0){
         root_level_item->setCheckState(Qt::CheckState::Checked);

       }
     }
  }

  // exclusions
  for (auto & exclusion : selection.getExclusions()){
    XYDataset::QualifiedName exclusion_qualified_name(exclusion);
    partialCheckParentGroups(exclusion_qualified_name);
    auto group_name =  DataSetTreeModel::getGroupName(exclusion_qualified_name);
    auto item_name = exclusion_qualified_name.datasetName();
    if (m_map_dir.count(group_name)==1){
      for (int i=0; i<m_map_dir.at(group_name)->rowCount();++i){
        auto item = m_map_dir.at(group_name)->child(i);
        if (item->text().toStdString()==item_name && item->rowCount()==0){
          item->setCheckState(Qt::CheckState::Unchecked);

        }
      }
    }
  }

}


DatasetSelection DataSetTreeModel::getState() const{
  auto separator = QString(QDir::separator()).toStdString();
  DatasetSelection current_state;

  // groupes
  std::vector<std::string> groups{};
  for (auto& group_tuple : m_map_dir){
    if (group_tuple.second->checkState()!=Qt::CheckState::Unchecked){
      groups.push_back(group_tuple.first);
    }
  }

  // isolated
  std::vector<std::string> isolated{};
  for (int i=0; i<this->rowCount();++i){
    auto root_level_item = this->item(i);
    if (root_level_item->checkState()==Qt::CheckState::Checked && root_level_item->rowCount()==0){
      isolated .push_back(root_level_item->text().toStdString());
    }
  }

 // exclusions
  std::vector<std::string> exclusions{};
  for (auto& group_tuple : m_map_dir){
    auto group_item = group_tuple.second;
    auto base_name = group_tuple.first;

    bool has_selected_parent=false;
    XYDataset::QualifiedName group_qualified_name(base_name);
    for (auto& added_group : groups){
      XYDataset::QualifiedName parent_group(added_group);
      if (group_qualified_name.belongsInGroup(parent_group)){
        has_selected_parent=true;
        break;
      }
    }
    if (group_item->checkState()==Qt::CheckState::PartiallyChecked || ( group_item->checkState()==Qt::CheckState::Unchecked && has_selected_parent)){
      for (int i=0; i<group_item->rowCount();++i){
        auto child_item = group_item->child(i);
        if (child_item->rowCount()==0 && child_item->checkState()==Qt::CheckState::Unchecked){
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




}
}
