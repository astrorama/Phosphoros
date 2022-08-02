/*
 * DialogLuminosityPrior.cpp

 *
 *  Created on: Sept 2, 2015
 *      Author: fdubath
 */

#include <QDir>
#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTextStream>
#include <boost/program_options.hpp>

#include "ElementsKernel/Logging.h"

#include "PhzQtUI/DialogLuminosityPrior.h"
#include "ui_DialogLuminosityPrior.h"

#include "FileUtils.h"
#include "PhzQtUI/DialogLuminosityFunction.h"
#include "PhzQtUI/DialogLuminositySedGroup.h"
#include "PhzQtUI/DialogZRanges.h"
#include "PhzQtUI/LuminosityFunctionInfo.h"

#include "Configuration/ConfigManager.h"
#include "DefaultOptionsCompleter.h"

#include "PhzQtUI/DialogLuminosityFunctions.h"

namespace Euclid {
namespace PhzQtUI {

namespace po = boost::program_options;

static Elements::Logging dialog_logger = Elements::Logging::getLogger("DialogLuminosityPrior");

DialogLuminosityPrior::DialogLuminosityPrior(std::string filter, DatasetRepo luminosity_repository, QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogLuminosityPrior) {
  m_filter                = filter;
  m_luminosity_repository = luminosity_repository;
  ui->setupUi(this);
  ui->frame_Luminosity->setStyleSheet("background-color: white ");
  m_z_min = 0.;
  m_z_max = 3000.;
  ui->lb_filter->setText(QString::fromStdString(m_filter));
}

DialogLuminosityPrior::~DialogLuminosityPrior() {}

void DialogLuminosityPrior::loadData(ModelSet model, std::string survey_name, std::string model_grid, double z_min,
                                     double z_max) {
  m_config_folder = QString::fromStdString(FileUtils::getGUILuminosityPriorConfig(true, survey_name, model.getName()));
  m_prior_configs = LuminosityPriorConfig::readFolder(m_config_folder.toStdString());
  m_model         = std::move(model);
  m_survey_name   = survey_name;
  m_model_grid_name = model_grid;
  m_z_min           = z_min;
  m_z_max           = z_max;

  // load the main grid
  loadMainGrid();
  manageBtnEnability(false, false);

  disconnect(ui->table_priors, SIGNAL(doubleClicked(QModelIndex)), 0, 0);
  connect(ui->table_priors, SIGNAL(doubleClicked(QModelIndex)), SLOT(priorGridDoubleClicked(QModelIndex)));
}

void DialogLuminosityPrior::priorSelectionChanged(QModelIndex new_index, QModelIndex) {

  size_t      row  = new_index.row();
  std::string name = new_index.sibling(row, 1).data().toString().toStdString();

  // Update the UI by setting the value for the selected prior
  auto config = m_prior_configs.at(name);
  ui->txt_name->setText(QString::fromStdString(config.getName()));

  ui->cb_unit->setCurrentIndex(0);

  clearGrid();
  m_groups          = config.getSedGRoups();
  m_zs              = config.getZs();
  m_luminosityInfos = std::move(config.getLuminosityFunctionArray());
  loadGrid();

  if (!config.getInMag()) {
    ui->cb_unit->setCurrentIndex(1);
  }
  manageBtnEnability(false, false, true);
}

void DialogLuminosityPrior::on_btn_new_clicked() {

  // create a new prior config...
  LuminosityPriorConfig new_config{};
  new_config.setName("<New_Luminosity_Prior>");
  new_config.setZs({m_z_min, m_z_max});

  // ...with a single group containing all the SEDs...
  std::vector<std::string> seds{};
  for (auto& item : m_model.getSeds()) {
    seds.push_back({item});
  }

  LuminosityPriorConfig::SedGroup group{"SEDs", seds};
  new_config.setSedGroups({group});

  LuminosityFunctionInfo info{};
  info.z_min        = m_z_min;
  info.z_max        = m_z_max;
  info.sedGroupName = "SEDs";
  new_config.setLuminosityFunctionList({info});

  // ... add it to the popup list
  m_prior_configs["<New_Luminosity_Prior>"] = new_config;

  // reload the prior grid (so the new prior appear)
  loadMainGrid();

  // select the new prior
  for (size_t i = 0; i < m_prior_configs.size(); ++i) {
    std::string name = ui->table_priors->model()->data(ui->table_priors->model()->index(i, 1)).toString().toStdString();

    if (name == "<New_Luminosity_Prior>") {
      ui->table_priors->selectRow(i);
      break;
    }
  }

  // turn the controls in edition
  manageBtnEnability(true, false);
  m_new = true;
}

void DialogLuminosityPrior::on_btn_duplicate_clicked() {
  // Get the parent (selected) prior name and grid name.
  QItemSelectionModel* select    = ui->table_priors->selectionModel();
  auto                 row_index = select->selectedRows().at(0);
  size_t               row       = row_index.row();
  std::string          name      = row_index.sibling(row, 1).data().toString().toStdString();

  // select the new name
  std::string new_name = name + "(Copy)";
  QString     new_file = m_config_folder + QDir::separator() + QString::fromStdString(new_name) + ".xml";
  size_t      i        = 2;
  while (QFile::exists(new_file)) {
    new_name = name + "(Copy " + QString::number(i).toStdString() + ")";
    new_file = m_config_folder + QDir::separator() + QString::fromStdString(new_name) + ".xml";
  }

  // copy the prior
  auto new_prior = m_prior_configs[name];
  new_prior.setName(new_name);

  // add it to the local list
  m_prior_configs[new_name] = new_prior;

  // reload the popup prior grid
  loadMainGrid();

  // select the copied prior

  for (int row_id = 0; row_id < ui->table_priors->model()->rowCount(); ++row_id) {
    auto curr_name = ui->table_priors->model()->data(ui->table_priors->model()->index(row_id, 1)).toString();
    if (curr_name.toStdString() == new_name) {
      ui->table_priors->selectRow(row_id);
      // start edition
      manageBtnEnability(true, false);
      break;
    }
  }
}

void DialogLuminosityPrior::on_btn_delete_clicked() {

  // Get the victim (selected) prior name and grid name.
  QItemSelectionModel* select    = ui->table_priors->selectionModel();
  auto                 row_index = select->selectedRows().at(0);
  size_t               row       = row_index.row();
  std::string          name      = row_index.sibling(row, 1).data().toString().toStdString();

  // delete the prior from the popup list
  m_prior_configs.erase(name);

  // delete the configuration file
  QFile::remove(m_config_folder + QDir::separator() + QString::fromStdString(name) + ".xml");

  // reload the popup prior grid
  loadMainGrid();

  // select the last prior of the grid
  if (m_prior_configs.size() > 0) {
    ui->table_priors->selectRow(m_prior_configs.size() - 1);
  } else {  //  or Clear the controls from the current information
    ui->txt_name->setText("");
    ui->cb_unit->setCurrentIndex(0);
  }

  // end edition in the controls
  manageBtnEnability(false, false);
}

void DialogLuminosityPrior::on_btn_edit_clicked() {
  manageBtnEnability(true, false);
}

void DialogLuminosityPrior::priorGridDoubleClicked(QModelIndex) {
  manageBtnEnability(true, false);
}

void DialogLuminosityPrior::onProgress(int current, int total) {
  ui->progressBar->setValue((100 * current) / total);
}

void DialogLuminosityPrior::on_btn_save_clicked() {

  QItemSelectionModel* select    = ui->table_priors->selectionModel();
  auto                 row_index = select->selectedRows().at(0);
  size_t               row       = row_index.row();
  std::string          name      = row_index.sibling(row, 1).data().toString().toStdString();
  auto                 info      = m_prior_configs[name];

  if (!validateInput(row)) {
    return;
  }

  updateInfo(info);

  updatePriorRow(row_index, row, info);

  m_prior_configs.erase(name);

  m_prior_configs[info.getName()] = info;

  // config file handling

  if (!m_new) {
    QFile::remove(m_config_folder + QDir::separator() + QString::fromStdString(name) + ".xml");
  }

  QFile file(m_config_folder + QDir::separator() + QString::fromStdString(info.getName()) + ".xml");
  file.open(QIODevice::WriteOnly);
  QTextStream stream(&file);
  QString     xml = info.serialize().toString();
  stream << xml;
  file.close();

  manageBtnEnability(false, false);

  m_new = false;
}

void DialogLuminosityPrior::on_btn_cancel_clicked() {
  if (m_new) {
    // clear the controls
    ui->txt_name->setText("");
    ui->cb_unit->setCurrentIndex(0);
    // clearGrid();

    // delete the record in the popup list
    QItemSelectionModel* select    = ui->table_priors->selectionModel();
    auto                 row_index = select->selectedRows().at(0);
    size_t               row       = row_index.row();
    std::string          name      = row_index.sibling(row, 1).data().toString().toStdString();
    m_prior_configs.erase(name);
  }

  loadMainGrid();

  if (m_prior_configs.size() > 0) {
    ui->table_priors->selectRow(m_prior_configs.size() - 1);
  }
  manageBtnEnability(false, false);
  m_new = false;
}

void DialogLuminosityPrior::on_cb_unit_currentIndexChanged(const QString&) {
  bool is_mag = ui->cb_unit->currentIndex() == 0;

  // swap the luminosity function from MAG to FLUX or conversly
  for (auto& funct_vect : m_luminosityInfos) {
    for (auto& funct : funct_vect) {
      funct.in_mag = is_mag;
    }
  }

  clearGrid();
  loadGrid();
}

void DialogLuminosityPrior::on_btn_group_clicked() {
  std::unique_ptr<DialogLuminositySedGroup> dialog(new DialogLuminositySedGroup());
  dialog->setGroups(m_groups);

  QItemSelectionModel* select    = ui->table_priors->selectionModel();
  auto                 row_index = select->selectedRows().at(0);
  size_t               row       = row_index.row();
  std::string          name      = row_index.sibling(row, 1).data().toString().toStdString();
  auto                 info      = m_prior_configs[name];
  auto                 sed_diff  = info.isCompatibleWithSeds(m_model.getSeds());
  dialog->setDiff(sed_diff.first, sed_diff.second);
  connect(dialog.get(), SIGNAL(popupClosing(std::vector<LuminosityPriorConfig::SedGroup>)), this,
          SLOT(groupPopupClosing(std::vector<LuminosityPriorConfig::SedGroup>)));
  dialog->exec();
}

void DialogLuminosityPrior::groupPopupClosing(std::vector<LuminosityPriorConfig::SedGroup> groups) {
  bool is_mag = ui->cb_unit->currentIndex() == 0;

  // update function,clear grid redraw
  if (m_groups.size() < groups.size()) {
    for (size_t i = 0; i < m_zs.size() - 1; ++i) {
      for (size_t j = m_groups.size(); j < groups.size(); ++j) {
        m_luminosityInfos[i].push_back(LuminosityFunctionInfo{});
        m_luminosityInfos[i][j].z_min  = m_zs[i];
        m_luminosityInfos[i][j].z_max  = m_zs[i + 1];
        m_luminosityInfos[i][j].in_mag = is_mag;
      }
    }
  } else if (m_groups.size() > groups.size()) {
    for (size_t i = 0; i < m_zs.size() - 1; ++i) {
      auto iter = m_luminosityInfos[i].begin();

      for (size_t j = 0; j < groups.size(); ++j) {
        ++iter;
      }
      m_luminosityInfos[i].erase(iter, m_luminosityInfos[i].end());
    }
  }

  m_groups = std::move(groups);

  for (size_t i = 0; i < m_zs.size() - 1; ++i) {
    for (size_t j = 0; j < m_groups.size(); ++j) {
      m_luminosityInfos[i][j].sedGroupName = m_groups[j].first;
    }
  }

  clearGrid();
  loadGrid();
}

void DialogLuminosityPrior::on_btn_z_clicked() {
  std::unique_ptr<DialogZRanges> dialog(new DialogZRanges());
  dialog->setRanges(m_zs);
  dialog->setMinMax(m_z_min, m_z_max);

  connect(dialog.get(), SIGNAL(popupClosing(std::vector<double>)), this, SLOT(zPopupClosing(std::vector<double>)));
  dialog->exec();
}

void DialogLuminosityPrior::zPopupClosing(std::vector<double> zs) {
  bool is_mag = ui->cb_unit->currentIndex() == 0;

  // update function,clear grid redraw
  if (m_zs.size() < zs.size()) {
    for (size_t i = m_zs.size() - 1; i < zs.size() - 1; ++i) {
      m_luminosityInfos.push_back({});
      for (size_t j = 0; j < m_groups.size(); ++j) {
        m_luminosityInfos[i].push_back(LuminosityFunctionInfo{});
        m_luminosityInfos[i][j].in_mag       = is_mag;
        m_luminosityInfos[i][j].sedGroupName = m_groups[j].first;
      }
    }
  } else if (m_zs.size() > zs.size()) {
    auto iter = m_luminosityInfos.begin();
    for (size_t i = 0; i < zs.size() - 1; ++i) {
      ++iter;
    }

    m_luminosityInfos.erase(iter, m_luminosityInfos.end());
  }

  m_zs = std::move(zs);

  for (size_t i = 0; i < m_zs.size() - 1; ++i) {
    for (size_t j = 0; j < m_groups.size(); ++j) {
      m_luminosityInfos[i][j].z_min = m_zs[i];
      m_luminosityInfos[i][j].z_max = m_zs[i + 1];
    }
  }

  clearGrid();
  loadGrid();
}

void DialogLuminosityPrior::onGridButtonClicked(size_t x, size_t y) {
  std::unique_ptr<DialogLuminosityFunction> dialog(new DialogLuminosityFunction(m_luminosity_repository));
  dialog->setInfo(m_luminosityInfos[x][y], x, y);
  connect(dialog.get(), SIGNAL(popupClosing(LuminosityFunctionInfo, size_t, size_t)), this,
          SLOT(luminosityFunctionPopupClosing(LuminosityFunctionInfo, size_t, size_t)));
  dialog->exec();
}

void DialogLuminosityPrior::luminosityFunctionPopupClosing(LuminosityFunctionInfo info, size_t x, size_t y) {
  m_luminosityInfos[x][y] = info;

  auto layoutItem = ui->gl_Luminosity->itemAtPosition(y + 1, x + 1);

  if (info.isComplete()) {
    layoutItem->widget()->setStyleSheet("background-color: white ");
    static_cast<GridButton*>(layoutItem->widget()->children()[1])->setText(info.getDescription());
  } else {
    layoutItem->widget()->setStyleSheet("background-color: red ");
    static_cast<GridButton*>(layoutItem->widget()->children()[1])->setText("To be defined");
  }
}

void DialogLuminosityPrior::on_btn_close_clicked() {
  accept();
}

void DialogLuminosityPrior::manageBtnEnability(bool in_edition, bool read_only, bool has_selected_row) {

  ui->btn_new->setEnabled(!read_only && !in_edition);
  ui->btn_delete->setEnabled(!read_only && !in_edition && has_selected_row);
  ui->btn_duplicate->setEnabled(!read_only && !in_edition && has_selected_row);

  ui->btn_edit->setEnabled(!read_only && !in_edition && has_selected_row);
  ui->btn_cancel->setEnabled(!read_only && in_edition && has_selected_row);
  ui->btn_save->setEnabled(!read_only && in_edition && has_selected_row);

  ui->btn_close->setEnabled(!read_only && !in_edition);

  ui->table_priors->setEnabled(!read_only && !in_edition);
  ui->txt_name->setEnabled(!read_only && in_edition);
  ui->cb_unit->setEnabled(!read_only && in_edition);
  ui->btn_group->setEnabled(!read_only && in_edition);
  ui->btn_z->setEnabled(!read_only && in_edition);

  ui->bulk_btn->setEnabled(!read_only && in_edition);

  for (auto button : m_grid_buttons) {
    button->setEnabled(in_edition);
  }
}

void DialogLuminosityPrior::manageBtnEnability(bool in_edition, bool read_only) {
  QItemSelectionModel* select = ui->table_priors->selectionModel();
  manageBtnEnability(in_edition, read_only, select->hasSelection());
}

void DialogLuminosityPrior::loadMainGrid() {
  QStandardItemModel* grid_model = new QStandardItemModel();
  grid_model->setColumnCount(5);

  QStringList setHeaders;
  setHeaders << "Id"
             << "Name"
             << "Unit"
             << "Regions"
             << "Message";
  grid_model->setHorizontalHeaderLabels(setHeaders);

  size_t id = 0;
  for (auto& config_pair : m_prior_configs) {

    bool                  z_ok           = config_pair.second.isCompatibleWithZ(m_z_min, m_z_max);
    auto                  sed_diff       = config_pair.second.isCompatibleWithSeds(m_model.getSeds());
    bool                  param_space_ok = z_ok && sed_diff.first.size() == 0 && sed_diff.second.size() == 0;
    QList<QStandardItem*> items;

    QStandardItem* item_id = new QStandardItem(QString::number(id));
    items.push_back(item_id);

    QStandardItem* item_name = new QStandardItem(QString::fromStdString(config_pair.second.getName()));
    if (!param_space_ok) {
      item_name->setData(QColor("orange"), Qt::BackgroundRole);
    }
    items.push_back(item_name);

    QString mag_string = "MAGNITUDE";
    if (!config_pair.second.getInMag()) {
      mag_string = "FLUX";
    }
    QStandardItem* item_tp = new QStandardItem(mag_string);
    if (!param_space_ok) {
      item_tp->setData(QColor("orange"), Qt::BackgroundRole);
    }
    items.push_back(item_tp);

    QStandardItem* item_nb = new QStandardItem(QString::number(config_pair.second.getLuminosityFunctionList().size()));
    if (!param_space_ok) {
      item_nb->setData(QColor("orange"), Qt::BackgroundRole);
    }
    items.push_back(item_nb);

    QString message = "";
    if (!z_ok) {
      message = message + "Incompatibility in the redshift range, please update it.";
    }

    if (sed_diff.first.size() != 0 || sed_diff.second.size() != 0) {
      message = message + "Incompatibility in the SED's list, please update it.";
    }

    QStandardItem* item_message = new QStandardItem(message);
    if (!param_space_ok) {
      item_message->setData(QColor("orange"), Qt::BackgroundRole);
    }
    items.push_back(item_message);

    grid_model->appendRow(items);
    ++id;
  }
  ui->table_priors->setModel(grid_model);

  connect(ui->table_priors->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
          SLOT(priorSelectionChanged(QModelIndex, QModelIndex)));

  ui->table_priors->setColumnHidden(0, true);
  ui->table_priors->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->table_priors->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->table_priors->resizeColumnsToContents();
  ui->table_priors->horizontalHeader()->setStretchLastSection(true);
}

void DialogLuminosityPrior::clearGrid() {
  size_t i = 0;
  for (auto child : ui->frame_Luminosity->children()) {
    if (i > 0) {
      delete child;
    }
    ++i;
  }
}

void DialogLuminosityPrior::loadGrid() {
  m_grid_buttons.clear();
  auto frame_1 = new QFrame();
  frame_1->setFrameStyle(QFrame::NoFrame);
  frame_1->setMinimumHeight(30);
  ui->gl_Luminosity->addWidget(frame_1, 0, 0);

  for (size_t i = 0; i < m_groups.size(); ++i) {
    auto frame_grp = new QFrame();
    frame_grp->setFrameStyle(QFrame::NoFrame);
    frame_grp->setMinimumHeight(30);
    auto layout = new QHBoxLayout();
    frame_grp->setLayout(layout);
    auto label = new QLabel(QString::fromStdString(m_groups[i].first));
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    ui->gl_Luminosity->addWidget(frame_grp, 1 + i, 0);
  }

  for (size_t i = 0; i < m_zs.size() - 1; ++i) {
    auto frame = new QFrame();
    frame->setFrameStyle(QFrame::NoFrame);
    frame->setMinimumHeight(30);
    auto layout = new QVBoxLayout();
    frame->setLayout(layout);
    auto label = new QLabel("z=" + QString::number(m_zs[i], 'f', 2) + " - " + QString::number(m_zs[i + 1], 'f', 2));
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    ui->gl_Luminosity->addWidget(frame, 0, 1 + i);
  }

  for (size_t i = 0; i < m_zs.size() - 1; ++i) {
    for (size_t j = 0; j < m_groups.size(); ++j) {
      auto frame = new QFrame();
      frame->setMinimumHeight(70);
      frame->setFrameStyle(QFrame::Box);
      if (!m_luminosityInfos[i][j].isComplete()) {
        frame->setStyleSheet("background-color: red ");
      }
      auto layout = new QVBoxLayout();
      frame->setLayout(layout);

      QString text_btn = "To be defined";
      if (m_luminosityInfos[i][j].isComplete()) {
        text_btn = m_luminosityInfos[i][j].getDescription();
      }
      GridButton* button = new GridButton(i, j, text_btn);
      button->setMinimumHeight(50);
      button->setStyleSheet("background-color: lightGrey ");
      connect(button, SIGNAL(GridButtonClicked(size_t, size_t)), this, SLOT(onGridButtonClicked(size_t, size_t)));
      layout->addWidget(button);
      m_grid_buttons.push_back(button);
      ui->gl_Luminosity->addWidget(frame, j + 1, i + 1);
    }
  }
}

bool DialogLuminosityPrior::validateInput(const size_t& current_index) {
  // Check the name
  QString new_name = ui->txt_name->text();
  if (new_name.compare("<New_Luminosity_Prior>") == 0 || new_name.length() == 0) {
    QMessageBox::warning(this, "Missing Name...", "Please provide a name for the Luminosity Prior.", QMessageBox::Ok,
                         QMessageBox::Ok);
    return false;
  }

  for (size_t i = 0; i < m_prior_configs.size(); ++i) {
    std::string old_name =
        ui->table_priors->model()->data(ui->table_priors->model()->index(i, 1)).toString().toStdString();
    if (i != current_index && old_name == new_name.toStdString()) {
      QMessageBox::warning(this, "Duplicate Name...",
                           "There is already a Luminosity Prior called '" + new_name +
                               "'. Please provide another name for the Luminosity Prior.",
                           QMessageBox::Ok, QMessageBox::Ok);
      return false;
    }
  }

  // check the functions
  for (auto& func_vect : m_luminosityInfos) {
    for (auto& func : func_vect) {
      if (!func.isComplete()) {
        QMessageBox::warning(this, "Missing Luminosity Function...",
                             "Please define all the luminosity functions in the grid.", QMessageBox::Ok,
                             QMessageBox::Ok);
        return false;
      }
    }
  }

  return true;
}

void DialogLuminosityPrior::updateInfo(LuminosityPriorConfig& info) {
  info.setInMag(ui->cb_unit->currentIndex() == 0);
  info.setZs(m_zs);
  info.setSedGroups(m_groups);

  std::vector<LuminosityFunctionInfo> list{};
  for (size_t i = 0; i < info.getZs().size() - 1; ++i) {
    for (size_t j = 0; j < info.getSedGRoups().size(); ++j) {
      list.push_back(m_luminosityInfos[i][j]);
    }
  }

  info.setLuminosityFunctionList(list);

  info.setName(ui->txt_name->text().toStdString());
}

void DialogLuminosityPrior::updatePriorRow(QModelIndex& index, const size_t& row, const LuminosityPriorConfig& info) {

  bool param_space_ok = info.isCompatibleWithParameterSpace(m_z_min, m_z_max, m_model.getSeds());

  if (param_space_ok) {
    ui->table_priors->model()->setData(index.sibling(row, 5), "");
    ui->table_priors->model()->setData(index.sibling(row, 1), QColor("white"), Qt::BackgroundRole);
    ui->table_priors->model()->setData(index.sibling(row, 2), QColor("white"), Qt::BackgroundRole);
    ui->table_priors->model()->setData(index.sibling(row, 3), QColor("white"), Qt::BackgroundRole);
    ui->table_priors->model()->setData(index.sibling(row, 4), QColor("white"), Qt::BackgroundRole);
  }

  ui->table_priors->model()->setData(index.sibling(row, 1), ui->txt_name->text());

  if (info.getInMag()) {
    ui->table_priors->model()->setData(index.sibling(row, 2), QString::fromStdString("MAGNITUDE"));
  } else {
    ui->table_priors->model()->setData(index.sibling(row, 2), QString::fromStdString("FLUX"));
  }

  ui->table_priors->model()->setData(index.sibling(row, 3),
                                     QString::number((info.getZs().size() - 1) * info.getSedGRoups().size()));
  ui->table_priors->resizeColumnsToContents();
  ui->table_priors->horizontalHeader()->setStretchLastSection(true);
}

void DialogLuminosityPrior::on_bulk_btn_clicked() {
  std::unique_ptr<DialogLuminosityFunctions> dialog(new DialogLuminosityFunctions());
  dialog->setInfos(m_luminosityInfos, m_groups, m_zs);
  connect(dialog.get(), SIGNAL(popupClosing(std::vector<std::vector<LuminosityFunctionInfo>>)), this,
          SLOT(luminosityFunctionsPopupClosing(std::vector<std::vector<LuminosityFunctionInfo>>)));
  dialog->exec();
}

void DialogLuminosityPrior::luminosityFunctionsPopupClosing(std::vector<std::vector<LuminosityFunctionInfo>> infos) {
  m_luminosityInfos = infos;
  clearGrid();
  loadGrid();
}

}  // namespace PhzQtUI
}  // namespace Euclid
