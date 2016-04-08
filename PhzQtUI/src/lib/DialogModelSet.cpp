
#include <QMessageBox>
#include <QStandardItemModel>
#include "FileUtils.h"
#include "FormUtils.h"
#include "PhzQtUI/DialogModelSet.h"
#include "ui_DialogModelSet.h"
#include "PhzQtUI/XYDataSetTreeModel.h"

#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/DoubleValidatorItemDelegate.h"
#include "PhzQtUI/ParameterRule.h"

using namespace std;

namespace Euclid{
namespace PhzQtUI {

DialogModelSet::DialogModelSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogModelSet)
{
    ui->setupUi(this);

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


    QStandardItemModel* z_model = new QStandardItemModel();
    z_model->setColumnCount(5);

    QStringList  z_setHeaders;
    z_setHeaders<<"Min"<<"Max"<<"Step"<<""<<"ref";
    z_model->setHorizontalHeaderLabels(z_setHeaders);
    ui->z_table->setColumnHidden(4,true);
    ui->z_table->setModel(z_model);
    ui->z_table->setEditTriggers(QAbstractItemView::DoubleClicked);
    DoubleValidatorItemDelegate *itDelegate = new  DoubleValidatorItemDelegate();
    ui->z_table->setItemDelegate(itDelegate);

    QStandardItemModel* ebv_model = new QStandardItemModel();
    ebv_model->setColumnCount(5);

    QStringList ebv_setHeaders;
    ebv_setHeaders << "Min" << "Max" << "Step" << "" << "ref";
    ebv_model->setHorizontalHeaderLabels(ebv_setHeaders);
    ui->ebv_table->setColumnHidden(4, true);
    ui->ebv_table->setModel(ebv_model);
    ui->ebv_table->setEditTriggers(QAbstractItemView::DoubleClicked);
    DoubleValidatorItemDelegate *ebv_itDelegate = new DoubleValidatorItemDelegate();
    ui->ebv_table->setItemDelegate(ebv_itDelegate);





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

  // Redshift & E(B-V)
    ui->txt_z_values->setEnabled(true);
    ui->z_table->setEnabled(true);
    ui->txt_ebv_values->setEnabled(true);
    ui->ebv_table->setEnabled(true);
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

    // Redshift & E(B-V)

    ui->txt_z_values->setEnabled(false);
    ui->z_table->setEnabled(false);
    ui->txt_ebv_values->setEnabled(false);
    ui->ebv_table->setEnabled(false);
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

    // E(B-V)
    populateEbvRangesAndValues(selected_rule);

  // Redshift
    populateZRangesAndValues(selected_rule);

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


    ui->tableView_ParameterRule->setNameToSelectedRule(ui->txt_name->text().toStdString());


    // SED
    string sed_root = sed_res.second;
    vector<string> sed_excl = static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->getExclusions(sed_root);
    ui->tableView_ParameterRule->setSedsToSelectedRule(std::move(sed_root),std::move(sed_excl));

    // Reddeing Curves
    string red_root =red_res.second;
    vector<string> red_excl = static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->getExclusions(red_root);
    ui->tableView_ParameterRule->setRedCurvesToSelectedRule(std::move(red_root),std::move(red_excl));

    // Redshift
    std::vector<Range> new_z_ranges{};
    for (int i = 0; i<ui->z_table->model()->rowCount()-1; ++i){
      double min = ui->z_table->model()->data(ui->z_table->model()->index(i,0)).toDouble();
      double max = ui->z_table->model()->data(ui->z_table->model()->index(i,1)).toDouble();
      double step = ui->z_table->model()->data(ui->z_table->model()->index(i,2)).toDouble();
      new_z_ranges.push_back({min,max,step});
    }
    ui->tableView_ParameterRule->setRedshiftRangesToSelectedRule(std::move(new_z_ranges));
    auto new_z_values = ParameterRule::parseValueList(ui->txt_z_values->text().toStdString());
    ui->tableView_ParameterRule->setRedshiftValuesToSelectedRule(std::move(new_z_values));

    ui->txt_z_values->setText(QString::fromStdString(ui->tableView_ParameterRule->getSelectedRule().getRedshiftStringValueList()));


    // E(B-V)
    std::vector<Range> new_ebv_ranges{};
    for (int i = 0; i<ui->ebv_table->model()->rowCount()-1; ++i){
      double min = ui->ebv_table->model()->data(ui->ebv_table->model()->index(i,0)).toDouble();
      double max = ui->ebv_table->model()->data(ui->ebv_table->model()->index(i,1)).toDouble();
      double step = ui->ebv_table->model()->data(ui->ebv_table->model()->index(i,2)).toDouble();
      new_ebv_ranges.push_back({min,max,step});
    }
    ui->tableView_ParameterRule->setEbvRangesToSelectedRule(std::move(new_ebv_ranges));
    auto new_ebv_values = ParameterRule::parseValueList(ui->txt_ebv_values->text().toStdString());
    ui->tableView_ParameterRule->setEbvValuesToSelectedRule(std::move(new_ebv_values));
    ui->txt_ebv_values->setText(QString::fromStdString(ui->tableView_ParameterRule->getSelectedRule().getEbvStringValueList()));


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


         // Redshift & E(B-V)
         populateZRangesAndValues(selected_rule);
         populateEbvRangesAndValues(selected_rule);
     }
     else{

         static_cast<XYDataSetTreeModel*>(ui->treeView_Sed->model())->setState("",{});
         static_cast<XYDataSetTreeModel*>(ui->treeView_Reddening->model())->setState("",{});

         // Redshift & E(B-V)
         for (int i = ui->z_table->model()->rowCount()-1; i>=0;--i){
            ui->z_table->model()->removeRow(i);
         }
         ui->txt_z_values->clear();
         for (int i = ui->ebv_table->model()->rowCount()-1; i>=0;--i){
            ui->ebv_table->model()->removeRow(i);
         }
         ui->txt_ebv_values->clear();

     }
     turnControlsInView();
 }


 void DialogModelSet::populateZRangesAndValues(ParameterRule selected_rule){
   for (int i = ui->z_table->model()->rowCount()-1; i>=0;--i){
              ui->z_table->model()->removeRow(i);
            }

            int i=0;
            m_current_z_range_id=0;
            for (auto& range : selected_rule.getZRanges()){
              QList<QStandardItem*> items;
              items.push_back(new QStandardItem(QString::number(range.getMin())));
              items.push_back(new QStandardItem(QString::number(range.getMax())));
              items.push_back(new QStandardItem(QString::number(range.getStep())));
              items.push_back(new QStandardItem());
              items.push_back(new QStandardItem(QString::number(m_current_z_range_id)));

              static_cast<QStandardItemModel*>(ui->z_table->model())->appendRow(items);

              QModelIndex index = ui->z_table->model()->index(i,3);

              GridButton* button = new GridButton(i,m_current_z_range_id,"-");

              connect(button,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onZDeleteClicked(size_t,size_t)));
              ui->z_table->setIndexWidget(index, button);
              ++i;
              ++m_current_z_range_id;
            }
            ui->z_table->model()->insertRow(i);
            QModelIndex index_add = ui->z_table->model()->index(i,3);

            QPushButton* button_Add = new QPushButton("Add");

            connect(button_Add,SIGNAL(clicked()),this,SLOT(onZAddClicked()));
            ui->z_table->setIndexWidget(index_add, button_Add);
            ui->z_table->setSpan(i,0,1,4);
            ui->z_table->setColumnHidden(4,true);

            ui->txt_z_values->setText(QString::fromStdString(selected_rule.getRedshiftStringValueList()));
 }


 void DialogModelSet::populateEbvRangesAndValues(ParameterRule selected_rule){
   for (int i = ui->ebv_table->model()->rowCount()-1; i>=0;--i){
              ui->ebv_table->model()->removeRow(i);
            }

            int i=0;
            m_current_ebv_range_id=0;
            for (auto& range : selected_rule.getEbvRanges()){
              QList<QStandardItem*> items;
              items.push_back(new QStandardItem(QString::number(range.getMin())));
              items.push_back(new QStandardItem(QString::number(range.getMax())));
              items.push_back(new QStandardItem(QString::number(range.getStep())));
              items.push_back(new QStandardItem());
              items.push_back(new QStandardItem(QString::number(m_current_ebv_range_id)));

              static_cast<QStandardItemModel*>(ui->ebv_table->model())->appendRow(items);

              QModelIndex index = ui->ebv_table->model()->index(i,3);

              GridButton* button = new GridButton(i,m_current_ebv_range_id,"-");

              connect(button,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onEbvDeleteClicked(size_t,size_t)));
              ui->ebv_table->setIndexWidget(index, button);
              ++i;
              ++m_current_ebv_range_id;
            }
            ui->ebv_table->model()->insertRow(i);
            QModelIndex index_add = ui->ebv_table->model()->index(i,3);

            QPushButton* button_Add = new QPushButton("Add");

            connect(button_Add,SIGNAL(clicked()),this,SLOT(onEbvAddClicked()));
            ui->ebv_table->setIndexWidget(index_add, button_Add);
            ui->ebv_table->setSpan(i,0,1,4);
            ui->ebv_table->setColumnHidden(4,true);

            ui->txt_ebv_values->setText(QString::fromStdString(selected_rule.getEbvStringValueList()));
 }

 void DialogModelSet::onZDeleteClicked(size_t,size_t ref){
   int rows = ui->z_table->model()->rowCount();
   for(int i = 0; i < rows; ++i)
   {
       if(ui->z_table->model()->data( ui->z_table->model()->index(i,4)) == QString::number(ref))
       {
         ui->z_table->model()->removeRow(i);
         break;
       }
   }
 }

 void DialogModelSet::onEbvDeleteClicked(size_t,size_t ref){
   int rows = ui->ebv_table->model()->rowCount();
   for(int i = 0; i < rows; ++i)
   {
       if(ui->ebv_table->model()->data( ui->ebv_table->model()->index(i,4)) == QString::number(ref))
       {
         ui->ebv_table->model()->removeRow(i);
         break;
       }
   }
 }

 void DialogModelSet::onZAddClicked(){
   int id = ui->z_table->model()->rowCount()-1;
   ui->z_table->model()->insertRow(id);

   GridButton* button = new GridButton(id,m_current_z_range_id,"-");

   ui->z_table->model()->setData( ui->z_table->model()->index(id,0), QString::number(0.));
   ui->z_table->model()->setData( ui->z_table->model()->index(id,1), QString::number(0.));
   ui->z_table->model()->setData( ui->z_table->model()->index(id,2), QString::number(0.));
   ui->z_table->model()->setData( ui->z_table->model()->index(id,4), QString::number(m_current_z_range_id));
   ++m_current_z_range_id;
   connect(button,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onZDeleteClicked(size_t,size_t)));

   QModelIndex index = ui->z_table->model()->index(id,3);
   ui->z_table->setIndexWidget(index, button);
 }

 void DialogModelSet::onEbvAddClicked(){
   int id = ui->ebv_table->model()->rowCount()-1;
   ui->ebv_table->model()->insertRow(id);

   GridButton* button = new GridButton(id,m_current_ebv_range_id,"-");

   ui->ebv_table->model()->setData( ui->ebv_table->model()->index(id,0), QString::number(0.));
   ui->ebv_table->model()->setData( ui->ebv_table->model()->index(id,1), QString::number(0.));
   ui->ebv_table->model()->setData( ui->ebv_table->model()->index(id,2), QString::number(0.));
   ui->ebv_table->model()->setData( ui->ebv_table->model()->index(id,4), QString::number(m_current_ebv_range_id));
   ++m_current_ebv_range_id;
   connect(button,SIGNAL(GridButtonClicked(size_t,size_t)),this,SLOT(onEbvDeleteClicked(size_t,size_t)));

   QModelIndex index = ui->ebv_table->model()->index(id,3);
   ui->ebv_table->setIndexWidget(index, button);
 }




}
}
