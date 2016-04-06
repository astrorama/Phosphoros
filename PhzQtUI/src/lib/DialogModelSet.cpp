
#include <QMessageBox>
#include "FileUtils.h"
#include "FormUtils.h"
#include "PhzQtUI/DialogModelSet.h"
#include "ui_DialogModelSet.h"
#include "PhzQtUI/XYDataSetTreeModel.h"

#include "PhzQtUI/GridButton.h"

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
    ui->txt_new_ebv->setValidator( new QDoubleValidator(0, 100, 4, this) );
    // z-range
    ui->txt_zMin->setValidator( new QDoubleValidator(0, 100, 2, this) );
    ui->txt_zMax->setValidator( new QDoubleValidator(0, 100, 2, this) );
    ui->txt_zStep->setValidator( new QDoubleValidator(0, 100, 4, this) );
    ui->txt_new_z->setValidator( new QDoubleValidator(0, 100, 4, this) );

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

    connect(ui->gb_ebv_value, SIGNAL(clicked(bool)), this, SLOT(set_Checked_ebv_range(bool)));
    connect(ui->gb_ebv_range, SIGNAL(clicked(bool)), this, SLOT(set_Checked_ebv_value(bool)));


    connect(ui->gb_red_value, SIGNAL(clicked(bool)), this, SLOT(set_Checked_red_range(bool)));
    connect(ui->gb_red_range, SIGNAL(clicked(bool)), this, SLOT(set_Checked_red_value(bool)));

}


DialogModelSet::~DialogModelSet()
{
}

void DialogModelSet::set_Checked_ebv_value(bool un_checked ){
  ui->gb_ebv_value->setChecked(!un_checked);
}

void DialogModelSet::set_Checked_ebv_range(bool un_checked ){
  ui->gb_ebv_range->setChecked(!un_checked);
}

void DialogModelSet::set_Checked_red_value(bool un_checked ){
  ui->gb_red_value->setChecked(!un_checked);
}

