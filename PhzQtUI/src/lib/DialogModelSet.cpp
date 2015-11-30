
#include <QMessageBox>
#include "FileUtils.h"
#include "PhzQtUI/DialogModelSet.h"
#include "ui_DialogModelSet.h"
#include "PhzQtUI/XYDataSetTreeModel.h"

using namespace std;

namespace Euclid{
namespace PhzQtUI {

DialogModelSet::DialogModelSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogModelSet)
{
    ui->setupUi(this);

    ui->txt_ebvMin->setValidator( new QDoubleValidator(0, 100, 2, this) );
    ui->txt_ebvMax->setValidator( new QDoubleValidator(0, 100, 2, this) );
    ui->txt_ebvStep->setValidator( new QDoubleValidator(0, 100, 4, this) );
    // z-range
    ui->txt_zMin->setValidator( new QDoubleValidator(0, 100, 2, this) );
    ui->txt_zMax->setValidator( new QDoubleValidator(0, 100, 2, this) );
    ui->txt_zStep->setValidator( new QDoubleValidator(0, 100, 4, this) );

    XYDataSetTreeModel* treeModel_sed = new XYDataSetTreeModel();
    treeModel_sed->loadDirectory(FileUtils::getSedRootPath(true),false,"SEDs");
    ui->treeView_Sed->setModel(treeModel_sed);
    ui->treeView_Sed->expandAll();

    connect(treeModel_sed, SIGNAL(itemChanged(QStandardItem*)), treeModel_sed,
                 SLOT(onItemChanged(QStandardItem*)));

    if (!treeModel_sed->item(0,0)->hasChildren()){
         QMessageBox::warning(this, "No available SED...",
                 "There is no SED to select. "
                 "You can provide and manage SEDs in the \"Configuration/Aux. Data\" page.",
                 QMessageBox::Ok);
    }

    XYDataSetTreeModel* treeModel_red = new XYDataSetTreeModel();
    treeModel_red->loadDirectory(FileUtils::getRedCurveRootPath(true),false,"Reddening Curves");
    ui->treeView_Reddening->setModel(treeModel_red);
    ui->treeView_Reddening->expandAll();

    connect( treeModel_red, SIGNAL(itemChanged(QStandardItem*)), treeModel_red,
                 SLOT(onItemChanged(QStandardItem*)));

    if (!treeModel_red->item(0,0)->hasChildren()){
         QMessageBox::warning(this, "No available Reddening Curve...",
                 "There is no reddening curve to select. "
                 "You can provide and manage reddening curves in the \"Configuration/Aux. Data\" page.",
                 QMessageBox::Ok);
    }

}

DialogModelSet::~DialogModelSet()
{
}

 void DialogModelSet::setViewMode(){
     m_view_popup=true;
     turnControlsInView();
 }

