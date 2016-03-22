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
#include <QScrollArea>

#include "PhzQtUI/GridButton.h"

//http://stackoverflow.com/questions/4412796/qt-qtableview-clickable-button-in-table-row

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
  auto frame = new QFrame();
      frame->setFrameStyle(QFrame::Box);
      auto layout = new QVBoxLayout();
      frame->setLayout(layout);

      auto titleFrame = new QFrame();
      titleFrame->setFrameStyle(QFrame::Box);
      auto titleLayout = new QHBoxLayout();
      titleFrame->setLayout(titleLayout);
      auto txt_title = new QLineEdit(QString::fromStdString(group.first));
      titleLayout->addWidget(txt_title);
      auto btn_del = new GridButton(i,0,"-");
      btn_del->setMaximumWidth(30);
      connect(btn_del,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onDeleteGroupClicked(size_t,size_t)));

      if (i_max==0){
        btn_del->setEnabled(false);
      }
      titleLayout->addWidget(btn_del);

      layout->addWidget(titleFrame);

      auto scrollFrame = new QFrame();
      auto layout2 = new QVBoxLayout();
      scrollFrame->setLayout(layout2);

      fillSedList(group.second,i,i_max,layout2);

      auto scroll = new QScrollArea();
      scroll->setWidget(scrollFrame);

      layout->addWidget(scroll);

      ui->hl_groups->addWidget(frame);

}


void DialogLuminositySedGroup::fillSedList(std::vector<std::string> seds ,size_t group_id, size_t max_group_id, QVBoxLayout* layout){
  size_t j = 0;
  for (auto sed : seds) {
    auto sedFrame = new QFrame();
    auto sedLayout = new QHBoxLayout();
    sedFrame->setLayout(sedLayout);

    if (group_id > 0) {
      auto btn_left = new GridButton(group_id, j, "<");
      btn_left->setMaximumWidth(30);
      sedLayout->addWidget(btn_left);
      connect(btn_left,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onMoveLeftClicked(size_t,size_t)));

    }

    auto txt_sed = new QLabel(QString::fromStdString(sed));
    sedLayout->addWidget(txt_sed);

    if (group_id < max_group_id) {
      auto btn_right = new GridButton(group_id, j, ">");
      btn_right->setMaximumWidth(30);
      sedLayout->addWidget(btn_right);
      connect(btn_right,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onMoveRightClicked(size_t,size_t)));
    }

    layout->addWidget(sedFrame);

    ++j;

  }


}


void DialogLuminositySedGroup::onMoveRightClicked(size_t sed_group_id,size_t sed_id){



  //add to the new list
  if (sed_group_id<m_groups.size()-1){
    clearSeds(sed_group_id+1);

    std::vector<std::string> new_seds{};
    auto old_seds = m_groups[sed_group_id+1].second;
    for (size_t j=0; j<old_seds.size();++j){
        new_seds.push_back({old_seds[j]});
    }
    new_seds.push_back({ m_groups[sed_group_id].second[sed_id]});

    m_groups[sed_group_id+1]=LuminosityPriorConfig::SedGroup(m_groups[sed_group_id+1].first,std::move(new_seds));
    auto group_frame = ui->frame_groups->children()[2+sed_group_id];


    auto scrollFrame = new QFrame();
    auto layout2 = new QVBoxLayout();
    scrollFrame->setLayout(layout2);
    fillSedList(m_groups[sed_group_id+1].second,sed_group_id+1,m_groups.size()-1,layout2);
    auto scroll = new QScrollArea();
    scroll->setWidget(scrollFrame);
    static_cast<QVBoxLayout*>(group_frame->children()[0])->addWidget(scroll);

  }

  //remove the id from the list
  clearSeds(sed_group_id);

  std::vector<std::string> new_seds { };
  auto old_seds = m_groups[sed_group_id].second;
  for (size_t j = 0; j < old_seds.size(); ++j) {
    if (j != sed_id) {
      new_seds.push_back( { old_seds[j] });
    }
  }

  m_groups[sed_group_id] = LuminosityPriorConfig::SedGroup(
      m_groups[sed_group_id].first, std::move(new_seds));
  auto group_frame = ui->frame_groups->children()[1 + sed_group_id];

  auto scrollFrame = new QFrame();
  auto layout2 = new QVBoxLayout();
  scrollFrame->setLayout(layout2);
  fillSedList(m_groups[sed_group_id].second, sed_group_id, m_groups.size() - 1, layout2);
  auto scroll = new QScrollArea();
  scroll->setWidget(scrollFrame);
  static_cast<QVBoxLayout*>(group_frame->children()[0])->addWidget(scroll);

}

