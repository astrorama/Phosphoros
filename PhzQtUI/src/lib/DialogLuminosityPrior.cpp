/*
 * DialogLuminosityPrior.cpp

 *
 *  Created on: Sept 2, 2015
 *      Author: fdubath
 */

#include <QFuture>
#include <qtconcurrentrun.h>
#include <QDir>
#include <QMessageBox>
#include "PhzQtUI/DialogLuminosityPrior.h"
#include <boost/program_options.hpp>
#include "ui_DialogLuminosityPrior.h"
#include <QLabel>

#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/LuminosityFunctionInfo.h"
#include "PhzQtUI/DialogLuminosityFunction.h"
#include "PhzQtUI/DialogLuminositySedGroup.h"



//assume 2x3 layout => 3 row 4 columns

namespace Euclid {
namespace PhzQtUI {



DialogLuminosityPrior::DialogLuminosityPrior(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogLuminosityPrior)
    {
       ui->setupUi(this);

       ui->frame_Luminosity->setStyleSheet( "background-color: white ");


       std::vector<double> zs {0.,1.,2.,6.};

       PhzLuminosity::SedGroup group_1("Group1", {{"sed 1"},{"sed 2"},{"sed 3"}});
       PhzLuminosity::SedGroup group_2("Group2", {{"sed 4"}});
       m_groups.push_back(std::move(group_1));
       m_groups.push_back(std::move(group_2));


       for (size_t i =0; i<zs.size()-1;++i){
          m_luminosityInfos.push_back({});
          for (size_t j =0; j<m_groups.size();++j){
            m_luminosityInfos[i].push_back(LuminosityFunctionInfo{});
            m_luminosityInfos[i][j].sedGroupName=m_groups[j].getName();
            m_luminosityInfos[i][j].z_min=zs[i];
            m_luminosityInfos[i][j].z_max=zs[i+1];
          }
       }

       m_luminosityInfos[1][1].phi=0.04;
       m_luminosityInfos[2][0].is_custom=true;
       m_luminosityInfos[2][0].curve_name="my curve";

       loadGrid(m_groups,zs);

    }

DialogLuminosityPrior::~DialogLuminosityPrior() {}



void DialogLuminosityPrior::ClearGrid(){
  ui->frame_Luminosity->findChildren<QWidget *>();
  for(auto child : ui->frame_Luminosity->children())
  {
      delete child;
  }

}

void DialogLuminosityPrior::loadGrid(const std::vector<PhzLuminosity::SedGroup>& groups, const std::vector<double>& zs){
  auto frame_1 = new QFrame();
        frame_1->setFrameStyle(QFrame::NoFrame);
        frame_1->setMinimumHeight(30);
        ui->gl_Luminosity->addWidget(frame_1, 0, 0);


        for (size_t i =0; i<groups.size();++i){
          auto frame_grp = new QFrame();
          frame_grp->setFrameStyle(QFrame::NoFrame);
          frame_grp->setMinimumHeight(30);
          auto layout= new QHBoxLayout();
          frame_grp->setLayout(layout);
          auto label = new QLabel(QString::fromStdString(groups[i].getName()));
          label->setAlignment(Qt::AlignCenter);
          layout->addWidget(label);
          ui->gl_Luminosity->addWidget(frame_grp, 1+i, 0);
        }


        for (size_t i =0; i<zs.size()-1;++i){
          auto frame = new QFrame();
          frame->setFrameStyle(QFrame::NoFrame);
          frame->setMinimumHeight(30);
          auto layout= new QVBoxLayout();
          frame->setLayout(layout);
          auto label = new QLabel("z="+QString::number(zs[i],'f', 2) +" - "+QString::number(zs[i+1],'f', 2));
          label->setAlignment(Qt::AlignCenter);
          layout->addWidget(label);
          ui->gl_Luminosity->addWidget(frame, 0,1+i);
       }

        for (size_t i=0;i<zs.size()-1;++i){
          for(size_t j=0;j<groups.size();++j){
            auto frame = new QFrame();
            frame->setMinimumHeight(30);
            frame->setFrameStyle(QFrame::Box);
            if (!m_luminosityInfos[i][j].isComplete()){
              frame->setStyleSheet( "background-color: red ");
            }
            auto layout = new QVBoxLayout();
            frame->setLayout(layout);

            auto label = new QLabel("To be defined");

            if (m_luminosityInfos[i][j].isComplete()){
              label->setText(m_luminosityInfos[i][j].getDescription());
            }
            label->setAlignment(Qt::AlignCenter);
            layout->addWidget(label);
            GridButton * button = new GridButton(i,j,"Define");
            button->setStyleSheet( "background-color: lightGrey ");
            connect( button, SIGNAL(GridButtonClicked(int,int)), this,SLOT(onGridButtonClicked(int,int)));
            layout->addWidget(button);
            ui->gl_Luminosity->addWidget(frame, j+1, i+1);
          }
        }
}

void DialogLuminosityPrior::onGridButtonClicked(int x,int y){
  std::unique_ptr<DialogLuminosityFunction> dialog(new DialogLuminosityFunction());
  dialog->setInfo(m_luminosityInfos[x][y],x,y);
  connect(dialog.get(),SIGNAL(popupClosing(LuminosityFunctionInfo, int, int)),
 this,    SLOT(luminosityFunctionPopupClosing(LuminosityFunctionInfo, int, int)));
  dialog->exec();


}


void DialogLuminosityPrior::luminosityFunctionPopupClosing(LuminosityFunctionInfo info, int x, int y){
  m_luminosityInfos[x][y]=info;

  auto layoutItem = ui->gl_Luminosity->itemAtPosition(y+1, x+1);



  if (info.isComplete()){
    layoutItem->widget()->setStyleSheet( "background-color: white ");
    static_cast<QLabel*>(layoutItem->widget()->children()[1])->setText(info.getDescription());
  } else {
    layoutItem->widget()->setStyleSheet( "background-color: red ");
    static_cast<QLabel*>(layoutItem->widget()->children()[1])->setText("To be defined");
  }


}

void DialogLuminosityPrior::on_btn_group_clicked(){

  std::unique_ptr<DialogLuminositySedGroup> dialog(new DialogLuminositySedGroup());
  dialog->setGroups(m_groups);
  connect(dialog.get(),SIGNAL(popupClosing(std::vector<PhzLuminosity::SedGroup>)),this,SLOT(groupPopupClosing(std::vector<PhzLuminosity::SedGroup>)));
  dialog->exec();

}


void DialogLuminosityPrior::groupPopupClosing(std::vector<PhzLuminosity::SedGroup> groups){
  m_groups=std::move(groups);

  // update function,clear grid redraw

 // ClearGrid();

}

}
}

