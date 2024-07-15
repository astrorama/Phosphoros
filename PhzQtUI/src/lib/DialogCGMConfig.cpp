/*
 * DialogCGMConfig.cpp
 *
 *  Created on: 2024/07/11
 *      Author: fdubath
 */

#include "PhzQtUI/DialogCGMConfig.h"
#include "ui_DialogCGMConfig.h"

namespace Euclid {
namespace PhzQtUI {

DialogCGMConfig::DialogCGMConfig(QWidget* parent) : QDialog(parent), ui(new Ui::DialogCGMConfig) {
  ui->setupUi(this);
}

DialogCGMConfig::~DialogCGMConfig() {}

void DialogCGMConfig::setValues(double A, double a, double c) {
    m_A = A;
    ui->SB_A->setValue(m_A);
    m_a = a;
    ui->SB_a->setValue(m_a);
    m_c = c;
    ui->SB_c->setValue(m_c);
}

double DialogCGMConfig::get_A_param() const{
    return m_A;
}

double DialogCGMConfig::get_a_param() const{
    return m_a;
}

double DialogCGMConfig::get_c_param() const{
    return m_c;
}


void DialogCGMConfig::on_pb_def_A_clicked(){
    ui->SB_A->setValue(4.92919285);
}

void DialogCGMConfig::on_pb_def_a_clicked(){
    ui->SB_a->setValue(0.76313514);
}

void DialogCGMConfig::on_pb_def_c_clicked(){
    ui->SB_c->setValue(17.54936014);
}


void DialogCGMConfig::on_btn_cancel_clicked() {
  reject();
}

void DialogCGMConfig::on_btn_save_clicked() {
  m_A = ui->SB_A->value();
  m_a = ui->SB_a->value();
  m_c = ui->SB_c->value();
  accept();
}

}  // namespace PhzQtUI
}  // namespace Euclid