 void DialogModelSet::setSingleLine(){
     m_singe_line=true;
     turnControlsInView();
 }


void DialogModelSet::loadData(const map<int,ParameterRule>& init_parameter_rules){
    ui->tableView_ParameterRule->loadParameterRules(init_parameter_rules, FileUtils::getSedRootPath(false), FileUtils::getRedCurveRootPath(false));


    connect(
      ui->tableView_ParameterRule->selectionModel(),
      SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
      SLOT(selectionChanged(QModelIndex, QModelIndex))
     );

    if (ui->tableView_ParameterRule->model()->rowCount()>0){
      ui->tableView_ParameterRule->selectRow(0);
    }


    turnControlsInView();
}



void DialogModelSet::turnControlsInEdition(){
    ui->buttonBox->setEnabled(false);


    ui->btn_new->setEnabled(false);
    ui->btn_duplicate->setEnabled(false);
    ui->btn_delete->setEnabled(false);

    ui->btn_edit->setEnabled(false);
    ui->btn_cancel->setEnabled(true);
    ui->btn_save->setEnabled(true);

    static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->setEnabled(true);
    static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->setEnabled(true);


    ui->txt_ebvMin->setEnabled(true);
    ui->txt_ebvMax->setEnabled(true);
    ui->txt_ebvStep->setEnabled(true);


    ui->txt_zMin->setEnabled(true);
    ui->txt_zMax->setEnabled(true);
    ui->txt_zStep->setEnabled(true);

    ui->txt_name->setEnabled(true);

    ui->tableView_ParameterRule->setEnabled(false);
}

void DialogModelSet::turnControlsInView(){

    bool accepte_new_line = !m_singe_line || ui->tableView_ParameterRule->model()->rowCount()==0;
    m_insert=false;
    ui->buttonBox->setEnabled(true);

    bool has_rule=ui->tableView_ParameterRule->hasSelectedPArameterRule();

    ui->btn_new->setEnabled(!m_view_popup && accepte_new_line);
    ui->btn_duplicate->setEnabled(!m_view_popup && has_rule && accepte_new_line);
    ui->btn_delete->setEnabled(!m_view_popup && has_rule);

    ui->btn_edit->setEnabled(!m_view_popup && has_rule);
    ui->btn_cancel->setEnabled(false);
    ui->btn_save->setEnabled(false);

    static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->setEnabled(false);
    static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->setEnabled(false);

    ui->txt_ebvMin->setEnabled(false);
    ui->txt_ebvMax->setEnabled(false);
    ui->txt_ebvStep->setEnabled(false);


    ui->txt_zMin->setEnabled(false);
    ui->txt_zMax->setEnabled(false);
    ui->txt_zStep->setEnabled(false);

    ui->txt_name->setEnabled(false);

    ui->tableView_ParameterRule->setEnabled(true);
}


void DialogModelSet::on_buttonBox_rejected()
{
    this->popupClosing(ui->tableView_ParameterRule->getModel()->getParameterRules());
}

void DialogModelSet::on_btn_new_clicked()
{
    ui->tableView_ParameterRule->newRule(false);
    m_insert=true;
    turnControlsInEdition();
}

void DialogModelSet::on_btn_delete_clicked()
{
    if (QMessageBox::question( this, "Confirm deletion...",
                                  "Do you really want to delete this Parameter Rule?",
                                  QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes){
        ui->tableView_ParameterRule->deletSelectedRule();
        turnControlsInView();
    }
}

void DialogModelSet::on_btn_duplicate_clicked()
{
    ui->tableView_ParameterRule->newRule(true);
    m_insert=true;
    turnControlsInEdition();
}

void DialogModelSet::on_btn_edit_clicked()
{
    turnControlsInEdition();
}

void DialogModelSet::on_btn_cancel_clicked()
{
    if (m_insert){
        ui->tableView_ParameterRule->deletSelectedRule();
    }else{
        auto selected_rule = ui->tableView_ParameterRule->getSelectedRule();

        ui->txt_name->setText(QString::fromStdString(selected_rule.getName()));
        // SED
        static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->setState(selected_rule.getSedRootObject(),selected_rule.getExcludedSeds());

        // Reddening Curve
        static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->setState(selected_rule.getReddeningRootObject(),selected_rule.getExcludedReddenings());

        // E(B-V)-range
        ui->txt_ebvMin->setText(QString::number(selected_rule.getEbvRange().getMin()));
        ui->txt_ebvMax->setText(QString::number(selected_rule.getEbvRange().getMax()));
        ui->txt_ebvStep->setText(QString::number(selected_rule.getEbvRange().getStep()));
        // z-range
        ui->txt_zMin->setText(QString::number(selected_rule.getZRange().getMin()));
        ui->txt_zMax->setText(QString::number(selected_rule.getZRange().getMax()));
        ui->txt_zStep->setText(QString::number(selected_rule.getZRange().getStep()));
    }
    turnControlsInView();
}

void DialogModelSet::on_btn_save_clicked()
{
    auto sed_res = static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->getRootSelection();
    auto red_res =  static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->getRootSelection();

    if(!sed_res.first|| !red_res.first){
        QMessageBox::warning( this, "Missing Data...",
                                          "Please provide SED(s) and Reddening Curve(s) selection.",
                                          QMessageBox::Ok );
        return;
    }

    if (!ui->tableView_ParameterRule->checkNameAlreadyUsed(ui->txt_name->text().toStdString())){
      QMessageBox::warning( this, "Duplicate Name...",
                           "The name you enter is already used, please provide another name.",
                           QMessageBox::Ok );
      return;
    }



  double ebv_min = ui->txt_ebvMin->text().toDouble();
  double ebv_max = ui->txt_ebvMax->text().toDouble();
  double ebv_step =ui->txt_ebvStep->text().toDouble();

  if ((ebv_min<0.) || (ebv_min>ebv_max) || (ebv_step<0.) ) {
    QMessageBox::warning(this, "Not acceptable Range...",
        "The E(B-V) range you have provided is not well formated, please check it.",
        QMessageBox::Ok);
    return;
  }

  double z_min = ui->txt_zMin->text().toDouble();
  double z_max = ui->txt_zMax->text().toDouble();
  double z_step =ui->txt_zStep->text().toDouble();
  if ((z_min<0.) || (z_min>z_max) || (z_step<0.) ) {
    QMessageBox::warning(this, "Not acceptable Range...",
        "The redshift range you have provided is not well formated, please check it.",
        QMessageBox::Ok);
    return;
  }

    ui->tableView_ParameterRule->setNameToSelectedRule(ui->txt_name->text().toStdString());


    // SED
    string sed_root = sed_res.second;
    vector<string> sed_excl = static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->getExclusions(sed_root);
    ui->tableView_ParameterRule->setSedsToSelectedRule(std::move(sed_root),std::move(sed_excl));

    // Reddeing Curves
    string red_root =red_res.second;
    vector<string> red_excl = static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->getExclusions(red_root);
    ui->tableView_ParameterRule->setRedCurvesToSelectedRule(std::move(red_root),std::move(red_excl));

    // E(B-V)-range
    Range new_ebv{ebv_min, ebv_max, ebv_step};

    // z-range
    Range new_z{z_min, z_max, z_step};

    ui->tableView_ParameterRule->setRangesToSelectedRule(std::move(new_ebv),std::move(new_z));

    turnControlsInView();

    auto selected_rule = ui->tableView_ParameterRule->getSelectedRule();

    // SED
    static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->setState(selected_rule.getSedRootObject(),selected_rule.getExcludedSeds());

    // Reddening Curve
    static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->setState(selected_rule.getReddeningRootObject(),selected_rule.getExcludedReddenings());

}

 void DialogModelSet::selectionChanged(QModelIndex new_index, QModelIndex){
     if (new_index.isValid()){
         ParameterRuleModel* model=ui->tableView_ParameterRule->getModel();

         auto selected_rule = model->getRule(new_index.row());

         ui->txt_name->setText(QString::fromStdString(selected_rule.getName()));

         // SED
         static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->setState(selected_rule.getSedRootObject(),selected_rule.getExcludedSeds());

         // Reddening Curve
         static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->setState(selected_rule.getReddeningRootObject(),selected_rule.getExcludedReddenings());

         // E(B-V)-range
         ui->txt_ebvMin->setText(QString::number(selected_rule.getEbvRange().getMin()));
         ui->txt_ebvMax->setText(QString::number(selected_rule.getEbvRange().getMax()));
         ui->txt_ebvStep->setText(QString::number(selected_rule.getEbvRange().getStep()));
         // z-range
         ui->txt_zMin->setText(QString::number(selected_rule.getZRange().getMin()));
         ui->txt_zMax->setText(QString::number(selected_rule.getZRange().getMax()));
         ui->txt_zStep->setText(QString::number(selected_rule.getZRange().getStep()));
     }
     else{

         static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->setState("",{});
         static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->setState("",{});

         // E(B-V)-range
         ui->txt_ebvMin->setText("");
         ui->txt_ebvMax->setText("");
         ui->txt_ebvStep->setText("");
         // z-range
         ui->txt_zMin->setText("");
         ui->txt_zMax->setText("");
         ui->txt_zStep->setText("");
     }
     turnControlsInView();
 }

}
}
