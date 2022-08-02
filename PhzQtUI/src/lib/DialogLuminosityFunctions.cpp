
#include "PhzQtUI/DialogLuminosityFunctions.h"
#include "FileUtils.h"
#include "ui_DialogLuminosityFunctions.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QStandardItemModel>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

DialogLuminosityFunctions::DialogLuminosityFunctions(QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogLuminosityFunctions) {
  ui->setupUi(this);
}

DialogLuminosityFunctions::~DialogLuminosityFunctions() {}

void DialogLuminosityFunctions::setInfos(std::vector<std::vector<LuminosityFunctionInfo>> infos,
                                         std::vector<LuminosityPriorConfig::SedGroup> groups, std::vector<double> zs) {

  m_infos  = std::move(infos);
  m_groups = std::move(groups);
  m_zs     = std::move(zs);

  QString             column_name = ((m_infos[0][0].in_mag) ? "M*" : "L*");
  QStandardItemModel* grid_model  = new QStandardItemModel();
  grid_model->setColumnCount(5);

  QStringList setHeaders;
  setHeaders << "Group"
             << "Z range"
             << "alpha"
             << "phi" << column_name;
  grid_model->setHorizontalHeaderLabels(setHeaders);

  for (size_t group_id = 0; group_id < m_groups.size(); ++group_id) {
    std::string group_name = m_groups[group_id].first;
    for (size_t range_id = 0; range_id < m_zs.size() - 1; ++range_id) {
      QString range_name = QString::number(m_zs[range_id], 'f', 2) + "-" + QString::number(m_zs[range_id + 1], 'f', 2);
      QList<QStandardItem*> items;
      QStandardItem*        item_group_name = new QStandardItem(QString::fromStdString(group_name));
      item_group_name->setEditable(false);
      items.push_back(item_group_name);
      QStandardItem* item_range_name = new QStandardItem(range_name);
      item_range_name->setEditable(false);
      items.push_back(item_range_name);

      auto&          info       = m_infos[range_id][group_id];
      QStandardItem* item_alpha = new QStandardItem(QString::number(info.alpha));
      items.push_back(item_alpha);
      QStandardItem* item_phi = new QStandardItem(QString::number(info.phi));
      items.push_back(item_phi);
      QStandardItem* item_ML = new QStandardItem(QString::number((info.in_mag) ? info.m : info.l));
      items.push_back(item_ML);
      grid_model->appendRow(items);
    }
  }

  ui->table_functions->setModel(grid_model);
}

void DialogLuminosityFunctions::on_btn_cancel_clicked() {
  reject();
}

void DialogLuminosityFunctions::on_btn_save_clicked() {

  for (size_t group_id = 0; group_id < m_groups.size(); ++group_id) {
    for (size_t range_id = 0; range_id < m_zs.size() - 1; ++range_id) {
      auto row_id = group_id * (m_zs.size() - 1) + range_id;

      auto& info = m_infos[range_id][group_id];

      QString alpha = ui->table_functions->model()->data(ui->table_functions->model()->index(row_id, 2)).toString();
      QString phi   = ui->table_functions->model()->data(ui->table_functions->model()->index(row_id, 3)).toString();
      QString m_l   = ui->table_functions->model()->data(ui->table_functions->model()->index(row_id, 4)).toString();

      QLocale locale{};
      bool    alpha_ok;
      double  d_alpha = locale.toDouble(alpha, &alpha_ok);

      bool   phi_ok;
      double d_phi = locale.toDouble(phi, &phi_ok);

      bool   m_l_ok;
      double d_m_l = locale.toDouble(m_l, &m_l_ok);

      if (alpha_ok) {
        info.alpha = d_alpha;
      }

      if (phi_ok) {
        info.phi = d_phi;
      }

      if (m_l_ok) {
        ((info.in_mag) ? info.m : info.l) = d_m_l;
      }
    }
  }
  popupClosing(m_infos);
  accept();
}

}  // namespace PhzQtUI
}  // namespace Euclid
