#include <QMessageBox>
#include "PhzQtUI/FormModelSet.h"

#include "ui_FormModelSet.h"
#include "PhzQtUI/DialogModelSet.h"
#include "FileUtils.h"

#include "ElementsKernel/Exception.h"
#include "PreferencesUtils.h"
#include "ElementsKernel/Logging.h"


namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("FormModelSet");

FormModelSet::FormModelSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormModelSet) {
    ui->setupUi(this);
    m_setInsert = false;
}

FormModelSet::~FormModelSet() {}


void FormModelSet::updateSelection(){

  ui->tableView_Set->selectRow(m_model_set_model_ptr->getSelectedRow());
}


void FormModelSet::loadSetPage(
    std::shared_ptr<ModelSetModel> model_set_model_ptr,
    DatasetRepo seds_repository,
    DatasetRepo redenig_curves_repository) {

    logger.info()<< "Load the Parameter Space Page";
    m_model_set_model_ptr = model_set_model_ptr;
    m_seds_repository = seds_repository;
    m_redenig_curves_repository = redenig_curves_repository;

    disconnect(ui->tableView_Set->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), 0, 0);
    disconnect(ui->tableView_Set, SIGNAL(doubleClicked(QModelIndex)), 0, 0);

    ui->tableView_Set->load(m_model_set_model_ptr);

    connect(
      ui->tableView_Set->selectionModel(),
      SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
      SLOT(setSelectionChanged(QModelIndex, QModelIndex)));
    connect(
      ui->tableView_Set,
      SIGNAL(doubleClicked(QModelIndex)),
      SLOT(setGridDoubleClicked(QModelIndex)));

    ui->txt_SetName->clear();
    ui->tableView_Set->clearSelection();

    disconnect(ui->tableView_ParameterRule, SIGNAL(doubleClicked(QModelIndex)), 0, 0);
    ui->tableView_ParameterRule->loadParameterRules(std::map<int, ParameterRule>{}, m_seds_repository, m_redenig_curves_repository);
    connect(ui->tableView_ParameterRule,
                    SIGNAL(doubleClicked(QModelIndex)),
                    SLOT(parameterGridDoubleClicked(QModelIndex)));

    updateSelection();
    setModelInView();
}

void  FormModelSet::setModelInEdition() {
  ui->frm_nav->setEnabled(false);
    ui->btn_SetNew->setEnabled(false);
    ui->btn_SetDuplicate->setEnabled(false);
    ui->btn_SetDelete->setEnabled(false);
    ui->btn_SetEdit->setEnabled(false);
    ui->btn_SetCancel->setEnabled(true);
    ui->btn_SetSave->setEnabled(true);
    ui->btn_new_region->setEnabled(true);
    ui->btn_open_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
    ui->btn_duplicate_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
    ui->btn_delete_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
    ui->txt_SetName->setEnabled(true);
    ui->tableView_Set->setEnabled(false);
}

