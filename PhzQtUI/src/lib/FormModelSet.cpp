#include <QMessageBox>
#include "PhzQtUI/FormModelSet.h"
#include "PhzQtUI/XYDataSetTreeModel.h"

#include "ui_FormModelSet.h"
#include "PhzQtUI/DialogModelSet.h"
#include "FileUtils.h"

#include "ElementsKernel/Exception.h"


namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {

FormModelSet::FormModelSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormModelSet)
{
    ui->setupUi(this);
    m_setInsert=false;
}

FormModelSet::~FormModelSet()
{
}


void FormModelSet::loadSetPage(DatasetRepo seds_repository,
    DatasetRepo redenig_curves_repository){

    m_seds_repository=seds_repository;
    m_redenig_curves_repository=redenig_curves_repository;

    ui->tableView_Set->loadFromPath(FileUtils::getModelRootPath(true));

    disconnect(ui->tableView_Set->selectionModel(),SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),0,0);
    connect(
      ui->tableView_Set->selectionModel(),
      SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
      SLOT(setSelectionChanged(QModelIndex, QModelIndex))
     );

    disconnect(ui->tableView_Set,SIGNAL(doubleClicked (QModelIndex)),0,0);
    connect(
                ui->tableView_Set,
                SIGNAL(doubleClicked (QModelIndex)),
                SLOT(setGridDoubleClicked(QModelIndex))
               );



    ui->txt_SetName->clear();
    ui->tableView_Set->clearSelection();
    ui->tableView_ParameterRule->loadParameterRules(std::map<int,ParameterRule>{},m_seds_repository,m_redenig_curves_repository);
    disconnect(ui->tableView_ParameterRule,SIGNAL(doubleClicked(QModelIndex)),0,0);
    connect(ui->tableView_ParameterRule,
                    SIGNAL(doubleClicked(QModelIndex)),
                    SLOT(parameterGridDoubleClicked(QModelIndex)));

    setModelInView();
}

void  FormModelSet::setModelInEdition(){
    ui->btn_SetToHome->setEnabled(false);
    ui->btn_backHome->setEnabled(false);
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

void  FormModelSet::setModelInView(){
    m_setInsert=false;
    ui->btn_SetToHome->setEnabled(true);
    ui->btn_backHome->setEnabled(true);
    ui->btn_SetNew->setEnabled(true);
    ui->btn_open_region->setEnabled(ui->tableView_ParameterRule->hasSelectedPArameterRule());
    ui->btn_SetDuplicate->setEnabled(ui->tableView_Set->hasSelectedSet());
    ui->btn_SetDelete->setEnabled(ui->tableView_Set->hasSelectedSet());
    ui->btn_SetEdit->setEnabled(ui->tableView_Set->hasSelectedSet());
    ui->btn_SetCancel->setEnabled(false);
    ui->btn_SetSave->setEnabled(false);
    ui->btn_new_region->setEnabled(false);
    ui->btn_duplicate_region->setEnabled(false);
    ui->btn_delete_region->setEnabled(false);
    ui->txt_SetName->setEnabled(false);
    ui->tableView_Set->setEnabled(true);
}



//  - Slots on this page
void FormModelSet::on_btn_SetToHome_clicked()
{
     navigateToHome();
}

void FormModelSet::on_btn_backHome_clicked()
{
     navigateToHome();
}


void FormModelSet::on_btn_SetNew_clicked()
{
     ui->tableView_Set->newSet(false);
     m_setInsert=true;
     setModelInEdition();
}

void FormModelSet::on_btn_SetDuplicate_clicked()
{
    ui->tableView_Set->newSet(true);
    m_setInsert=true;
    setModelInEdition();
}

void FormModelSet::on_btn_SetDelete_clicked()
{
    if (QMessageBox::question( this, "Confirm deletion...",
                                  "Do you really want to delete the Parameter Space '"+ui->tableView_Set->getSelectedName()+"' ?",
                                  QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes){
        ui->tableView_Set->deleteSelectedSet(true);
    }
}

void FormModelSet::setGridDoubleClicked(QModelIndex)
{
  on_btn_SetEdit_clicked();
}

void FormModelSet::on_btn_SetEdit_clicked()
{
    setModelInEdition();
}

void FormModelSet::on_btn_SetCancel_clicked()
{
    if (m_setInsert){
        ui->tableView_Set->deleteSelectedSet(false);
        m_setInsert=false;
    }else{
        ui->txt_SetName->setText(QString(ui->tableView_Set->getSelectedName()));

        ui->tableView_ParameterRule->loadParameterRules(ui->tableView_Set->getSelectedParameterRules(), m_seds_repository,
                            m_redenig_curves_repository);
        if (ui->tableView_Set->getSelectedParameterRules().size()>0 ){
          ui->tableView_ParameterRule->selectRow(0);
        }

        disconnect(ui->tableView_ParameterRule->selectionModel(),SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),0,0);
        connect(ui->tableView_ParameterRule->selectionModel(),
                                    SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                                    SLOT(rulesSelectionChanged(QModelIndex, QModelIndex)));
    }

    setModelInView();
}

void FormModelSet::on_btn_SetSave_clicked()
{
   std::string old_name =  ui->tableView_Set->getSelectedName().toStdString();
   if (! ui->tableView_Set->setSelectedName( ui->txt_SetName->text())){
     QMessageBox::warning( this,
        "Duplicate name...",
       "Parameter Space Name already exists, please enter a new name.",
       QMessageBox::Ok );
     return;
   }

   auto rules =  ui->tableView_Set->getSelectedParameterRules();
   ui->tableView_Set->setSelectedRules(ui->tableView_ParameterRule->getModel()->getParameterRules());

   try{
       ui->tableView_Set->getSelectedAxesTuple();
     } catch (Elements::Exception except){
       QMessageBox::warning( this,
                                    "Overlapping Regions...",
                                    except.what(),
                                    QMessageBox::Ok );

       ui->tableView_Set->setSelectedRules(rules);
       return;
   }

   ui->tableView_Set->updateModelNumberForSelected();
   ui->tableView_Set->saveSelectedSet(old_name);
   setModelInView();

}

void FormModelSet::setSelectionChanged(QModelIndex new_index, QModelIndex)
{
    if (new_index.isValid()){
        ModelSetModel* model=ui->tableView_Set->getModel();
        ui->txt_SetName->setText(model->getName(new_index.row()));

        ui->tableView_ParameterRule->loadParameterRules(
            model->getParameterRules(new_index.row()),
            m_seds_repository,
            m_redenig_curves_repository);
        disconnect(ui->tableView_ParameterRule->selectionModel(),SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),0,0);
        connect(ui->tableView_ParameterRule->selectionModel(),
                               SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                               SLOT(rulesSelectionChanged(QModelIndex, QModelIndex)));

        if (model->getParameterRules(new_index.row()).size()>0){
          ui->tableView_ParameterRule->selectRow(0);
        }
    }
    else{
        ui->txt_SetName->setText("");
        ui->tableView_ParameterRule->loadParameterRules(
            std::map<int,ParameterRule>{},
            m_seds_repository,
            m_redenig_curves_repository);
    }
    setModelInView();
}


