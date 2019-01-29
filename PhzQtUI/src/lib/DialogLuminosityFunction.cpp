/*
 * DialogLuminosityFunction.cpp
 *
 *  Created on: September 2, 2015
 *      Author: fdubath
 */

#include "ui_DialogLuminosityFunction.h"
#include "PhzQtUI/DialogLuminosityFunction.h"

#include "FormUtils.h"
#include "PhzQtUI/LuminosityFunctionInfo.h"
#include "PhzQtUI/DialogLuminosityFunctionCurveSelector.h"


namespace Euclid {
namespace PhzQtUI {

DialogLuminosityFunction::DialogLuminosityFunction(DatasetRepo luminosity_repository, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogLuminosityFunction)
    {
       m_luminosity_repository = luminosity_repository;
       ui->setupUi(this);
       m_x=0;
       m_y=0;
}

DialogLuminosityFunction::~DialogLuminosityFunction() {}

void DialogLuminosityFunction::setInfo(LuminosityFunctionInfo info, size_t x, size_t y){
  m_FunctionInfo=std::move(info);
  m_x=x;
  m_y=y;

  //// setup the popup with the info values
  ui->lbl_groupName->setText(QString::fromStdString(m_FunctionInfo.sedGroupName));
  ui->lbl_redshift->setText(QString::number(m_FunctionInfo.z_min,'f', 2) +" - "+QString::number(m_FunctionInfo.z_max,'f', 2));

  // display the equation for magnitude or flux
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
    ui->lb_curve->setText(QString::fromStdString(m_FunctionInfo.curve_name));
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

}



void DialogLuminosityFunction::on_gb_schechter_clicked(){
  ui->gb_custom->setChecked(!ui->gb_schechter->isChecked());
}

void DialogLuminosityFunction::on_gb_custom_clicked(){
  ui->gb_schechter->setChecked(!ui->gb_custom->isChecked());
}

void DialogLuminosityFunction::on_btn_curve_clicked(){
  std::unique_ptr<DialogLuminosityFunctionCurveSelector> dialog(new DialogLuminosityFunctionCurveSelector(m_luminosity_repository));
  dialog->setCurve(ui->lb_curve->text().toStdString());

  connect(dialog.get(),
          SIGNAL(popupClosing(std::string)),
          this,
          SLOT(curvePopupClosing(std::string)));
  dialog->exec();
}

void DialogLuminosityFunction::curvePopupClosing(std::string curve){
  ui->lb_curve->setText(QString::fromStdString(curve));
}

void DialogLuminosityFunction::on_btn_cancel_clicked(){
  reject();
}

void DialogLuminosityFunction::on_btn_save_clicked(){
  // Build the FunctionInfo from the user input.
  m_FunctionInfo.is_custom=ui->gb_custom->isChecked();
  m_FunctionInfo.curve_name=ui->lb_curve->text().toStdString();
  m_FunctionInfo.alpha=FormUtils::parseToDouble(ui->txt_alpha->text());
  m_FunctionInfo.l=FormUtils::parseToDouble(ui->txt_L->text());
  m_FunctionInfo.m=FormUtils::parseToDouble(ui->txt_M->text());
  m_FunctionInfo.phi=FormUtils::parseToDouble(ui->txt_phi->text());

  // return the info to the caller with the coordinates
  popupClosing(m_FunctionInfo,m_x,m_y);
  accept();
}

}
}

