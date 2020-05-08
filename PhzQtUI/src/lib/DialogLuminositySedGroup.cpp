/*
 * DialogLuminositySedGroup.cpp
 *
 *  Created on: September 4, 2015
 *      Author: fdubath
 */
#include <QMessageBox>
#include "PhzQtUI/DialogLuminositySedGroup.h"
#include "ui_DialogLuminositySedGroup.h"
#include <QLabel>
#include <QLineEdit>

#include <QTreeView>
#include <QHeaderView>

#include <QStandardItemModel>
#include "PhzQtUI/GridButton.h"
#include "XYDataset/QualifiedName.h"

#include "PhzQtUI/SedGroupModel.h"



namespace Euclid {
namespace PhzQtUI {

DialogLuminositySedGroup::DialogLuminositySedGroup(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogLuminositySedGroup) {
 ui->setupUi(this);
}

DialogLuminositySedGroup::~DialogLuminositySedGroup() {}

void DialogLuminositySedGroup::setGroups(std::vector<LuminosityPriorConfig::SedGroup> groups){
  m_groups = std::move(groups);


}

void DialogLuminositySedGroup::setDiff(std::vector<std::string> missing_seds, std::vector<std::string> new_seds){
  if (missing_seds.size()>0){
    for (auto& group : m_groups){
      auto iterator = std::begin(group.second);
      while (iterator != std::end(group.second)) {
        std::string sed_name = *iterator;
        if (std::find(missing_seds.begin(), missing_seds.end(), sed_name) != missing_seds.end()){
          iterator = group.second.erase(iterator);
        }
        else {
          ++iterator;
        }
      }
    }
  }
  if (new_seds.size()>0){

    std::vector<std::string> sed_to_add{};
    for (auto & test_sed : new_seds){
      bool found=false;
      for (auto& group : m_groups){
        if (std::find(group.second.begin(), group.second.end(), test_sed) != group.second.end()){
          found=true;
          break;
        }
      }

      if(!found){
        sed_to_add.push_back(test_sed);
      }
    }

    if (sed_to_add.size()>0){
      m_groups.push_back(LuminosityPriorConfig::SedGroup("Added_SED",new_seds));
    }
  }

  size_t i=0;
    size_t max_i = m_groups.size()-1;
    for (auto& group : m_groups){
      addGroup(group,i,max_i);
      ++i;

    }
}

void DialogLuminositySedGroup::addGroup(LuminosityPriorConfig::SedGroup group, size_t i, size_t i_max){

  auto frame_tree = new QFrame();
  frame_tree->setFrameStyle(QFrame::Box);
  auto layout_tree = new QVBoxLayout();
  frame_tree->setLayout(layout_tree);
  auto title_frame_tree = new QFrame();
  title_frame_tree->setFrameStyle(QFrame::Box);
  auto title_layout_tree = new QHBoxLayout();
  title_frame_tree->setLayout(title_layout_tree);
  auto title_text_tree = new QLineEdit(QString::fromStdString(group.first));
  title_layout_tree->addWidget(title_text_tree);
  auto btn_del_tree = new GridButton(i,0,"-");
  btn_del_tree->setMaximumWidth(30);
  connect(btn_del_tree,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onDeleteGroupClicked(size_t,size_t)));
  if (i_max==0){
    btn_del_tree->setEnabled(false);
  }
  title_layout_tree->addWidget(btn_del_tree);
  layout_tree->addWidget(title_frame_tree);
  auto tree_view = new QTreeView();

  auto model = new SedGroupModel();
  model->load(group.second);
  tree_view->setModel(model);

  tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
  tree_view->setDragEnabled(true);
  tree_view->viewport()->setAcceptDrops(true);
  tree_view->setDropIndicatorShown(true);



  tree_view->header()->hide();
  tree_view->setColumnHidden(1,true);
  tree_view->setColumnHidden(2,true);
  layout_tree->addWidget(tree_view);
  tree_view->expandAll();
  ui->layout_groups->addWidget(frame_tree);

}



void DialogLuminositySedGroup::on_btn_add_clicked(){

  m_groups.push_back(LuminosityPriorConfig::SedGroup("New_Group",{}));

  addGroup(m_groups[m_groups.size()-1],m_groups.size()-1,m_groups.size()-1);

}


void DialogLuminositySedGroup::onDeleteGroupClicked(size_t sed_group_id,size_t){

  readNewGroups();

  for (int i = ui->layout_groups->count()-1;  i >=0;  --i) {
     QWidget *widget = ui->layout_groups->itemAt(i)->widget();
     if (widget != NULL) {
      delete widget;
     }
  }

  // move the SED into the next (or previous) group
  size_t target_group_id = sed_group_id-1;
  if (sed_group_id==0){
    target_group_id=1;
  }

  std::vector<std::string> new_seds{};
  auto old_seds = m_groups[target_group_id].second;
  for (size_t j=0; j<old_seds.size();++j){
          new_seds.push_back({old_seds[j]});
  }

  old_seds = m_groups[sed_group_id].second;
  for (size_t j=0; j<old_seds.size();++j){
            new_seds.push_back({old_seds[j]});
  }
  m_groups[target_group_id]=LuminosityPriorConfig::SedGroup(m_groups[target_group_id].first,std::move(new_seds));

  auto iter = m_groups.begin();
  for (size_t j=0; j<sed_group_id;++j ){
    ++iter;
  }

  m_groups.erase(iter);

  size_t i=0;
  size_t max_i = m_groups.size()-1;
    for (auto& group : m_groups){
      addGroup(group,i,max_i);
      ++i;

  }

}



void DialogLuminositySedGroup::readNewGroups(){
  auto group_iter = m_groups.begin();
  for (int i = 0; i< ui->layout_groups->count();  ++i) {
     QWidget *widget = ui->layout_groups->itemAt(i)->widget();
     if (widget != NULL) {
       QString name = static_cast<QLineEdit*>(widget->children()[1]->children()[1])->text();
       group_iter->first=name.toStdString();
       SedGroupModel* model = static_cast<SedGroupModel*>(static_cast<QTreeView*>(widget->children()[2])->model());
       group_iter->second = model->getSeds();
       ++group_iter;
     }
  }
}


void DialogLuminositySedGroup::on_btn_cancel_clicked(){
  reject();
}



void DialogLuminositySedGroup::on_btn_save_clicked(){
  readNewGroups();

  for (size_t i=0; i <m_groups.size();++i){
    auto& group_1 = m_groups[i];

    if (group_1.second.size()==0){
          QMessageBox::warning(this,
                               "Empty group",
                               "The SED Group '"+ QString::fromStdString(group_1.first)+ "' is empty.\n"
                               "Please delete it or move some SED inside.",
                               QMessageBox::Ok,
                               QMessageBox::Ok);
          return;
        }

    if (group_1.first.find_first_of("\t ") != std::string::npos){
      QMessageBox::warning(this,
                           "Space in the Name",
                           "Please do not use space in the group names.",
                           QMessageBox::Ok,
                           QMessageBox::Ok);
                return;
    }



    for (size_t j=i+1; j <m_groups.size();++j){
       auto& group_2 = m_groups[j];
        if (group_1.first==group_2.first){
          QMessageBox::warning(this,
                               "Duplicate name",
                               "Multiple SED Groups are named '"+ QString::fromStdString(group_1.first)+ "'.\n"
                               "Please ensure that the name are unique.",
                               QMessageBox::Ok,
                               QMessageBox::Ok);
          return;
        }
      }
  }

  popupClosing(std::move(m_groups));
  accept();
}



}
}

