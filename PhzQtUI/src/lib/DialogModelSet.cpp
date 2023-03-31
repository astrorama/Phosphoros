
#include "PhzQtUI/DialogModelSet.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include "FormUtils.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "PhzQtUI/SedTreeModel.h"
#include "ui_DialogModelSet.h"
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QStandardItemModel>
#include <algorithm>

#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/MessageButton.h"
#include "PhzQtUI/ParameterRule.h"

#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"

#include <QProgressDialog>
#include <chrono>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("DialogModelSet");

void DialogModelSet::loadSeds() {
  DataSetTreeModel* treeModel_sed = new DataSetTreeModel(m_seds_repository);
  treeModel_sed->load();
  ui->treeView_Sed->setModel(treeModel_sed);
  ui->treeView_Sed->expandAll();
  ui->treeView_Sed->hideColumn(1);
  ui->treeView_Sed->hideColumn(2);
  ui->treeView_Sed->resizeColumnToContents(0);
  ui->treeView_Sed->collapseAll();

  connect(treeModel_sed, SIGNAL(itemChanged(QStandardItem*)), treeModel_sed, SLOT(onItemChanged(QStandardItem*)));

  if (treeModel_sed->rowCount() == 0) {
    QMessageBox::warning(this, "No available SED...",
                         "There is no SED to select. "
                         "You can provide and manage SEDs in the \"Configuration/Aux. Data\" page.",
                         QMessageBox::Ok);
  }
}

DialogModelSet::DialogModelSet(DatasetRepo seds_repository, DatasetRepo redenig_curves_repository, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogModelSet)
    , m_seds_repository(seds_repository)
    , m_redenig_curves_repository(redenig_curves_repository) {
  ui->setupUi(this);

  loadSeds();

  DataSetTreeModel* treeModel_red = new DataSetTreeModel(m_redenig_curves_repository);
  treeModel_red->load();
  ui->treeView_Reddening->setModel(treeModel_red);
  ui->treeView_Reddening->expandAll();
  ui->treeView_Reddening->hideColumn(1);
  ui->treeView_Reddening->resizeColumnToContents(0);
  ui->treeView_Reddening->collapseAll();

  connect(treeModel_red, SIGNAL(itemChanged(QStandardItem*)), treeModel_red, SLOT(onItemChanged(QStandardItem*)));

  if (treeModel_red->rowCount() == 0) {
    QMessageBox::warning(this, "No available Reddening Curve...",
                         "There is no reddening curve to select. "
                         "You can provide and manage reddening curves in the \"Configuration/Aux. Data\" page.",
                         QMessageBox::Ok);
  }
}

DialogModelSet::~DialogModelSet() {}

void DialogModelSet::sedProcessStarted() {
  ui->labelMessage->setText("Adding emission Lines to the SEDs...");
  for (auto button : m_message_buttons) {
    button->setEnabled(false);
  }
}

void DialogModelSet::sedProcessfinished(int, QProcess::ExitStatus) {
  // remove the buttons
  for (auto button : m_message_buttons) {
    delete button;
  }

  m_message_buttons = std::vector<MessageButton*>();

  // reload the provider and the model
  std::unique_ptr<XYDataset::FileParser>         sed_file_parser{new XYDataset::AsciiParser{}};
  std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(
      new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(true), std::move(sed_file_parser)});
  m_seds_repository->resetProvider(std::move(sed_provider));

  loadSeds();
  if (m_view_popup) {
    static_cast<SedTreeModel*>(ui->treeView_Sed->model())->setEnabled(false);
  }
  ui->labelMessage->setText("Processing of SEDs completed.");
}

