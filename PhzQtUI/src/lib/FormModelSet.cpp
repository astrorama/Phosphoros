#include "PhzQtUI/FormModelSet.h"
#include <QMessageBox>

#include "FileUtils.h"
#include "PhzQtUI/DialogModelSet.h"
#include "PhzQtUI/DialogRange.h"
#include "ui_FormModelSet.h"

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "PreferencesUtils.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("FormModelSet");

FormModelSet::FormModelSet(QWidget* parent) : QWidget(parent), ui(new Ui::FormModelSet) {
  ui->setupUi(this);
  m_setInsert = false;
}

FormModelSet::~FormModelSet() {}

void FormModelSet::updateSelection() {

  if (m_model_set_model_ptr->getSelectedRow() >= 0) {
    logger.info() << "Update page for parameter space " << m_model_set_model_ptr->getSelectedModelSet().getName();
    // valid selection
    const ModelSet& selected_model = m_model_set_model_ptr->getSelectedModelSet();
    ui->txt_SetName->setText(QString::fromStdString(selected_model.getName()));

    if (ui->tableView_ParameterRule->selectionModel() != NULL) {
      disconnect(ui->tableView_ParameterRule->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), 0,
                 0);
    }
    ui->tableView_ParameterRule->loadParameterRules(selected_model.getParameterRules(), m_seds_repository,
                                                    m_redenig_curves_repository);

    connect(ui->tableView_ParameterRule->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            SLOT(rulesSelectionChanged(QModelIndex, QModelIndex)));

    if (m_model_set_model_ptr->getSelectedModelSet().getParameterRules().size() > 0) {
      ui->tableView_ParameterRule->selectRow(0);
    }

  } else {
    // Not a valid selection

    ui->txt_SetName->clear();
    if (ui->tableView_ParameterRule->selectionModel() != NULL) {
      disconnect(ui->tableView_ParameterRule->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), 0,
                 0);
    }
    ui->tableView_ParameterRule->loadParameterRules(std::map<int, ParameterRule>{}, m_seds_repository,
                                                    m_redenig_curves_repository);

    connect(ui->tableView_ParameterRule->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            SLOT(rulesSelectionChanged(QModelIndex, QModelIndex)));
  }
}

void FormModelSet::redshiftRangePopupClosing(std::vector<Range> ranges, std::set<double> values) {
  m_model_set_model_ptr->setGlobalRedshiftRangeToSelected(ranges, values);
  setModelInEdition();
}

void FormModelSet::ebvRangePopupClosing(std::vector<Range> ranges, std::set<double> values) {
  m_model_set_model_ptr->setGlobalEbvRangeToSelected(ranges, values);
  setModelInEdition();
}

void FormModelSet::on_btn_conf_z_clicked() {
  std::unique_ptr<DialogRange> popUp(new DialogRange());

  popUp->setData(m_model_set_model_ptr->getSelectedModelSet().getZRanges(),
                 m_model_set_model_ptr->getSelectedModelSet().getZValues(), true);

  connect(popUp.get(), SIGNAL(popupClosing(std::vector<Range>, std::set<double>)),
          SLOT(redshiftRangePopupClosing(std::vector<Range>, std::set<double>)));

  popUp->exec();
}

void FormModelSet::on_btn_conf_ebv_clicked() {
  std::unique_ptr<DialogRange> popUp(new DialogRange());

  popUp->setData(m_model_set_model_ptr->getSelectedModelSet().getEbvRanges(),
                 m_model_set_model_ptr->getSelectedModelSet().getEbvValues(), false);

  connect(popUp.get(), SIGNAL(popupClosing(std::vector<Range>, std::set<double>)),
          SLOT(ebvRangePopupClosing(std::vector<Range>, std::set<double>)));

  popUp->exec();
}

