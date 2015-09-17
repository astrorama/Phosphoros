/*
 * DialogZRanges.cpp
 *
 *  Created on: September 4, 2015
 *      Author: fdubath
 */
#include <QFuture>
#include <qtconcurrentrun.h>
#include <QDir>
#include <QMessageBox>
#include "PhzQtUI/DialogZRanges.h"
#include <boost/program_options.hpp>
#include "ui_DialogZRanges.h"
#include "XYDataset/QualifiedName.h"
#include <QLabel>
#include <QLineEdit>

#include "PhzQtUI/GridButton.h"

// #include <future>
//http://stackoverflow.com/questions/4412796/qt-qtableview-clickable-button-in-table-row
//assume 2x3 layout => 3 row 4 columns

namespace Euclid {
namespace PhzQtUI {



DialogZRanges::DialogZRanges(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogZRanges)
    {
       ui->setupUi(this);
}

DialogZRanges::~DialogZRanges() {}




void DialogZRanges::refresh(){
  //cleaning
  size_t i = 0;
   for (auto child : ui->frame_z->children()) {
     if (i > 0) {
       delete child;
     }
     ++i;
   }



   for (size_t i=0; i<m_zs.size(); ++i){
     auto titleFrame = new QFrame();
     titleFrame->setFrameStyle(QFrame::Box);
     auto titleLayout = new QHBoxLayout();
     titleFrame->setLayout(titleLayout);
     auto txt_title = new QLineEdit(QString::number(m_zs[i]));
     titleLayout->addWidget(txt_title);

     if (i>0 && i<m_zs.size()-1){
       auto btn_del = new GridButton(i,0,"-");
       btn_del->setMaximumWidth(30);
       connect(btn_del,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onDeleteClicked(size_t,size_t)));
       titleLayout->addWidget(btn_del);
     }

     ui->verticalLayout_z->addWidget(titleFrame);
   }


}


void DialogZRanges::setRanges(std::vector<double> zs){
  m_zs = std::move(zs);

  size_t max_i = m_zs.size()-1;

  ui->txt_new_z->setValidator(new QDoubleValidator(m_zs[0],m_zs[max_i],2,this));

  refresh();

}



void DialogZRanges::on_btn_add_clicked(){
 double new_z = ui->txt_new_z->text().toDouble();

 if (new_z<=m_zs[0] || new_z>=m_zs[m_zs.size()-1]){
   QMessageBox::warning(this, "Out of range value...",
                                                           "Please provide a redshift value in the existing range.",
                                                              QMessageBox::Ok,QMessageBox::Ok);
       return;
 }

 for (auto& z : m_zs){
   if (new_z==z){
     QMessageBox::warning(this, "Duplicate Redshift...",
                                                         "Please provide a non-existing redshift value.",
                                                            QMessageBox::Ok,QMessageBox::Ok);
     return;
   }
 }

 auto z_iter = m_zs.begin();
 while(true){
   if (*z_iter>new_z){
     m_zs.insert(z_iter,new_z);
     break;
   }
   ++z_iter;
 }

 refresh();
 ui->txt_new_z->setText("");
}


void DialogZRanges::onDeleteClicked(size_t index,size_t ){
  size_t running=0;
  auto z_iter = m_zs.begin();
  while(true){
     if (running==index){
       m_zs.erase(z_iter);
       break;
     }
     ++running;
     ++z_iter;
   }

  refresh();
}



void DialogZRanges::on_btn_cancel_clicked(){
  reject();
}

void DialogZRanges::on_btn_save_clicked(){

  popupClosing(std::move(m_zs));
  accept();
}



}
}

