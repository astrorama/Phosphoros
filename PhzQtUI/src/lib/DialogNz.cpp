/*
 * DialogNz.cpp
 *
 *  Created on: 2019-09-13
 *      Author: fdubath
 */


#include <QString>
#include <QMessageBox>
#include "PhzQtUI/DialogNz.h"
#include "ui_DialogNz.h"
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzQtUI {

DialogNz::DialogNz(std::vector<FilterMapping> filters, QWidget *parent):
QDialog(parent), ui(new Ui::DialogNz), m_filters{filters} {

  ui->setupUi(this);
  //fill the CBs
  for (auto& filter : m_filters) {
        ui->cb_b_filter->addItem(QString::fromStdString(filter.getFilterFile()));
        ui->cb_i_filter->addItem(QString::fromStdString(filter.getFilterFile()));
    }

}

DialogNz::~DialogNz() {


}


void DialogNz::on_btn_cancel_clicked(){
  reject();
}

void DialogNz::on_btn_save_clicked(){
  popupClosing(ui->cb_b_filter->currentText().toStdString(), ui->cb_i_filter->currentText().toStdString());
  accept();
}



}
}

