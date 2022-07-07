
#include <QMessageBox>

#include "PhzQtUI/DialogSelectParam.h"
#include "PhzQtUI/SedParamUtils.h"
#include "ui_DialogSelectParam.h"
#include <QStandardItemModel>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

DialogSelectParam::DialogSelectParam(const std::set<std::string>& param_list, QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogSelectParam), m_param_list{param_list} {
  ui->setupUi(this);

  QStandardItemModel* table_model = new QStandardItemModel();

  for (auto& param : m_param_list) {
    QStandardItem* item = new QStandardItem(QString::fromStdString(param));
    item->setCheckable(true);
    table_model->appendRow(item);
  }
  ui->table_param->setModel(table_model);
}

DialogSelectParam::~DialogSelectParam() {}

void DialogSelectParam::setParams(std::set<std::string> selected_param) {
  auto model = static_cast<QStandardItemModel*>(ui->table_param->model());
  for (auto& param : selected_param) {
    for (int i = 0; i < model->rowCount(); ++i) {
      auto item = model->item(i, 0);
      if (item->text() == QString::fromStdString(param)) {
        item->setCheckState(Qt::CheckState::Checked);
      }
    }
  }
}

void DialogSelectParam::on_btn_save_clicked() {

  std::vector<std::string> res;
  auto                     model = static_cast<QStandardItemModel*>(ui->table_param->model());
  for (int i = 0; i < model->rowCount(); ++i) {
    auto item = model->item(i, 0);
    if (item->checkState() == Qt::CheckState::Checked) {
      res.push_back(item->text().toStdString());
    }
  }

  popupClosing(res);
  accept();
}

void DialogSelectParam::on_btn_cancel_clicked() {
  reject();
}

}  // namespace PhzQtUI
}  // namespace Euclid