void FormModelSet::loadSetPage(std::shared_ptr<ModelSetModel> model_set_model_ptr, DatasetRepo seds_repository,
                               DatasetRepo redenig_curves_repository) {

  logger.info() << "Load the Parameter Space Page";
  m_model_set_model_ptr       = model_set_model_ptr;
  m_seds_repository           = seds_repository;
  m_redenig_curves_repository = redenig_curves_repository;

  ui->cb_selSpace->clear();
  auto current_selection = m_model_set_model_ptr->getSelectedModelSet().getName();
  int  index             = 0;
  m_diconnect_cb = true;
  for (auto& model_name : m_model_set_model_ptr->getModelSetList()) {
    ui->cb_selSpace->addItem(model_name);
    if (current_selection == model_name.toStdString()) {
      ui->cb_selSpace->setCurrentIndex(index);
    }
    ++index;
  }
  m_diconnect_cb = false;
  updateSelection();
  setModelInView();
}

void FormModelSet::on_cb_selSpace_currentIndexChanged(const QString&) {
  if (!m_diconnect_cb) {
    m_model_set_model_ptr->selectModelSet(ui->cb_selSpace->currentText());
    updateSelection();
  }
}

void FormModelSet::setModelInEdition() {
  ui->cb_selSpace->setEnabled(false);
  ui->frm_nav->setEnabled(false);
  ui->btn_SetNew->setEnabled(false);
  ui->btn_SetDuplicate->setEnabled(false);
  ui->btn_SetDelete->setEnabled(false);
  ui->btn_SetCancel->setEnabled(true);
  ui->btn_SetSave->setEnabled(true);
  ui->txt_SetName->setEnabled(m_model_set_model_ptr->getSelectedRow() >= 0);
  ui->btn_new_region->setEnabled(m_model_set_model_ptr->getSelectedRow() >= 0);
  ui->btn_open_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
  ui->btn_duplicate_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
  ui->btn_delete_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
}

void FormModelSet::setModelInView() {
  m_setInsert = false;

  ui->cb_selSpace->setEnabled(true);
  ui->frm_nav->setEnabled(true);
  ui->btn_SetNew->setEnabled(true);
  ui->btn_open_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
  ui->btn_SetDuplicate->setEnabled(m_model_set_model_ptr->getSelectedRow() >= 0);
  ui->btn_SetDelete->setEnabled(m_model_set_model_ptr->getSelectedRow() >= 0);
  ui->btn_SetCancel->setEnabled(false);
  ui->btn_SetSave->setEnabled(false);
  ui->txt_SetName->setEnabled(m_model_set_model_ptr->getSelectedRow() >= 0);
  ui->btn_new_region->setEnabled(m_model_set_model_ptr->getSelectedRow() >= 0);
  ui->btn_open_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
  ui->btn_duplicate_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
  ui->btn_delete_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
}

//  - Slots on this page
void FormModelSet::on_btn_ToAnalysis_clicked() {
  navigateToComputeRedshift(false);
}
void FormModelSet::on_btn_ToOption_clicked() {
  navigateToConfig();
}
void FormModelSet::on_btn_ToCatalog_clicked() {
  navigateToCatalog(false);
}

void FormModelSet::on_btn_ToPP_clicked() {
  navigateToPostProcessing(false);
}

void FormModelSet::on_btn_exit_clicked() {
  quit(true);
}

void FormModelSet::reload_cb() {
  auto current_selection = m_model_set_model_ptr->getSelectedModelSet().getName();
  m_diconnect_cb         = true;
  ui->cb_selSpace->clear();
  m_diconnect_cb = false;
  int index      = 0;
  for (auto& model_name : m_model_set_model_ptr->getModelSetList()) {
    ui->cb_selSpace->addItem(model_name);
    if (current_selection == model_name.toStdString()) {
      ui->cb_selSpace->setCurrentIndex(index);
    }
    ++index;
  }
  updateSelection();
}

void FormModelSet::on_btn_SetNew_clicked() {
  m_model_set_model_ptr->newModelSet(false);
  reload_cb();

  m_setInsert = true;
  setModelInEdition();
}

