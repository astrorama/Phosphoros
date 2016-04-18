
#include <QMessageBox>
#include <QStandardItemModel>
#include "ElementsKernel/Exception.h"
#include "FileUtils.h"
#include "FormUtils.h"
#include "PhzQtUI/DialogModelSet.h"
#include "ui_DialogModelSet.h"
#include "PhzQtUI/DataSetTreeModel.h"

#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/DoubleValidatorItemDelegate.h"
#include "PhzQtUI/ParameterRule.h"

using namespace std;

namespace Euclid{
namespace PhzQtUI {

DialogModelSet::DialogModelSet(DatasetRepo seds_repository,
    DatasetRepo redenig_curves_repository, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogModelSet),
    m_seds_repository(seds_repository),
    m_redenig_curves_repository(redenig_curves_repository)
{
    ui->setupUi(this);

    DataSetTreeModel* treeModel_sed = new DataSetTreeModel(m_seds_repository);
    treeModel_sed->load();
    ui->treeView_Sed->setModel(treeModel_sed);
    ui->treeView_Sed->collapseAll();

    connect(treeModel_sed, SIGNAL(itemChanged(QStandardItem*)), treeModel_sed,
                 SLOT(onItemChanged(QStandardItem*)));

    if (treeModel_sed->rowCount() == 0){
         QMessageBox::warning(this, "No available SED...",
                 "There is no SED to select. "
                 "You can provide and manage SEDs in the \"Configuration/Aux. Data\" page.",
                 QMessageBox::Ok);
    }

    DataSetTreeModel* treeModel_red = new DataSetTreeModel(m_redenig_curves_repository);
    treeModel_red->load();
    ui->treeView_Reddening->setModel(treeModel_red);
    ui->treeView_Reddening->collapseAll();

    connect( treeModel_red, SIGNAL(itemChanged(QStandardItem*)), treeModel_red,
                 SLOT(onItemChanged(QStandardItem*)));

    if (treeModel_red->rowCount() == 0){
         QMessageBox::warning(this, "No available Reddening Curve...",
                 "There is no reddening curve to select. "
                 "You can provide and manage reddening curves in the \"Configuration/Aux. Data\" page.",
                 QMessageBox::Ok);
    }

    QRegExp rx("(\\s*(\\d+|\\d+\\.\\d*)\\s*(,|$))*");
    ui->txt_ebv_values->setValidator(new QRegExpValidator(rx));
    ui->txt_z_values->setValidator(new QRegExpValidator(rx));


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
    ui->tableView_ParameterRule->loadParameterRules(
        init_parameter_rules,
        m_seds_repository,
        m_redenig_curves_repository);


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

    static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->setEnabled(true);
    static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setEnabled(true);

    ui->txt_name->setEnabled(true);
    ui->tableView_ParameterRule->setEnabled(false);

  // Redshift & E(B-V)
    ui->txt_ebv_values->setEnabled(true);
    ui->btn_add_z_range->setEnabled(true);
    SetRangeControlsEnabled(ui->Layout_ebv_range,true);

    ui->txt_z_values->setEnabled(true);
    ui->btn_add_ebv_range->setEnabled(true);
    SetRangeControlsEnabled(ui->Layout_z_range,true);
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

    static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->setEnabled(false);
    static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setEnabled(false);



    ui->txt_name->setEnabled(false);

    ui->tableView_ParameterRule->setEnabled(true);

    // Redshift & E(B-V)
    ui->txt_ebv_values->setEnabled(false);
    ui->btn_add_z_range->setEnabled(false);
    SetRangeControlsEnabled(ui->Layout_ebv_range,false);

    ui->txt_z_values->setEnabled(false);
    ui->btn_add_ebv_range->setEnabled(false);
    SetRangeControlsEnabled(ui->Layout_z_range,false);
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
    turnControlsInView();
  } else {
    auto selected_rule = ui->tableView_ParameterRule->getSelectedRule();

    ui->txt_name->setText(QString::fromStdString(selected_rule.getName()));

    // E(B-V)
    populateEbvRangesAndValues(selected_rule);

    // Redshift
    populateZRangesAndValues(selected_rule);

    turnControlsInView();

    // SED
    static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->setState(selected_rule.getSedSelection());
    // Reddening Curve
    static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setState(selected_rule.getRedCurveSelection());

  }
}



void DialogModelSet::on_btn_save_clicked()
{
  auto sed_selection = static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->getState();
  auto red_curve_selection = static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->getState();

  if (sed_selection.isEmpty() ){
    QMessageBox::warning( this,
                          "Missing Data...",
                          "Please provide SED(s) and Reddening Curve(s) selection.",
                          QMessageBox::Ok );
    return;
  }

  if (red_curve_selection.isEmpty()){
    QMessageBox::warning( this,
                          "Missing Data...",
                          "Please provide SED(s) and Reddening Curve(s) selection.",
                          QMessageBox::Ok );
    return;
  }


  if (!ui->tableView_ParameterRule->checkNameAlreadyUsed(ui->txt_name->text().toStdString())){
    QMessageBox::warning( this,
                          "Duplicate Name...",
                          "The name you enter is already used, please provide another name.",
                          QMessageBox::Ok );
    return;
  }


    // Redshift and E(B-V)
    auto new_z_ranges = getRanges( ui->Layout_z_range);
    auto new_ebv_ranges = getRanges( ui->Layout_ebv_range);

    auto old_z_ranges =ui->tableView_ParameterRule->getSelectedRule().getZRanges();
    auto old_ebv_ranges =ui->tableView_ParameterRule->getSelectedRule().getEbvRanges();

    try {
        ui->tableView_ParameterRule->setRedshiftRangesToSelectedRule(std::move(new_z_ranges));
    } catch (const Elements::Exception& e) {
        QMessageBox::warning( this, "Error while setting redshift ranges...",
                                  e.what(),
                                  QMessageBox::Ok );

        ui->tableView_ParameterRule->setRedshiftRangesToSelectedRule(std::move(old_z_ranges));
        return;
    }

    try {
          ui->tableView_ParameterRule->setEbvRangesToSelectedRule(std::move(new_ebv_ranges));
      } catch (const Elements::Exception& e) {
          QMessageBox::warning( this, "Error while setting E(B-V) ranges...",
                                    e.what(),
                                    QMessageBox::Ok );

          ui->tableView_ParameterRule->setEbvRangesToSelectedRule(std::move(old_ebv_ranges));
          return;
      }

    ui->tableView_ParameterRule->setNameToSelectedRule(ui->txt_name->text().toStdString());


    // SED
    ui->tableView_ParameterRule->setSedsToSelectedRule(std::move(sed_selection));

    // Reddeing Curves
    ui->tableView_ParameterRule->setRedCurvesToSelectedRule(std::move(red_curve_selection));



    auto new_z_values = ParameterRule::parseValueList(ui->txt_z_values->text().toStdString());
    ui->tableView_ParameterRule->setRedshiftValuesToSelectedRule(std::move(new_z_values));

    ui->txt_z_values->setText(QString::fromStdString(ui->tableView_ParameterRule->getSelectedRule().getRedshiftStringValueList()));



    auto new_ebv_values = ParameterRule::parseValueList(ui->txt_ebv_values->text().toStdString());
    ui->tableView_ParameterRule->setEbvValuesToSelectedRule(std::move(new_ebv_values));
    ui->txt_ebv_values->setText(QString::fromStdString(ui->tableView_ParameterRule->getSelectedRule().getEbvStringValueList()));


    turnControlsInView();

    auto selected_rule = ui->tableView_ParameterRule->getSelectedRule();

    // SED
    static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->setState(selected_rule.getSedSelection());
    // Reddening Curve
    static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setState(selected_rule.getRedCurveSelection());


}


