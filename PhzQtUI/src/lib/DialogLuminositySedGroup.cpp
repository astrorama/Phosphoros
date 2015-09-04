/*
 * DialogLuminositySedGroup.cpp
 *
 *  Created on: September 4, 2015
 *      Author: fdubath
 */
#include <QFuture>
#include <qtconcurrentrun.h>
#include <QDir>
#include <QMessageBox>
#include "PhzQtUI/DialogLuminositySedGroup.h"
#include <boost/program_options.hpp>
#include "ui_DialogLuminositySedGroup.h"
#include "XYDataset/QualifiedName.h"
#include <QLabel>
#include <QLineEdit>

#include "PhzQtUI/GridButton.h"

// #include <future>
//http://stackoverflow.com/questions/4412796/qt-qtableview-clickable-button-in-table-row
//assume 2x3 layout => 3 row 4 columns

namespace Euclid {
namespace PhzQtUI {



DialogLuminositySedGroup::DialogLuminositySedGroup(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogLuminositySedGroup)
    {
       ui->setupUi(this);
}

DialogLuminositySedGroup::~DialogLuminositySedGroup() {}

void DialogLuminositySedGroup::setGroups(std::vector<PhzLuminosity::SedGroup> groups){
  m_groups = std::move(groups);

  int i=0;
  int max_i = m_groups.size()-1;
  for (auto& group : m_groups){
    addGroup(group,i,max_i);
    ++i;

  }

}


void DialogLuminositySedGroup::addGroup(PhzLuminosity::SedGroup group, int i, int i_max){
  auto frame = new QFrame();
      frame->setFrameStyle(QFrame::Box);
      auto layout = new QVBoxLayout();
      frame->setLayout(layout);

      auto titleFrame = new QFrame();
      titleFrame->setFrameStyle(QFrame::Box);
      auto titleLayout = new QHBoxLayout();
      titleFrame->setLayout(titleLayout);
      auto txt_title = new QLineEdit(QString::fromStdString(group.getName()));
      titleLayout->addWidget(txt_title);
      auto btn_del = new GridButton(i,0,"-");
      btn_del->setMaximumWidth(30);
      connect(btn_del,SIGNAL(GridButtonClicked(int,int)),this,SLOT(onDeleteGroupClicked(int,int)));

      if (i_max==0){
        btn_del->setEnabled(false);
      }
      titleLayout->addWidget(btn_del);

      layout->addWidget(titleFrame);

      fillSedList(group.getSedNameList(),i,i_max,layout);

      layout->addStretch();

      ui->hl_groups->addWidget(frame);

}


void DialogLuminositySedGroup::fillSedList(std::vector<std::string> seds ,int group_id, int max_group_id, QVBoxLayout* layout){
  int j = 0;
  for (auto sed : seds) {
    auto sedFrame = new QFrame();
    auto sedLayout = new QHBoxLayout();
    sedFrame->setLayout(sedLayout);

    if (group_id > 0) {
      auto btn_left = new GridButton(group_id, j, "<");
      btn_left->setMaximumWidth(30);
      sedLayout->addWidget(btn_left);
      connect(btn_left,SIGNAL(GridButtonClicked(int,int)),this,SLOT(onMoveLeftClicked(int,int)));

    }

    auto txt_sed = new QLabel(QString::fromStdString(sed));
    sedLayout->addWidget(txt_sed);

    if (group_id < max_group_id) {
      auto btn_right = new GridButton(group_id, j, ">");
      btn_right->setMaximumWidth(30);
      sedLayout->addWidget(btn_right);
      connect(btn_right,SIGNAL(GridButtonClicked(int,int)),this,SLOT(onMoveRightClicked(int,int)));
    }

    layout->addWidget(sedFrame);

    ++j;

  }



}


void DialogLuminositySedGroup::onMoveRightClicked(int sed_group_id,int sed_id){



  //add to the new list
  if (sed_group_id<m_groups.size()-1){
    clearSeds(sed_group_id+1);

    std::vector<XYDataset::QualifiedName> new_seds{};
    auto old_seds = m_groups[sed_group_id+1].getSedNameList();
    for (size_t j=0; j<old_seds.size();++j){
        new_seds.push_back({old_seds[j]});
    }
    new_seds.push_back({ m_groups[sed_group_id].getSedNameList()[sed_id]});

    m_groups[sed_group_id+1]=PhzLuminosity::SedGroup(m_groups[sed_group_id+1].getName(),std::move(new_seds));
    auto group_frame = ui->frame_groups->children()[2+sed_group_id];
    auto layout = group_frame->children()[0];

    fillSedList(m_groups[sed_group_id+1].getSedNameList(),sed_group_id+1,int{m_groups.size()-1},static_cast<QVBoxLayout*>(layout));
  }

  //remove the id from the list
  if (sed_group_id>-1){
    clearSeds(sed_group_id);

    std::vector<XYDataset::QualifiedName> new_seds{};
    auto old_seds = m_groups[sed_group_id].getSedNameList();
    for (size_t j=0; j<old_seds.size();++j){
      if (j!=sed_id){
        new_seds.push_back({old_seds[j]});
      }
    }

    m_groups[sed_group_id]=PhzLuminosity::SedGroup(m_groups[sed_group_id].getName(),std::move(new_seds));
    auto group_frame = ui->frame_groups->children()[1+sed_group_id];
    auto layout = group_frame->children()[0];

    fillSedList(m_groups[sed_group_id].getSedNameList(),sed_group_id,int{m_groups.size()-1},static_cast<QVBoxLayout*>(layout));
  }
}

void DialogLuminositySedGroup::onMoveLeftClicked(int sed_group_id,int sed_id){
  //add to the new list
    if (sed_group_id>0){
      clearSeds(sed_group_id-1);

      std::vector<XYDataset::QualifiedName> new_seds{};
      auto old_seds = m_groups[sed_group_id-1].getSedNameList();
      for (size_t j=0; j<old_seds.size();++j){
          new_seds.push_back({old_seds[j]});
      }
      new_seds.push_back({ m_groups[sed_group_id].getSedNameList()[sed_id]});

      m_groups[sed_group_id-1]=PhzLuminosity::SedGroup(m_groups[sed_group_id-1].getName(),std::move(new_seds));
      auto group_frame = ui->frame_groups->children()[sed_group_id];
      auto layout = group_frame->children()[0];

      fillSedList(m_groups[sed_group_id-1].getSedNameList(),sed_group_id-1,int{m_groups.size()-1},static_cast<QVBoxLayout*>(layout));
    }

    //remove the id from the list
    if (sed_group_id<m_groups.size()){
      clearSeds(sed_group_id);

      std::vector<XYDataset::QualifiedName> new_seds{};
      auto old_seds = m_groups[sed_group_id].getSedNameList();
      for (size_t j=0; j<old_seds.size();++j){
        if (j!=sed_id){
          new_seds.push_back({old_seds[j]});
        }
      }

      m_groups[sed_group_id]=PhzLuminosity::SedGroup(m_groups[sed_group_id].getName(),std::move(new_seds));
      auto group_frame = ui->frame_groups->children()[1+sed_group_id];
      auto layout = group_frame->children()[0];

      fillSedList(m_groups[sed_group_id].getSedNameList(),sed_group_id,int{m_groups.size()-1},static_cast<QVBoxLayout*>(layout));
    }
}


void DialogLuminositySedGroup::clearSeds(int group_id){
  auto group_frame = ui->frame_groups->children()[1+group_id];
  int i=0;
  for(auto child : group_frame->children()){
    if (i>1){
      delete child;
    }
    ++i;
  }
}


void DialogLuminositySedGroup::on_btn_add_clicked(){

  m_groups.push_back(PhzLuminosity::SedGroup("New Group",{}));

    int i = 0;
     for (auto child : ui->frame_groups->children()) {
       if (i > m_groups.size()-2) {
         delete child;
       }
       ++i;
     }
  addGroup(m_groups[m_groups.size()-2],m_groups.size()-2,m_groups.size()-1);

  addGroup(m_groups[m_groups.size()-1],m_groups.size()-1,m_groups.size()-1);

}


void DialogLuminositySedGroup::onDeleteGroupClicked(int sed_group_id,int){
  int i = 0;
  for (auto child : ui->frame_groups->children()) {
    if (i > 0) {
      delete child;
    }
    ++i;
  }

  // move the SED into the next (or previous) group
  int target_group_id = sed_group_id-1;
  if (target_group_id<0){
    target_group_id=1;
  }

  std::vector<XYDataset::QualifiedName> new_seds{};
  auto old_seds = m_groups[target_group_id].getSedNameList();
  for (size_t j=0; j<old_seds.size();++j){
          new_seds.push_back({old_seds[j]});
  }

  old_seds = m_groups[sed_group_id].getSedNameList();
  for (size_t j=0; j<old_seds.size();++j){
            new_seds.push_back({old_seds[j]});
  }
  m_groups[target_group_id]=PhzLuminosity::SedGroup(m_groups[target_group_id].getName(),std::move(new_seds));

  auto iter = m_groups.begin();
  for (int j=0; j<sed_group_id;++j ){
    ++iter;
  }

  m_groups.erase(iter);

  i=0;
    int max_i = m_groups.size()-1;
    for (auto& group : m_groups){
      addGroup(group,i,max_i);
      ++i;

  }

}



std::vector<std::string> DialogLuminositySedGroup::getNewGroupNames(){
  std::vector<std::string>  result{};
  int  i=0;
  for (auto child : ui->frame_groups->children()) {
     if (i > 0) {
       result.push_back(static_cast<QLineEdit*>(child->children()[1]->children()[1])->text().toStdString());
     }
     ++i;
   }


  return result;
}




void DialogLuminositySedGroup::on_btn_cancel_clicked(){
  reject();
}

void DialogLuminositySedGroup::on_btn_save_clicked(){

  //check all group have at least 1 element
  for(auto& group : m_groups){
    if (group.getSedNameList().size()==0){
      QMessageBox::warning(this, "Empty group",
                                     "The SED Group '"+ QString::fromStdString(group.getName())+ "' is empty.\n"
                                        "Please delete it or move some SED inside.",
                                        QMessageBox::Ok,QMessageBox::Ok);
      return;
    }
  }
  // groups names are distincts

  auto names = getNewGroupNames();
  for (int i=0; i<names.size();++i){
    for (int j=i+1; j<names.size();++j){
        if (names[i]==names[j]){
          QMessageBox::warning(this, "Duplicate name",
                                              "Multiple SED Groups are named '"+ QString::fromStdString(names[i])+ "'.\n"
                                                 "Please ensure that the name are unique.",
                                                 QMessageBox::Ok,QMessageBox::Ok);
          return;
        }
      }
  }

  for (int i=0;i<m_groups.size();++i){
    std::vector<XYDataset::QualifiedName> new_seds{};
     auto old_seds = m_groups[i].getSedNameList();
     for (size_t j=0; j<old_seds.size();++j){
             new_seds.push_back({old_seds[j]});
     }
     m_groups[i]=PhzLuminosity::SedGroup(names[i],std::move(new_seds));

  }

  popupClosing(std::move(m_groups));
  accept();
}



}
}