void DialogModelSet::addEmissionLineButtonClicked(const QString& group) {
  if (QMessageBox::question(this, "Add emission lines to SEDs in a folder...",
                            QString::fromStdString("This action will create a new folder named ") + group +
                                QString::fromStdString("_el generated from SEDs from folder ") + group +
                                QString::fromStdString(" with added emission lines?"),
                            QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
    // do the procesing
    QProcess* lineAdder = new QProcess();
    lineAdder->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    auto    aux_path = FileUtils::getAuxRootPath();
    QString command  = QString::fromStdString("PhosphorosAddEmissionLines --sed-dir " + aux_path) + QDir::separator() +
                      QString::fromStdString("SEDs") + QDir::separator() + group;

    connect(lineAdder, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(sedProcessfinished(int, QProcess::ExitStatus)));
    connect(lineAdder, SIGNAL(started()), this, SLOT(sedProcessStarted()));

    lineAdder->start(command);
  } else {
    ui->labelMessage->setText("");
  }
}

void DialogModelSet::setViewMode() {
  m_view_popup = true;
  turnControlsInView();
}

void DialogModelSet::loadData(int ref, const std::map<int, ParameterRule>& init_parameter_rules,
                              std::vector<Range> z_ranges, std::set<double> z_values, std::vector<Range> ebv_ranges,
                              std::set<double> ebv_values) {
  m_rules      = init_parameter_rules;
  m_ref        = ref;
  m_z_ranges   = z_ranges;
  m_z_values   = z_values;
  m_ebv_ranges = ebv_ranges;
  m_ebv_values = ebv_values;

  auto selected_rule = m_rules[m_ref];

  ui->txt_name->setText(QString::fromStdString(selected_rule.getName()));

  // SED
  static_cast<SedTreeModel*>(ui->treeView_Sed->model())->setState(selected_rule.getSedSelection());
  // Reddening Curve
  static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setState(selected_rule.getRedCurveSelection());

  // find the current borders
  std::set<double> old_z_val{};
  if (selected_rule.getZRanges().size() > 0) {
    for (auto& range : selected_rule.getZRanges()) {
      for (double val = range.getMin(); val < range.getMax() + range.getStep(); val += range.getStep()) {
        old_z_val.insert(val);
      }
    }
  } else {
    old_z_val = selected_rule.getRedshiftValues();
  }
  double old_z_min = 0;
  double old_z_max = 10000;
  if (old_z_val.size() > 0) {
    old_z_min = *(old_z_val.begin());
    old_z_max = *(old_z_val.rbegin());
  }

  // get the global values
  std::set<double> z_val{};
  if (m_z_ranges.size() > 0) {
    for (auto& range : m_z_ranges) {
      for (int index = 0; index <= std::round((range.getMax() - range.getMin()) / range.getStep()); index++) {
        double val = range.getMin() + index * range.getStep();
        z_val.insert(val);
      }
    }
  } else {
    z_val = m_z_values;
  }

  // fill the CB and find the selected values
  ui->min_z_cb->clear();
  ui->max_z_cb->clear();
  size_t index_z     = 0;
  size_t index_z_min = -1;
  size_t index_z_max = -1;
  for (auto& data : z_val) {
    ui->min_z_cb->addItem(QString::number(data));
    ui->max_z_cb->addItem(QString::number(data));

    if (data <= old_z_min) {
      index_z_min = index_z;
    }

    if (data <= old_z_max) {
      index_z_max = index_z;
    }

    ++index_z;
  }
  ui->max_z_cb->setCurrentIndex(index_z_max);
  ui->min_z_cb->setCurrentIndex(index_z_min);

  // find the current borders
  std::set<double> old_ebv_val{};
  if (selected_rule.getEbvRanges().size() > 0) {
    for (auto& range : selected_rule.getEbvRanges()) {
      for (double val = range.getMin(); val < range.getMax() + range.getStep(); val += range.getStep()) {
        old_ebv_val.insert(val);
      }
    }
  } else {
    old_ebv_val = selected_rule.getEbvValues();
  }
  double old_ebv_min = 0;
  double old_ebv_max = 10000;
  if (old_ebv_val.size() > 0) {
    old_ebv_min = *(old_ebv_val.begin());
    old_ebv_max = *(old_ebv_val.rbegin());
  }

  std::set<double> ebv_val{};
  if (m_ebv_ranges.size() > 0) {
    for (auto& range : m_ebv_ranges) {
      for (int index = 0; index <= std::round((range.getMax() - range.getMin()) / range.getStep()); index++) {
        double val = range.getMin() + index * range.getStep();
        ebv_val.insert(val);
      }
    }
  } else {
    ebv_val = m_ebv_values;
  }

  ui->min_ebv_cb->clear();
  ui->max_ebv_cb->clear();
  size_t index_ebv     = 0;
  size_t index_ebv_min = -1;
  size_t index_ebv_max = -1;
  for (auto& data : ebv_val) {
    ui->min_ebv_cb->addItem(QString::number(data));
    ui->max_ebv_cb->addItem(QString::number(data));

    if (data <= old_ebv_min) {
      index_ebv_min = index_ebv;
    }

    if (data <= old_ebv_max) {
      index_ebv_max = index_ebv;
    }

    ++index_ebv;
  }

  ui->max_ebv_cb->setCurrentIndex(index_ebv_max);
  ui->min_ebv_cb->setCurrentIndex(index_ebv_min);

  turnControlsInEdition();
}

void DialogModelSet::turnControlsInEdition() {
  ui->buttonBox->setEnabled(true);

  static_cast<SedTreeModel*>(ui->treeView_Sed->model())->setEnabled(true);
  static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setEnabled(true);

  ui->txt_name->setEnabled(true);

  // Redshift & E(B-V)
  ui->min_z_cb->setEnabled(true);
  ui->max_z_cb->setEnabled(true);
  ui->min_ebv_cb->setEnabled(true);
  ui->max_ebv_cb->setEnabled(true);
}

void DialogModelSet::turnControlsInView() {

  ui->buttonBox->setEnabled(true);
  ui->buttonBox->button(QDialogButtonBox::StandardButton::Save)->setEnabled(false);

  static_cast<SedTreeModel*>(ui->treeView_Sed->model())->setEnabled(false);
  static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setEnabled(false);

  ui->txt_name->setEnabled(false);

  // Redshift & E(B-V)
  ui->min_z_cb->setEnabled(false);
  ui->max_z_cb->setEnabled(false);
  ui->min_ebv_cb->setEnabled(false);
  ui->max_ebv_cb->setEnabled(false);
}

void DialogModelSet::on_buttonBox_rejected() {
  this->popupClosing(m_ref, m_rules[m_ref], false);
}

void DialogModelSet::on_buttonBox_accepted() {

  auto sed_selection       = static_cast<SedTreeModel*>(ui->treeView_Sed->model())->getState();
  auto red_curve_selection = static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->getState();

  if (ui->txt_name->text().trimmed().size() == 0) {
    QMessageBox::warning(this, "Missing Data...", "Please provide a name.", QMessageBox::Ok);
    return;
  }
  for (auto it = m_rules.begin(); it != m_rules.end(); ++it) {
    if (it->second.getName() == ui->txt_name->text().toStdString() && it->first != m_ref) {
      QMessageBox::warning(this, "Duplicate Name...",
                           "The name you enter is already used, please provide another name.", QMessageBox::Ok);
      return;
    }
  }

  if (sed_selection.isEmpty()) {
    QMessageBox::warning(this, "Missing Data...", "Please provide SED(s) selection.", QMessageBox::Ok);
    return;
  }

  if (red_curve_selection.isEmpty()) {
    QMessageBox::warning(this, "Missing Data...", "Please provide Reddening Curve(s) selection.", QMessageBox::Ok);
    return;
  }

  // Redshift and E(B-V)

  double             min_z = ui->min_z_cb->currentText().toDouble();
  double             max_z = ui->max_z_cb->currentText().toDouble();
  std::set<double>   new_z_values{};
  std::vector<Range> new_z_ranges{};
  if (m_z_ranges.size() > 0) {
    for (auto& range : m_z_ranges) {
      double start = std::max(min_z, range.getMin());
      double stop  = std::min(max_z, range.getMax());
      if (start <= range.getMax() && stop >= range.getMin()) {
        new_z_ranges.emplace_back(start, stop, range.getStep());
      }
    }
  } else {
    for (auto& value : m_z_values) {
      if (value >= min_z && value <= max_z) {
        new_z_values.insert(value);
      }
    }
  }

  auto old_z_ranges = m_rules[m_ref].getZRanges();

  try {
    m_rules[m_ref].setRedshiftValues(std::move(new_z_values));
    m_rules[m_ref].setZRanges(std::move(new_z_ranges));
    m_rules[m_ref].getModelNumber(m_seds_repository, m_redenig_curves_repository, true);
    m_rules[m_ref].getRedshiftRangeString();
  } catch (const Elements::Exception& e) {
    QMessageBox::warning(this, "Error while setting redshift ranges...", e.what(), QMessageBox::Ok);

    m_rules[m_ref].setZRanges(std::move(old_z_ranges));
    return;
  }

  double             min_ebv = ui->min_ebv_cb->currentText().toDouble();
  double             max_ebv = ui->max_ebv_cb->currentText().toDouble();
  std::set<double>   new_ebv_values{};
  std::vector<Range> new_ebv_ranges{};
  if (m_ebv_ranges.size() > 0) {
    for (auto& range : m_ebv_ranges) {
      double start = std::max(min_ebv, range.getMin());
      double stop  = std::min(max_ebv, range.getMax());
      if (start <= range.getMax() && stop >= range.getMin()) {
        new_ebv_ranges.emplace_back(start, stop, range.getStep());
      }
    }
  } else {
    for (auto& value : m_ebv_values) {
      if (value >= min_ebv && value <= max_ebv) {
        new_ebv_values.insert(value);
      }
    }
  }

  auto old_ebv_ranges = m_rules[m_ref].getEbvRanges();

  try {
    m_rules[m_ref].setEbvValues(std::move(new_ebv_values));
    m_rules[m_ref].setEbvRanges(std::move(new_ebv_ranges));
    m_rules[m_ref].getModelNumber(m_seds_repository, m_redenig_curves_repository, true);
    m_rules[m_ref].getEbvRangeString();
  } catch (const Elements::Exception& e) {
    QMessageBox::warning(this, "Error while setting E(B-V) ranges...", e.what(), QMessageBox::Ok);

    m_rules[m_ref].setEbvRanges(std::move(old_ebv_ranges));
    return;
  }

  m_rules[m_ref].setName(ui->txt_name->text().toStdString());

  // SED
  m_rules[m_ref].setSedSelection(std::move(sed_selection));

  // Reddeing Curves
  m_rules[m_ref].setRedCurveSelection(std::move(red_curve_selection));

  m_rules[m_ref].getModelNumber(m_seds_repository, m_redenig_curves_repository, true);

  this->popupClosing(m_ref, m_rules[m_ref], true);
  this->close();
}

}  // namespace PhzQtUI
}  // namespace Euclid