void  FormModelSet::setModelInView() {
    m_setInsert = false;
    ui->frm_nav->setEnabled(true);
    ui->btn_SetNew->setEnabled(true);
    ui->btn_open_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
    ui->btn_SetDuplicate->setEnabled(m_model_set_model_ptr->getSelectedRow() >= 0);
    ui->btn_SetDelete->setEnabled(m_model_set_model_ptr->getSelectedRow() >= 0);
    ui->btn_SetEdit->setEnabled(m_model_set_model_ptr->getSelectedRow() >= 0);
    ui->btn_SetCancel->setEnabled(false);
    ui->btn_SetSave->setEnabled(false);
    ui->btn_new_region->setEnabled(false);
    ui->btn_duplicate_region->setEnabled(false);
    ui->btn_delete_region->setEnabled(false);
    ui->txt_SetName->setEnabled(false);
    ui->tableView_Set->setEnabled(true);
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

void FormModelSet::on_btn_ToPP_clicked(){
  navigateToPostProcessing(false);
}

void FormModelSet::on_btn_exit_clicked() {
  quit(true);
}


void FormModelSet::on_btn_SetNew_clicked() {
  m_model_set_model_ptr->newModelSet(false);
  updateSelection();
  m_setInsert = true;
  setModelInEdition();
}

void FormModelSet::on_btn_SetDuplicate_clicked() {
  m_model_set_model_ptr->newModelSet(true);
  updateSelection();
  m_setInsert = true;
  setModelInEdition();
}

void FormModelSet::on_btn_SetDelete_clicked() {
  if (QMessageBox::question( this, "Confirm deletion...",
                                "Do you really want to delete the Parameter Space '" +
                                QString::fromStdString(m_model_set_model_ptr->getSelectedModelSet().getName()) +"' ?",
                                QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
    m_model_set_model_ptr->deleteSelected();
    selectFromGrid();
  }
}

void FormModelSet::setGridDoubleClicked(QModelIndex) {
  on_btn_SetEdit_clicked();
}

void FormModelSet::on_btn_SetEdit_clicked() {
    setModelInEdition();
}


void FormModelSet::selectFromGrid() {
  QItemSelectionModel *current_selection = ui->tableView_Set->selectionModel();
  if (current_selection->hasSelection()) {
    int row = current_selection->selectedRows().at(0).row();
    m_model_set_model_ptr->selectModelSet(row);
  } else {
    m_model_set_model_ptr->selectModelSet(-1);
  }
  updateSelection();
}


void FormModelSet::on_btn_SetCancel_clicked() {
    if (m_setInsert) {
      m_model_set_model_ptr->deleteSelected();
      m_setInsert = false;
    } else {
      m_model_set_model_ptr->cancelSelected();
      ui->txt_SetName->setText(QString::fromStdString(m_model_set_model_ptr->getSelectedModelSet().getName()));
      ui->tableView_ParameterRule->loadParameterRules(m_model_set_model_ptr->getSelectedModelSet().getParameterRules(),
                            m_seds_repository,
                            m_redenig_curves_repository);
     disconnect(ui->tableView_ParameterRule->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), 0, 0);
     connect(ui->tableView_ParameterRule->selectionModel(),
                                    SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                                    SLOT(rulesSelectionChanged(QModelIndex, QModelIndex)));
  }

  selectFromGrid();
  setModelInView();
}



void FormModelSet::on_txt_SetName_textEdited(const QString& text) {
  m_model_set_model_ptr->setNameToSelected(text);
}


void FormModelSet::on_btn_SetSave_clicked() {

   if (! m_model_set_model_ptr->checkUniqueName(
       QString::fromStdString(m_model_set_model_ptr->getSelectedModelSet().getName()),
       m_model_set_model_ptr->getSelectedRow())) {
     QMessageBox::warning(this,
        "Duplicate name...",
       "Parameter Space Name already exists, please enter a new name.",
       QMessageBox::Ok);
     return;
   }

   try {
     m_model_set_model_ptr->getSelectedModelSet().getAxesTuple();
     } catch (Elements::Exception& except) {
       QMessageBox::warning(this, "Overlapping Regions...",
                                    except.what(),
                                    QMessageBox::Ok);
       return;
   }

   m_model_set_model_ptr->saveSelected();
   setModelInView();
   updateSelection();
}


void FormModelSet::setSelectionChanged(QModelIndex new_index, QModelIndex) {
    if (new_index.isValid()) {
      m_model_set_model_ptr->selectModelSet(new_index.row());
    } else {
      m_model_set_model_ptr->selectModelSet(-1);
    }

    ui->txt_SetName->setText(QString::fromStdString(m_model_set_model_ptr->getSelectedModelSet().getName()));
    disconnect(ui->tableView_ParameterRule->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), 0, 0);
    ui->tableView_ParameterRule->loadParameterRules(
            m_model_set_model_ptr->getSelectedModelSet().getParameterRules(),
            m_seds_repository,
            m_redenig_curves_repository);

    connect(ui->tableView_ParameterRule->selectionModel(),
                               SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                               SLOT(rulesSelectionChanged(QModelIndex, QModelIndex)));

    if (m_model_set_model_ptr->getSelectedModelSet().getParameterRules().size() > 0) {
          ui->tableView_ParameterRule->selectRow(0);
    }

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


void FormModelSet::on_btn_open_region_clicked() {
  std::unique_ptr<DialogModelSet> popUp( new  DialogModelSet(m_seds_repository, m_redenig_curves_repository));
  int refid = ui->tableView_ParameterRule->getSelectedRuleId();
  popUp->loadData(refid, ui->tableView_ParameterRule->getModel()->getParameterRules());

  if (!ui->btn_SetSave->isEnabled()) {
      popUp->setViewMode();
  }

  connect(popUp.get(),
      SIGNAL(popupClosing(int, ParameterRule, bool)),
      SLOT(setEditionPopupClosing(int, ParameterRule, bool)));

  popUp->exec();
}


void FormModelSet::on_btn_new_region_clicked() {
  std::unique_ptr<DialogModelSet> popUp( new  DialogModelSet(m_seds_repository, m_redenig_curves_repository));
      auto current_list = ui->tableView_ParameterRule->getModel()->getParameterRules();
      current_list.insert(std::make_pair(-1, ParameterRule {}));
      popUp->loadData(-1, current_list);

      connect(popUp.get(),
        SIGNAL(popupClosing(int, ParameterRule, bool)),
        SLOT(setEditionPopupClosing(int, ParameterRule, bool)));

      popUp->exec();
}


void FormModelSet::on_btn_duplicate_region_clicked() {
  std::unique_ptr<DialogModelSet> popUp( new  DialogModelSet(m_seds_repository, m_redenig_curves_repository));
        auto current_list = ui->tableView_ParameterRule->getModel()->getParameterRules();
        auto rule = ui->tableView_ParameterRule->getSelectedRule();
        rule.setName(rule.getName()+"_Copy");
        current_list.insert(std::make_pair(-1, rule));
        popUp->loadData(-1, current_list);

        connect(popUp.get(),
          SIGNAL(popupClosing(int, ParameterRule, bool)),
          SLOT(setEditionPopupClosing(int, ParameterRule, bool)));

        popUp->exec();
}


void FormModelSet::on_btn_delete_region_clicked() {

  auto rule_name = QString::fromStdString(ui->tableView_ParameterRule->getSelectedRule().getName());
  if (QMessageBox::question( this, "Confirm deletion...",
                                   "Do you really want to delete the Sub-space '"+rule_name+"' ?",
                                   QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
         ui->tableView_ParameterRule->deletSelectedRule();
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
    ui->tableView_ParameterRule->loadParameterRules(current_rules, m_seds_repository,
                     m_redenig_curves_repository);
    if (current_rules.size() > 0) {
      if (ref > 0) {
             ui->tableView_ParameterRule->selectRow(current_row);
      } else {
        ui->tableView_ParameterRule->selectRow(current_rules.size()-1);
      }
    }

    if (!ui->btn_SetSave->isEnabled()) {
       setModelInView();
     } else {
       setModelInEdition();
       m_model_set_model_ptr->setParameterRulesToSelected(ui->tableView_ParameterRule->getModel()->getParameterRules());
     }
 }
}

}
}