void FormModelSet::on_btn_SetDuplicate_clicked() {
  m_model_set_model_ptr->newModelSet(true);
  reload_cb();
  m_setInsert = true;
  setModelInEdition();
}

void FormModelSet::on_btn_SetDelete_clicked() {
  if (QMessageBox::question(this, "Confirm deletion...",
                            "Do you really want to delete the Parameter Space '" +
                                QString::fromStdString(m_model_set_model_ptr->getSelectedModelSet().getName()) + "' ?",
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    m_model_set_model_ptr->deleteSelected();
    reload_cb();
    setModelInView();
  }
}

void FormModelSet::on_btn_SetCancel_clicked() {
  if (m_setInsert) {
    m_model_set_model_ptr->deleteSelected();
    m_setInsert = false;

  } else {
    m_model_set_model_ptr->cancelSelected();
  }

  reload_cb();
  setModelInView();
}

void FormModelSet::on_txt_SetName_textEdited(const QString& text) {
  m_model_set_model_ptr->setNameToSelected(text);

  setModelInEdition();
}

void FormModelSet::on_btn_SetSave_clicked() {

  if (!m_model_set_model_ptr->checkUniqueName(
          QString::fromStdString(m_model_set_model_ptr->getSelectedModelSet().getName()),
          m_model_set_model_ptr->getSelectedRow())) {
    QMessageBox::warning(this, "Duplicate name...", "Parameter Space Name already exists, please enter a new name.",
                         QMessageBox::Ok);
    return;
  }

  try {
    m_model_set_model_ptr->getSelectedModelSet().getAxesTuple(m_seds_repository, m_redenig_curves_repository);
  } catch (Elements::Exception& except) {
    QMessageBox::warning(this, "Overlapping Regions...", except.what(), QMessageBox::Ok);
    return;
  }

  m_model_set_model_ptr->saveSelected();
  reload_cb();
  setModelInView();
}

void FormModelSet::parameterGridDoubleClicked(QModelIndex) {
  on_btn_open_region_clicked();
}

void FormModelSet::rulesSelectionChanged(QModelIndex, QModelIndex) {
  if (!ui->btn_SetSave->isEnabled()) {
    setModelInView();
  } else {
    setModelInEdition();
  }
}

bool FormModelSet::checkEbvZ() {
  if (m_model_set_model_ptr->getSelectedModelSet().getEbvRanges().size() == 0 &&
      m_model_set_model_ptr->getSelectedModelSet().getEbvValues().size() == 0) {
    QMessageBox::warning(this, "Missing E(B-V) range...",
                         "You must configure the E(B-V) range before setting up a parametre sub-space.",
                         QMessageBox::Ok);
    return false;
  } else if (m_model_set_model_ptr->getSelectedModelSet().getZRanges().size() == 0 &&
             m_model_set_model_ptr->getSelectedModelSet().getZValues().size() == 0) {
    QMessageBox::warning(this, "Missing redshift range...",
                         "You must configure the redshift range before setting up a parametre sub-space.",
                         QMessageBox::Ok);
    return false;
  } else {
    return true;
  }
}

void FormModelSet::on_btn_open_region_clicked() {
  if (!checkEbvZ()) {
    return;
  }
  std::unique_ptr<DialogModelSet> popUp(new DialogModelSet(m_seds_repository, m_redenig_curves_repository));
  int                             refid = ui->tableView_ParameterRule->getSelectedRuleId();
  popUp->loadData(refid, ui->tableView_ParameterRule->getModel()->getParameterRules(),
                  m_model_set_model_ptr->getSelectedModelSet().getZRanges(),
                  m_model_set_model_ptr->getSelectedModelSet().getZValues(),
                  m_model_set_model_ptr->getSelectedModelSet().getEbvRanges(),
                  m_model_set_model_ptr->getSelectedModelSet().getEbvValues());

  connect(popUp.get(), SIGNAL(popupClosing(int, ParameterRule, bool)),
          SLOT(setEditionPopupClosing(int, ParameterRule, bool)));

  popUp->exec();
}

void FormModelSet::on_btn_new_region_clicked() {
  if (!checkEbvZ()) {
    return;
  }
  std::unique_ptr<DialogModelSet> popUp(new DialogModelSet(m_seds_repository, m_redenig_curves_repository));
  auto                            current_list = ui->tableView_ParameterRule->getModel()->getParameterRules();
  current_list.insert(std::make_pair(-1, ParameterRule{}));
  popUp->loadData(-1, current_list, m_model_set_model_ptr->getSelectedModelSet().getZRanges(),
                  m_model_set_model_ptr->getSelectedModelSet().getZValues(),
                  m_model_set_model_ptr->getSelectedModelSet().getEbvRanges(),
                  m_model_set_model_ptr->getSelectedModelSet().getEbvValues());

  connect(popUp.get(), SIGNAL(popupClosing(int, ParameterRule, bool)),
          SLOT(setEditionPopupClosing(int, ParameterRule, bool)));

  popUp->exec();
}

void FormModelSet::on_btn_duplicate_region_clicked() {
  if (!checkEbvZ()) {
    return;
  }
  std::unique_ptr<DialogModelSet> popUp(new DialogModelSet(m_seds_repository, m_redenig_curves_repository));
  auto                            current_list = ui->tableView_ParameterRule->getModel()->getParameterRules();
  auto                            rule         = ui->tableView_ParameterRule->getSelectedRule();
  rule.setName(rule.getName() + "_Copy");
  current_list.insert(std::make_pair(-1, rule));
  popUp->loadData(-1, current_list, m_model_set_model_ptr->getSelectedModelSet().getZRanges(),
                  m_model_set_model_ptr->getSelectedModelSet().getZValues(),
                  m_model_set_model_ptr->getSelectedModelSet().getEbvRanges(),
                  m_model_set_model_ptr->getSelectedModelSet().getEbvValues());

  connect(popUp.get(), SIGNAL(popupClosing(int, ParameterRule, bool)),
          SLOT(setEditionPopupClosing(int, ParameterRule, bool)));

  popUp->exec();
}

void FormModelSet::on_btn_delete_region_clicked() {

  auto rule_name = QString::fromStdString(ui->tableView_ParameterRule->getSelectedRule().getName());
  if (QMessageBox::question(this, "Confirm deletion...",
                            "Do you really want to delete the Sub-space '" + rule_name + "' ?",
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    ui->tableView_ParameterRule->deletSelectedRule();
    m_model_set_model_ptr->setParameterRulesToSelected(ui->tableView_ParameterRule->getModel()->getParameterRules());
    setModelInEdition();
  }
}

void FormModelSet::setEditionPopupClosing(int ref, ParameterRule rule, bool validated) {
  if (validated) {
    auto current_rules = ui->tableView_ParameterRule->getModel()->getParameterRules();
    if (ref < 0) {
      // insert the new rule
      int max_ref = -1;
      for (auto& rules : current_rules) {
        if (rules.first > max_ref) {
          max_ref = rules.first;
        }
      }

      ++max_ref;
      current_rules.insert(std::make_pair(max_ref, rule));
    } else {
      // update current rule
      current_rules[ref] = rule;
    }
    int current_row = 0;
    if (ui->tableView_ParameterRule->getModel()->getParameterRules().size() > 0) {
      current_row = ui->tableView_ParameterRule->selectionModel()->selectedIndexes()[0].row();
    }
    ui->tableView_ParameterRule->loadParameterRules(current_rules, m_seds_repository, m_redenig_curves_repository);
    if (current_rules.size() > 0) {
      if (ref > 0) {
        ui->tableView_ParameterRule->selectRow(current_row);
      } else {
        ui->tableView_ParameterRule->selectRow(current_rules.size() - 1);
      }
    }

    m_model_set_model_ptr->setParameterRulesToSelected(ui->tableView_ParameterRule->getModel()->getParameterRules());

    setModelInEdition();
  }
}

}  // namespace PhzQtUI
}  // namespace Euclid