 void DialogModelSet::selectionChanged(QModelIndex new_index, QModelIndex){
     if (new_index.isValid()){
         ParameterRuleModel* model=ui->tableView_ParameterRule->getModel();

         auto selected_rule = model->getRule(new_index.row());

         ui->txt_name->setText(QString::fromStdString(selected_rule.getName()));

         // SED
         static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->setState(selected_rule.getSedSelection());
         // Reddening Curve
         static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setState(selected_rule.getRedCurveSelection());


         // Redshift & E(B-V)
         populateZRangesAndValues(selected_rule);
         populateEbvRangesAndValues(selected_rule);
     }
     else{

        // SED
        static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->setState({});
        // Reddening Curve
        static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setState({});


         cleanRangeControl( ui->Layout_z_range);
         ui->txt_z_values->clear();
         cleanRangeControl( ui->Layout_ebv_range);
         ui->txt_ebv_values->clear();

     }
     turnControlsInView();
 }


 void DialogModelSet::populateZRangesAndValues(ParameterRule selected_rule){
  ui->txt_z_values->setText(QString::fromStdString(selected_rule.getRedshiftStringValueList()));
  cleanRangeControl( ui->Layout_z_range);
  m_current_z_range_id = 0;

  for (auto& range : selected_rule.getZRanges()) {
    auto del_button = new GridButton(0, m_current_z_range_id, "Delete");
    connect(del_button, SIGNAL(GridButtonClicked(size_t,size_t)), this,
        SLOT(onZDeleteClicked(size_t,size_t)));
    ui->Layout_z_range->addWidget(createRangeControls(del_button, m_current_z_range_id, false, range));
    ++m_current_z_range_id;

  }
}