void DialogModelSet::set_Checked_red_range(bool un_checked ){
  ui->gb_red_range->setChecked(!un_checked);
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

    disconnect(ui->tableView_ParameterRule,SIGNAL(doubleClicked(QModelIndex)),0,0);
     connect(ui->tableView_ParameterRule,
                       SIGNAL(doubleClicked(QModelIndex)),
                       SLOT(setGridDoubleClicked(QModelIndex)));

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




    ui->txt_name->setEnabled(true);

    ui->tableView_ParameterRule->setEnabled(false);

    ui->gb_ebv_value->setEnabled(true);
    ui->gb_ebv_range->setEnabled(true);
    ui->gb_red_value->setEnabled(true);
    ui->gb_red_range->setEnabled(true);
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



    ui->txt_name->setEnabled(false);

    ui->tableView_ParameterRule->setEnabled(true);

    ui->gb_ebv_value->setEnabled(false);
    ui->gb_ebv_range->setEnabled(false);
    ui->gb_red_value->setEnabled(false);
    ui->gb_red_range->setEnabled(false);
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


void DialogModelSet::setGridDoubleClicked(QModelIndex){
  if (!m_view_popup){
    turnControlsInEdition();
  }
}

void DialogModelSet::on_btn_edit_clicked()
{
    turnControlsInEdition();
}

void DialogModelSet::on_btn_cancel_clicked()
{
  if (m_insert) {
    ui->tableView_ParameterRule->deletSelectedRule();
  } else {
    auto selected_rule = ui->tableView_ParameterRule->getSelectedRule();

    ui->txt_name->setText(QString::fromStdString(selected_rule.getName()));
    // SED
    static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->setState(
        selected_rule.getSedRootObject(), selected_rule.getExcludedSeds());

    // Reddening Curve
    static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->setState(
        selected_rule.getReddeningRootObject(),
        selected_rule.getExcludedReddenings());

    // E(B-V)-range
    ui->gb_ebv_range->setChecked(selected_rule.hasEbvRange());
    ui->gb_ebv_value->setChecked(!selected_rule.hasEbvRange());
    refreshEbvValues();
    if (selected_rule.hasEbvRange()) {
      // E(B-V)-range
      ui->txt_ebvMin->setText(
          QString::number(selected_rule.getEbvRange().getMin()));
      ui->txt_ebvMax->setText(
          QString::number(selected_rule.getEbvRange().getMax()));
      ui->txt_ebvStep->setText(
          QString::number(selected_rule.getEbvRange().getStep()));
      m_current_ebv_values= {};
    } else {
      ui->txt_ebvMin->clear();
      ui->txt_ebvMax->clear();
      ui->txt_ebvStep->clear();
      m_current_ebv_values=selected_rule.getEbvValues();
    }
    addEbvValues(m_current_z_values);

    ui->gb_red_range->setChecked(selected_rule.hasRedshiftRange());
    ui->gb_red_value->setChecked(!selected_rule.hasRedshiftRange());
    refreshZValues();
    if (selected_rule.hasRedshiftRange()) {
      ui->txt_zMin->setText(
          QString::number(selected_rule.getZRange().getMin()));
      ui->txt_zMax->setText(
          QString::number(selected_rule.getZRange().getMax()));
      ui->txt_zStep->setText(
          QString::number(selected_rule.getZRange().getStep()));
      m_current_z_values= {};
    } else {
      ui->txt_zMin->clear();
      ui->txt_zMax->clear();
      ui->txt_zStep->clear();

      m_current_z_values=selected_rule.getRedshiftValues();
    }

    addZValues(m_current_z_values);
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



    bool has_ebv_range = ui->gb_ebv_range->isChecked();
    double ebv_min;
    double ebv_max;
    double ebv_step;

    if (has_ebv_range){
        ebv_min = FormUtils::parseToDouble(ui->txt_ebvMin->text());
        ebv_max = FormUtils::parseToDouble(ui->txt_ebvMax->text());
        ebv_step = FormUtils::parseToDouble(ui->txt_ebvStep->text());

        if ((ebv_min<0.) || (ebv_min>ebv_max) || (ebv_step<0.) ) {
          QMessageBox::warning(this, "Not acceptable Range...",
              "The E(B-V) range you have provided is not well formated, please check it.",
              QMessageBox::Ok);
          return;
        }
    } else {
      if (m_current_ebv_values.size()<1 ) {
        QMessageBox::warning(this, "Not acceptable Value...",
                    "Please porovide at least one E(B-V) value.",
                    QMessageBox::Ok);
        return;
      }
    }

    bool has_red_range = ui->gb_red_range->isChecked();
    double z_min;
    double z_max;
    double z_step;

    if (has_red_range) {
      z_min = FormUtils::parseToDouble(ui->txt_zMin->text());
      z_max = FormUtils::parseToDouble(ui->txt_zMax->text());
      z_step = FormUtils::parseToDouble(ui->txt_zStep->text());

      if ((z_min < 0.) || (z_min > z_max) || (z_step < 0.)) {
        QMessageBox::warning(this, "Not acceptable Range...",
            "The redshift range you have provided is not well formated, please check it.",
            QMessageBox::Ok);
        return;
      }
    } else {

      if (m_current_z_values.size() < 1) {
        QMessageBox::warning(this, "Not acceptable Value...",
            "Please provide at least one Redshift value.",
            QMessageBox::Ok);
        return;
      }
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



    // E(B-V)
    ui->tableView_ParameterRule->setHasEbvRangeToSelectedRule(has_ebv_range);
    if (has_ebv_range){
      Range new_ebv{ebv_min, ebv_max, ebv_step};
      ui->tableView_ParameterRule->setEbvRangeToSelectedRule(new_ebv);
    } else {
      ui->tableView_ParameterRule->setEbvValuesToSelectedRule(m_current_ebv_values);
    }


    // z
    ui->tableView_ParameterRule->setHasRedshiftRangeToSelectedRule(has_red_range);
    if (has_red_range){
      Range new_z{z_min, z_max, z_step};
          ui->tableView_ParameterRule->setRedshiftRangeToSelectedRule(new_z);
    } else {
          ui->tableView_ParameterRule->setRedshiftValuesToSelectedRule(m_current_z_values);
    }


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

         ui->gb_ebv_range->setChecked(selected_rule.hasEbvRange());
         ui->gb_ebv_value->setChecked(!selected_rule.hasEbvRange());
         refreshEbvValues();
         if (selected_rule.hasEbvRange()){
           // E(B-V)-range
           ui->txt_ebvMin->setText(QString::number(selected_rule.getEbvRange().getMin()));
           ui->txt_ebvMax->setText(QString::number(selected_rule.getEbvRange().getMax()));
           ui->txt_ebvStep->setText(QString::number(selected_rule.getEbvRange().getStep()));
           m_current_ebv_values={};
         } else {
           ui->txt_ebvMin->clear();
           ui->txt_ebvMax->clear();
           ui->txt_ebvStep->clear();
           m_current_ebv_values = selected_rule.getEbvValues();
         }
         addEbvValues(m_current_ebv_values);

         ui->gb_red_range->setChecked(selected_rule.hasRedshiftRange());
         ui->gb_red_value->setChecked(!selected_rule.hasRedshiftRange());
         refreshZValues();
         if (selected_rule.hasRedshiftRange()){
           ui->txt_zMin->setText(QString::number(selected_rule.getZRange().getMin()));
           ui->txt_zMax->setText(QString::number(selected_rule.getZRange().getMax()));
           ui->txt_zStep->setText(QString::number(selected_rule.getZRange().getStep()));
           m_current_z_values={};
         } else {
           ui->txt_zMin->clear();
           ui->txt_zMax->clear();
           ui->txt_zStep->clear();

           m_current_z_values=selected_rule.getRedshiftValues();
         }

         addZValues(m_current_z_values);
     }
     else{

         static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->setState("",{});
         static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->setState("",{});

         // E(B-V)-range
         ui->txt_ebvMin->clear();
         ui->txt_ebvMax->clear();
         ui->txt_ebvStep->clear();
         refreshEbvValues();
         // z-range
         ui->txt_zMin->clear();
         ui->txt_zMax->clear();
         ui->txt_zStep->clear();
         refreshZValues();
     }
     turnControlsInView();
 }


 void DialogModelSet::refreshZValues(){
   size_t i = 0;
     for (auto child : ui->z_frame->children()) {
       if (i > 0) {
         delete child;
       }
       ++i;
     }
 }


 void DialogModelSet::addZValues(std::set<double> values){
   auto value_iter = values.begin();
   for (size_t i=0; i<values.size(); ++i){
       auto titleFrame = new QFrame();
       titleFrame->setFrameStyle(QFrame::Box);
       auto titleLayout = new QHBoxLayout();
       titleFrame->setLayout(titleLayout);
       auto txt_title = new QLabel(QString::number(*value_iter));
       titleLayout->addWidget(txt_title);
       auto btn_del = new GridButton(i,0,"-");
       btn_del->setMaximumWidth(30);
       btn_del->setMaximumHeight(20);
       connect(btn_del,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onDeleteZClicked(size_t,size_t)));
       titleLayout->addWidget(btn_del);
       ui->verticalLayout_z->addWidget(titleFrame);
       value_iter++;
     }
 }

 void DialogModelSet::on_btn_add_z_clicked(){
   double new_z = ui->txt_new_z->text().toDouble();
   m_current_z_values.insert(new_z);
   refreshZValues();
   addZValues(m_current_z_values);
   ui->txt_new_z->setText("");
 }

 void DialogModelSet::onDeleteZClicked(size_t index,size_t ){
   size_t running=0;
   auto z_iter = m_current_z_values.begin();
   while(true){
      if (running==index){
        m_current_z_values.erase(z_iter);
        break;
      }
      ++running;
      ++z_iter;
    }

   refreshZValues();
   addZValues(m_current_z_values);
 }

 void DialogModelSet::refreshEbvValues(){
   size_t i = 0;
     for (auto child : ui->ebv_frame->children()) {
       if (i > 0) {
         delete child;
       }
       ++i;
     }
 }


 void DialogModelSet::addEbvValues(std::set<double> values){
   auto value_iter = values.begin();
   for (size_t i=0; i<values.size(); ++i){
       auto titleFrame = new QFrame();
       titleFrame->setFrameStyle(QFrame::Box);
       auto titleLayout = new QHBoxLayout();
       titleFrame->setLayout(titleLayout);
       auto txt_title = new QLabel(QString::number(*value_iter));
       titleLayout->addWidget(txt_title);
       auto btn_del = new GridButton(i,0,"-");
       btn_del->setMaximumWidth(30);
       btn_del->setMaximumHeight(20);
       connect(btn_del,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onDeleteZClicked(size_t,size_t)));
       titleLayout->addWidget(btn_del);
       ui->verticalLayout_ebv->addWidget(titleFrame);
       value_iter++;
     }
 }

 void DialogModelSet::on_btn_add_ebv_clicked(){
   double new_ebv = ui->txt_new_ebv->text().toDouble();
   m_current_ebv_values.insert(new_ebv);
   refreshEbvValues();
   addEbvValues(m_current_ebv_values);
   ui->txt_new_ebv->setText("");
 }

 void DialogModelSet::onDeleteEbvClicked(size_t index,size_t ){
   size_t running=0;
   auto ebv_iter = m_current_ebv_values.begin();
   while(true){
      if (running==index){
        m_current_ebv_values.erase(ebv_iter);
        break;
      }
      ++running;
      ++ebv_iter;
    }

   refreshEbvValues();
   addEbvValues(m_current_ebv_values);
 }

}
}
