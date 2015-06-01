#include <QMessageBox>
#include "PhzQtUI/FormModelSet.h"
#include "ui_FormModelSet.h"
#include "PhzQtUI/DialogModelSet.h"
#include "FileUtils.h"

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


void FormModelSet::loadSetPage(){

    ui->tableView_Set->loadFromPath(FileUtils::getModelRootPath(true));

    connect(
      ui->tableView_Set->selectionModel(),
      SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
      SLOT(setSelectionChanged(QModelIndex, QModelIndex))
     );
    ui->txt_SetName->clear();
    ui->tableView_Set->clearSelection();
    ui->tableView_ParameterRule->loadParameterRules(std::map<int,ParameterRule>{},"","");
    setModelInView();
}

void  FormModelSet::setModelInEdition(){
    ui->btn_SetToHome->setEnabled(false);
    ui->btn_backHome->setEnabled(false);
    ui->btn_SetNew->setEnabled(false);
    ui->btn_SetDuplicate->setEnabled(false);
    ui->btn_SetDelete->setEnabled(false);
    ui->btn_viewSet->setEnabled(false);
    ui->btn_SetEdit->setEnabled(false);
    ui->btn_SetCancel->setEnabled(true);
    ui->btn_SetSave->setEnabled(true);
    ui->btn_SetToRules->setEnabled(true);
    ui->txt_SetName->setEnabled(true);
    ui->tableView_Set->setEnabled(false);
}

void  FormModelSet::setModelInView(){
    m_setInsert=false;
    ui->btn_SetToHome->setEnabled(true);
    ui->btn_backHome->setEnabled(true);
    ui->btn_SetNew->setEnabled(true);
    ui->btn_viewSet->setEnabled(ui->tableView_Set->hasSelectedSet());
    ui->btn_SetDuplicate->setEnabled(ui->tableView_Set->hasSelectedSet());
    ui->btn_SetDelete->setEnabled(ui->tableView_Set->hasSelectedSet());
    ui->btn_SetEdit->setEnabled(ui->tableView_Set->hasSelectedSet());
    ui->btn_SetCancel->setEnabled(false);
    ui->btn_SetSave->setEnabled(false);
    ui->btn_SetToRules->setEnabled(false);
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
                                  "Do you really want to delete the Model Set '"+ui->tableView_Set->getSelectedName()+"' ?",
                                  QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes){
        ui->tableView_Set->deleteSelectedSet(true);
    }
}

void FormModelSet::on_btn_SetEdit_clicked()
{
    setModelInEdition();
    on_btn_SetToRules_clicked();
}

void FormModelSet::on_btn_SetCancel_clicked()
{
    if (m_setInsert){
        ui->tableView_Set->deleteSelectedSet(false);
        m_setInsert=false;
    }else{
        ui->txt_SetName->setText(QString(ui->tableView_Set->getSelectedName()));
        ui->tableView_ParameterRule->loadParameterRules(ui->tableView_Set->getSelectedParameterRules(),FileUtils::getSedRootPath(false),FileUtils::getRedCurveRootPath(false));
    }

    setModelInView();
}

void FormModelSet::on_btn_SetSave_clicked()
{
    std::string old_name =  ui->tableView_Set->getSelectedName().toStdString();
   if ( ui->tableView_Set->setSelectedName( ui->txt_SetName->text())){
      ui->tableView_Set->setSelectedRules(ui->tableView_ParameterRule->getModel()->getParameterRules());
      ui->tableView_Set->updateModelNumberForSelected();

      ui->tableView_Set->saveSelectedSet(old_name);
      setModelInView();
   }
   else{
       QMessageBox::warning( this,
                             "Duplicate name...",
                             "The name you keyed in is already used. Please enter a new name.",
                             QMessageBox::Ok );
   }
}

void FormModelSet::setSelectionChanged(QModelIndex new_index, QModelIndex)
{
    if (new_index.isValid()){
        ModelSetModel* model=ui->tableView_Set->getModel();
        ui->txt_SetName->setText(model->getName(new_index.row()));

        ui->tableView_ParameterRule->loadParameterRules(model->getParameterRules(new_index.row()),FileUtils::getSedRootPath(false),FileUtils::getRedCurveRootPath(false));
    }
    else{
        ui->txt_SetName->setText("");
        ui->tableView_ParameterRule->loadParameterRules(std::map<int,ParameterRule>{},"","");
    }
    setModelInView();
}

void FormModelSet::on_btn_SetToRules_clicked()
{
  std::unique_ptr<DialogModelSet> popUp( new  DialogModelSet());
    popUp->loadData(ui->tableView_ParameterRule->getModel()->getParameterRules());

    // As PHOSPHOROS do not know how to treat sparse grid for now, block to 1 rule!
    popUp->setSingleLine();

    connect(
      popUp.get(),
      SIGNAL(popupClosing(std::map<int,ParameterRule>)),
      SLOT(setEditionPopupClosing(std::map<int,ParameterRule>))
     );

    popUp->exec();
}

void FormModelSet::on_btn_viewSet_clicked()
{
  std::unique_ptr<DialogModelSet> popUp( new  DialogModelSet());
    popUp->loadData(ui->tableView_ParameterRule->getModel()->getParameterRules());
    popUp->setViewMode();

    connect(
      popUp.get(),
      SIGNAL(popupClosing(std::map<int,ParameterRule>)),
      SLOT(setEditionPopupClosing(std::map<int,ParameterRule>))
     );

    popUp->exec();
}

void FormModelSet::setEditionPopupClosing(std::map<int,ParameterRule> rules){

     ui->tableView_ParameterRule->loadParameterRules(rules,FileUtils::getSedRootPath(false),FileUtils::getRedCurveRootPath(false));
 }

}
}