void DialogLuminositySedGroup::onMoveLeftClicked(size_t sed_group_id,size_t sed_id){
  //add to the new list
    if (sed_group_id>0){
      clearSeds(sed_group_id-1);

      std::vector<std::string> new_seds{};
      auto old_seds = m_groups[sed_group_id-1].second;
      for (size_t j=0; j<old_seds.size();++j){
          new_seds.push_back({old_seds[j]});
      }
      new_seds.push_back({ m_groups[sed_group_id].second[sed_id]});

      m_groups[sed_group_id-1]=LuminosityPriorConfig::SedGroup(m_groups[sed_group_id-1].first,std::move(new_seds));
      auto group_frame = ui->frame_groups->children()[sed_group_id];


      auto scrollFrame = new QFrame();
      auto layout2 = new QVBoxLayout();
      scrollFrame->setLayout(layout2);
      fillSedList(m_groups[sed_group_id-1].second,sed_group_id-1,m_groups.size()-1,layout2);
      auto scroll = new QScrollArea();
      scroll->setWidget(scrollFrame);
      static_cast<QVBoxLayout*>(group_frame->children()[0])->addWidget(scroll);

    }

    //remove the id from the list
    if (sed_group_id<m_groups.size()){
      clearSeds(sed_group_id);

      std::vector<std::string> new_seds{};
      auto old_seds = m_groups[sed_group_id].second;
      for (size_t j=0; j<old_seds.size();++j){
        if (j!=sed_id){
          new_seds.push_back({old_seds[j]});
        }
      }

      m_groups[sed_group_id]=LuminosityPriorConfig::SedGroup(m_groups[sed_group_id].first,std::move(new_seds));
      auto group_frame = ui->frame_groups->children()[1+sed_group_id];

      auto scrollFrame = new QFrame();
      auto layout2 = new QVBoxLayout();
      scrollFrame->setLayout(layout2);
      fillSedList(m_groups[sed_group_id].second,sed_group_id,m_groups.size()-1,layout2);
      auto scroll = new QScrollArea();
      scroll->setWidget(scrollFrame);
      static_cast<QVBoxLayout*>(group_frame->children()[0])->addWidget(scroll);
    }
}


void DialogLuminositySedGroup::clearSeds(size_t group_id){
  auto group_frame = ui->frame_groups->children()[1+group_id];

  delete group_frame->children()[2];

}


void DialogLuminositySedGroup::on_btn_add_clicked(){

  m_groups.push_back(LuminosityPriorConfig::SedGroup("New_Group",{}));

    size_t i = 0;
     for (auto child : ui->frame_groups->children()) {
       if (i > m_groups.size()-2) {
         delete child;
       }
       ++i;
     }
  addGroup(m_groups[m_groups.size()-2],m_groups.size()-2,m_groups.size()-1);

  addGroup(m_groups[m_groups.size()-1],m_groups.size()-1,m_groups.size()-1);

}


void DialogLuminositySedGroup::onDeleteGroupClicked(size_t sed_group_id,size_t){
  size_t i = 0;
  for (auto child : ui->frame_groups->children()) {
    if (i > 0) {
      delete child;
    }
    ++i;
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

  i=0;
  size_t max_i = m_groups.size()-1;
    for (auto& group : m_groups){
      addGroup(group,i,max_i);
      ++i;

  }

}



std::vector<std::string> DialogLuminositySedGroup::getNewGroupNames(){
  std::vector<std::string>  result{};
  size_t  i=0;
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
    if (group.second.size()==0){
      QMessageBox::warning(this,
                           "Empty group",
                           "The SED Group '"+ QString::fromStdString(group.first)+ "' is empty.\n"
                           "Please delete it or move some SED inside.",
                           QMessageBox::Ok,
                           QMessageBox::Ok);
      return;
    }
  }
  // groups names are distincts

  auto names = getNewGroupNames();
  for (size_t i=0; i<names.size();++i){
    if (names[i].find_first_of("\t ") != std::string::npos){
      QMessageBox::warning(this,
                           "Space in the Name",
                           "Please do not use space in the group names.",
                           QMessageBox::Ok,
                           QMessageBox::Ok);
                return;
    }



    for (size_t j=i+1; j<names.size();++j){
        if (names[i]==names[j]){
          QMessageBox::warning(this,
                               "Duplicate name",
                               "Multiple SED Groups are named '"+ QString::fromStdString(names[i])+ "'.\n"
                               "Please ensure that the name are unique.",
                               QMessageBox::Ok,
                               QMessageBox::Ok);
          return;
        }
      }
  }

  for (size_t i=0;i<m_groups.size();++i){
    std::vector<std::string> new_seds{};
     auto old_seds = m_groups[i].second;
     for (size_t j=0; j<old_seds.size();++j){
             new_seds.push_back({old_seds[j]});
     }
     m_groups[i]=LuminosityPriorConfig::SedGroup(names[i],std::move(new_seds));

  }

  popupClosing(std::move(m_groups));
  accept();
}



}
}