void FormModelSet::parameterGridDoubleClicked(QModelIndex){
  on_btn_open_region_clicked();
}





void FormModelSet::rulesSelectionChanged(QModelIndex, QModelIndex) {
  if (!ui->btn_SetSave->isEnabled()) {
    setModelInView();
  } else {
    setModelInEdition();
  }
}


void FormModelSet::on_btn_open_region_clicked()
{
  std::unique_ptr<DialogModelSet> popUp( new  DialogModelSet(m_seds_repository, m_redenig_curves_repository));
     int refid = ui->tableView_ParameterRule->getSelectedRuleId();
    popUp->loadData(refid,ui->tableView_ParameterRule->getModel()->getParameterRules());

    if (!ui->btn_SetSave->isEnabled()){
      popUp->setViewMode();
    }

    connect(
      popUp.get(),
      SIGNAL(popupClosing(int,ParameterRule,bool)),
      SLOT(setEditionPopupClosing(int,ParameterRule,bool))
     );

    popUp->exec();
}


void FormModelSet::on_btn_new_region_clicked(){
  std::unique_ptr<DialogModelSet> popUp( new  DialogModelSet(m_seds_repository, m_redenig_curves_repository));
      auto current_list = ui->tableView_ParameterRule->getModel()->getParameterRules();
      current_list.insert(std::make_pair(-1,ParameterRule{}));
      popUp->loadData(-1,current_list);

      connect(
        popUp.get(),
        SIGNAL(popupClosing(int,ParameterRule,bool)),
        SLOT(setEditionPopupClosing(int,ParameterRule,bool))
       );

      popUp->exec();
}


void FormModelSet::on_btn_duplicate_region_clicked(){
  std::unique_ptr<DialogModelSet> popUp( new  DialogModelSet(m_seds_repository, m_redenig_curves_repository));
        auto current_list = ui->tableView_ParameterRule->getModel()->getParameterRules();
        auto rule = ui->tableView_ParameterRule->getSelectedRule();
        rule.setName(rule.getName()+"_Copy");
        current_list.insert(std::make_pair(-1,rule));
        popUp->loadData(-1,current_list);

        connect(
          popUp.get(),
          SIGNAL(popupClosing(int,ParameterRule,bool)),
          SLOT(setEditionPopupClosing(int,ParameterRule,bool))
         );

        popUp->exec();
}


void FormModelSet::on_btn_delete_region_clicked(){

  auto rule_name = QString::fromStdString(ui->tableView_ParameterRule->getSelectedRule().getName());
  if (QMessageBox::question( this, "Confirm deletion...",
                                   "Do you really want to delete the Sub-space '"+rule_name+"' ?",
                                   QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes){
         ui->tableView_ParameterRule->deletSelectedRule();
     }
}

void FormModelSet::setEditionPopupClosing(int ref ,ParameterRule rule ,bool validated){
  if (validated){
    auto current_rules = ui->tableView_ParameterRule->getModel()->getParameterRules();
    if (ref<0){
      // insert the new rule
      int max_ref=-1;
      for(auto& rules : current_rules) {
        if (rules.first>max_ref){
          max_ref=rules.first;
        }
      }

      ++max_ref;
      current_rules.insert(std::make_pair(max_ref,rule));
    } else {
      //update current rule
      current_rules[ref]=rule;

    }
    int current_row = 0;
    if (ui->tableView_ParameterRule->getModel()->getParameterRules().size()>0 ){
      current_row = ui->tableView_ParameterRule->selectionModel()->selectedIndexes()[0].row();
    }
    ui->tableView_ParameterRule->loadParameterRules(current_rules, m_seds_repository,
                     m_redenig_curves_repository);
    if (current_rules.size()>0 ){
      if (ref>0) {
             ui->tableView_ParameterRule->selectRow(current_row);
      } else {
        ui->tableView_ParameterRule->selectRow(current_rules.size()-1);
      }
    }

    if (!ui->btn_SetSave->isEnabled()) {
       setModelInView();
     } else {
       setModelInEdition();
     }
 }
}

}
}