 void DialogModelSet::populateEbvRangesAndValues(ParameterRule selected_rule){
   ui->txt_ebv_values->setText(QString::fromStdString(selected_rule.getEbvStringValueList()));
   cleanRangeControl( ui->Layout_ebv_range);
   m_current_ebv_range_id = 0;

   for (auto& range : selected_rule.getEbvRanges()) {
       auto del_button = new GridButton(0, m_current_ebv_range_id, "Delete");
       connect(del_button, SIGNAL(GridButtonClicked(size_t,size_t)), this,
           SLOT(onEbvDeleteClicked(size_t,size_t)));
       ui->Layout_ebv_range->addWidget(createRangeControls(del_button, m_current_ebv_range_id, false, range));
       ++m_current_ebv_range_id;
     }
 }

 void DialogModelSet::onZDeleteClicked(size_t,size_t ref){
   deleteRangeAt(ui->Layout_z_range, ref);
 }

 void DialogModelSet::onEbvDeleteClicked(size_t,size_t ref){
   deleteRangeAt(ui->Layout_ebv_range, ref);
 }



void DialogModelSet::on_btn_add_z_range_clicked() {
  auto del_button = new GridButton(0, m_current_z_range_id, "Delete");
  connect(del_button, SIGNAL(GridButtonClicked(size_t,size_t)), this,
      SLOT(onZDeleteClicked(size_t,size_t)));
  ui->Layout_z_range->addWidget(createRangeControls(del_button, m_current_z_range_id, true));
  ++m_current_z_range_id;
  turnControlsInEdition();
}

void DialogModelSet::on_btn_add_ebv_range_clicked() {
  auto del_button = new GridButton(0, m_current_ebv_range_id, "Delete");
  connect(del_button, SIGNAL(GridButtonClicked(size_t,size_t)), this,
      SLOT(onEbvDeleteClicked(size_t,size_t)));
  ui->Layout_ebv_range->addWidget(createRangeControls(del_button, m_current_ebv_range_id, true));
  ++m_current_ebv_range_id;
  turnControlsInEdition();
}


QFrame* DialogModelSet::createRangeControls(GridButton* del_button, int range_id, bool enabled ){
   return createRangeControls(del_button, enabled, range_id,{-1,-1,-1});
 }

QFrame* DialogModelSet::createRangeControls(GridButton* del_button, int range_id, bool enabled, const Range& range){
  bool do_create_void = range.getStep()<0;

  auto range_frame = new QFrame();
  auto range_layout = new QHBoxLayout();
  range_frame->setLayout(range_layout);
  auto lbl_min = new QLabel("Min :");
  range_layout->addWidget(lbl_min);
  auto txt_min = new QLineEdit();
  if (!do_create_void){
    txt_min->setText(QString::number(range.getMin(),'g',15));
  }
  txt_min->setValidator(new QDoubleValidator(0, 10000, 20));
  txt_min->setEnabled(enabled);
  range_layout->addWidget(txt_min);

  auto lbl_max = new QLabel("Max :");
  range_layout->addWidget(lbl_max);
  auto txt_max = new QLineEdit();
  if (!do_create_void){
    txt_max->setText(QString::number(range.getMax(),'g',15));
  }
  txt_max->setValidator(new QDoubleValidator(0, 10000, 20));
  txt_max->setEnabled(enabled);
  range_layout->addWidget(txt_max);

  auto lbl_step = new QLabel("Step :");
  range_layout->addWidget(lbl_step);
  auto txt_step = new QLineEdit();
  if (!do_create_void){
    txt_step->setText(QString::number(range.getStep(),'g',15));
  }
  txt_step->setValidator(new QDoubleValidator(0, 10000, 20));
  txt_step->setEnabled(enabled);
  range_layout->addWidget(txt_step);


  del_button->setEnabled(enabled);
  range_layout->addWidget(del_button);

  auto lbl_id = new QLabel(QString::number(range_id));
  lbl_id->setVisible(false);
  range_layout->addWidget(lbl_id);

  return range_frame;
}




void DialogModelSet::cleanRangeControl(QVBoxLayout* ranges_layout) {
  for (int i = ranges_layout->count() - 1; i >= 0; --i) {
    QWidget *widget = ranges_layout->itemAt(i)->widget();
    if (widget != NULL) {
      delete widget;
    }
  }
}


void DialogModelSet::SetRangeControlsEnabled(QVBoxLayout* ranges_layout, bool is_enabled) {
  for (int i = 0; i < ranges_layout->count(); ++i) {
    QWidget *range_frame = ranges_layout->itemAt(i)->widget();
    if (range_frame != NULL) {
      auto range_layout = range_frame->layout();
      range_layout->itemAt(1)->widget()->setEnabled(is_enabled);
      range_layout->itemAt(3)->widget()->setEnabled(is_enabled);
      range_layout->itemAt(5)->widget()->setEnabled(is_enabled);
      range_layout->itemAt(6)->widget()->setEnabled(is_enabled);
    }
  }
}


std::vector<Range> DialogModelSet::getRanges(QVBoxLayout* ranges_layout) {
  std::vector<Range> new_ranges { };

  for (int i = 0; i < ranges_layout->count(); ++i) {
    QWidget *range_frame = ranges_layout->itemAt(i)->widget();
    if (range_frame != NULL) {
      auto range_layout = range_frame->layout();
      double min = static_cast<QLineEdit*>(range_layout->itemAt(1)->widget())->text().toDouble();
      double max = static_cast<QLineEdit*>(range_layout->itemAt(3)->widget())->text().toDouble();
      double step = static_cast<QLineEdit*>(range_layout->itemAt(5)->widget())->text().toDouble();
      new_ranges.push_back( { min, max, step });
    }
  }

  return new_ranges;
}

void DialogModelSet::deleteRangeAt(QVBoxLayout* ranges_layout, size_t range_id) {
 for (int i = 0; i < ranges_layout->count(); ++i) {
   QWidget *range_frame = ranges_layout->itemAt(i)->widget();
   if (range_frame != NULL) {
     auto range_layout = range_frame->layout();
     if (range_id == static_cast<QLabel*>(range_layout->itemAt(7)->widget())->text().toUInt()) {
       delete range_frame;
       break;
     }
   }
 }
}

}
}
