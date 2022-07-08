/*
 * DialogNz.cpp
 *
 *  Created on: 2019-09-13
 *      Author: fdubath
 */

#include "PhzQtUI/DialogNz.h"
#include "PhzQtUI/DialogFilterSelector.h"
#include "XYDataset/QualifiedName.h"
#include "ui_DialogNz.h"
#include <QMessageBox>
#include <QString>
#include <memory>

namespace Euclid {
namespace PhzQtUI {

DialogNz::DialogNz(std::vector<FilterMapping> filters, DatasetRepo filter_repository, std::string b_filter,
                   std::string i_filter, QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogNz), m_filters{filters}, m_b_filter{b_filter}, m_i_filter{i_filter} {

  ui->setupUi(this);

  m_filter_repository = filter_repository;

  // fill the CB
  for (auto& filter : m_filters) {
    ui->cb_i_filter->addItem(QString::fromStdString(filter.getFilterFile()));
  }

  // set the current values
  ui->lbl_b_filter->setText(QString::fromStdString(m_b_filter));

  bool found = false;
  for (int i = 0; i < ui->cb_i_filter->count(); i++) {
    if (ui->cb_i_filter->itemText(i).toStdString() == m_i_filter) {
      ui->cb_i_filter->setCurrentIndex(i);
      found = true;
      break;
    }
  }

  if (!found && ui->cb_i_filter->count() > 0) {
    ui->cb_i_filter->setCurrentIndex(0);
  }
}

DialogNz::~DialogNz() {}

void DialogNz::filterPopupClosing(std::string filter) {
  m_b_filter = filter;
  ui->lbl_b_filter->setText(QString::fromStdString(m_b_filter));
}

void DialogNz::on_btn_select_filter_clicked() {
  std::unique_ptr<DialogFilterSelector> dialog(new DialogFilterSelector(m_filter_repository));
  dialog->setFilter(m_b_filter);

  connect(dialog.get(), SIGNAL(popupClosing(std::string)), this, SLOT(filterPopupClosing(std::string)));
  dialog->exec();
}

void DialogNz::on_btn_cancel_clicked() {
  reject();
}

void DialogNz::on_btn_save_clicked() {
  popupClosing(m_b_filter, ui->cb_i_filter->currentText().toStdString());
  accept();
}

}  // namespace PhzQtUI
}  // namespace Euclid
