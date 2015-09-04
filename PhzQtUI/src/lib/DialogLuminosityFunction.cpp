/*
 * DialogLuminosityFunction.cpp
 *
 *  Created on: September 2, 2015
 *      Author: fdubath
 */
#include <QFuture>
#include <qtconcurrentrun.h>
#include <QDir>
#include <QMessageBox>
#include "PhzQtUI/DialogLuminosityFunction.h"
#include <boost/program_options.hpp>
#include "ui_DialogLuminosityFunction.h"
#include <QLabel>

#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/LuminosityFunctionInfo.h"

// #include <future>
//http://stackoverflow.com/questions/4412796/qt-qtableview-clickable-button-in-table-row
//assume 2x3 layout => 3 row 4 columns

namespace Euclid {
namespace PhzQtUI {



DialogLuminosityFunction::DialogLuminosityFunction(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogLuminosityFunction)
    {
       ui->setupUi(this);
}

DialogLuminosityFunction::~DialogLuminosityFunction() {}

void DialogLuminosityFunction::setInfo(LuminosityFunctionInfo info, int x, int y){
  m_FunctionInfo=std::move(info);
  m_x=x;
  m_y=y;

  ui->lbl_groupName->setText(QString::fromStdString(m_FunctionInfo.sedGroupName));
  ui->lbl_redshift->setText(QString::number(m_FunctionInfo.z_min,'f', 2) +" - "+QString::number(m_FunctionInfo.z_max,'f', 2));

  if (m_FunctionInfo.in_mag){
    ui->lbl_equation_L->setVisible(false);
    ui->lbl_L->setVisible(false);
    ui->txt_L->setVisible(false);
  } else {
    ui->lbl_equation_M->setVisible(false);
    ui->lbl_M->setVisible(false);
    ui->txt_M->setVisible(false);
  }

  if (m_FunctionInfo.is_custom){
    ui->gb_schechter->setChecked(false);
  } else {
    ui->gb_custom->setChecked(false);
  }

  ui->txt_alpha->setValidator(new QDoubleValidator(this));
  ui->txt_alpha->setText(QString::number(m_FunctionInfo.alpha));


  ui->txt_L->setValidator(new QDoubleValidator(this));
  ui->txt_L->setText(QString::number(m_FunctionInfo.l));

  ui->txt_M->setValidator(new QDoubleValidator(this));
  ui->txt_M->setText(QString::number(m_FunctionInfo.m));

  ui->txt_phi->setValidator(new QDoubleValidator(this));
  ui->txt_phi->setText(QString::number(m_FunctionInfo.phi));

  // load combo box and set initial value if any

}



void DialogLuminosityFunction::on_gb_schechter_clicked(){
  ui->gb_custom->setChecked(!ui->gb_schechter->isChecked());
}

void DialogLuminosityFunction::on_gb_custom_clicked(){
  ui->gb_schechter->setChecked(!ui->gb_custom->isChecked());
}


void DialogLuminosityFunction::on_btn_cancel_clicked(){
  reject();
}

void DialogLuminosityFunction::on_btn_save_clicked(){

  m_FunctionInfo.is_custom=ui->gb_custom->isChecked();

  m_FunctionInfo.curve_name=ui->cb_curves->currentText().toStdString();

  m_FunctionInfo.alpha=ui->txt_alpha->text().toDouble();
  m_FunctionInfo.l=ui->txt_L->text().toDouble();
  m_FunctionInfo.m=ui->txt_M->text().toDouble();
  m_FunctionInfo.phi=ui->txt_phi->text().toDouble();

  popupClosing(m_FunctionInfo,m_x,m_y);
  accept();
}



}
}

