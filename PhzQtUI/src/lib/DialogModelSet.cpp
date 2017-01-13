
#include <QMessageBox>
#include <QStandardItemModel>
#include "ElementsKernel/Exception.h"
#include "FileUtils.h"
#include "FormUtils.h"
#include "PhzQtUI/DialogModelSet.h"
#include "ui_DialogModelSet.h"
#include "PhzQtUI/DataSetTreeModel.h"

#include "PhzQtUI/GridButton.h"
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


void DialogModelSet::loadData(int ref ,const std::map<int,ParameterRule>& init_parameter_rules){
  m_rules=init_parameter_rules;
  m_ref=ref;

  auto selected_rule = m_rules[m_ref];

          ui->txt_name->setText(QString::fromStdString(selected_rule.getName()));

          // SED
          static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->setState(selected_rule.getSedSelection());
          // Reddening Curve
          static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setState(selected_rule.getRedCurveSelection());


          // Redshift & E(B-V)
          populateZRangesAndValues(selected_rule);
          populateEbvRangesAndValues(selected_rule);


  turnControlsInEdition();
}



void DialogModelSet::turnControlsInEdition(){
    ui->buttonBox->setEnabled(true);

    static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->setEnabled(true);
    static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setEnabled(true);

    ui->txt_name->setEnabled(true);

  // Redshift & E(B-V)
    ui->txt_ebv_values->setEnabled(true);
    ui->btn_add_z_range->setEnabled(true);
    SetRangeControlsEnabled(ui->Layout_ebv_range,true);

    ui->txt_z_values->setEnabled(true);
    ui->btn_add_ebv_range->setEnabled(true);
    SetRangeControlsEnabled(ui->Layout_z_range,true);
}

void DialogModelSet::turnControlsInView(){

    ui->buttonBox->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Save)->setEnabled(false);

    static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->setEnabled(false);
    static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->setEnabled(false);

    ui->txt_name->setEnabled(false);

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
  this->popupClosing(m_ref,m_rules[m_ref],false);
}


void DialogModelSet::on_buttonBox_accepted()
{

  auto sed_selection = static_cast<DataSetTreeModel*>(ui->treeView_Sed->model())->getState();
  auto red_curve_selection = static_cast<DataSetTreeModel*>(ui->treeView_Reddening->model())->getState();

  if (ui->txt_name->text().trimmed().size()==0){
    QMessageBox::warning( this,
                             "Missing Data...",
                             "Please provide a name.",
                             QMessageBox::Ok );
       return;
  }
  for(auto it = m_rules.begin(); it != m_rules.end(); ++it ) {
    if (it->second.getName()==ui->txt_name->text().toStdString() &&  it->first!=m_ref){
      QMessageBox::warning( this,
                               "Duplicate Name...",
                               "The name you enter is already used, please provide another name.",
                               QMessageBox::Ok );
         return;
    }
  }


  if (sed_selection.isEmpty() ){
    QMessageBox::warning( this,
                          "Missing Data...",
                          "Please provide SED(s) selection.",
                          QMessageBox::Ok );
    return;
  }

  if (red_curve_selection.isEmpty()){
    QMessageBox::warning( this,
                          "Missing Data...",
                          "Please provide Reddening Curve(s) selection.",
                          QMessageBox::Ok );
    return;
  }



    // Redshift and E(B-V)
    auto new_z_ranges = getRanges( ui->Layout_z_range);
    auto new_ebv_ranges = getRanges( ui->Layout_ebv_range);



    auto old_z_ranges =m_rules[m_ref].getZRanges();
    auto old_ebv_ranges =m_rules[m_ref].getEbvRanges();

    try {
      m_rules[m_ref].setZRanges(std::move(new_z_ranges));
      m_rules[m_ref].getModelNumber(true);
      m_rules[m_ref].getRedshiftRangeString();
    } catch (const Elements::Exception& e) {
        QMessageBox::warning( this, "Error while setting redshift ranges...",
                                  e.what(),
                                  QMessageBox::Ok );

        m_rules[m_ref].setZRanges(std::move(old_z_ranges));
        return;
    }

    try {
      m_rules[m_ref].setEbvRanges(std::move(new_ebv_ranges));
      m_rules[m_ref].getModelNumber(true);
      m_rules[m_ref].getEbvRangeString();
      } catch (const Elements::Exception& e) {
          QMessageBox::warning( this, "Error while setting E(B-V) ranges...",
                                    e.what(),
                                    QMessageBox::Ok );

          m_rules[m_ref].setEbvRanges(std::move(old_ebv_ranges));
          return;
      }


      m_rules[m_ref].setName(ui->txt_name->text().toStdString());


    // SED
      m_rules[m_ref].setSedSelection(std::move(sed_selection));

    // Reddeing Curves
      m_rules[m_ref].setRedCurveSelection(std::move(red_curve_selection));



    auto new_z_values = ParameterRule::parseValueList(ui->txt_z_values->text().toStdString());
    m_rules[m_ref].setRedshiftValues(std::move(new_z_values));



    auto new_ebv_values = ParameterRule::parseValueList(ui->txt_ebv_values->text().toStdString());
    m_rules[m_ref].setEbvValues(std::move(new_ebv_values));


    m_rules[m_ref].getModelNumber(true);




    this->popupClosing(m_ref,m_rules[m_ref],true);
    this->close();
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
   return createRangeControls(del_button, range_id, enabled,{-1,-1,-1});
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
     size_t value = static_cast<QLabel*>(range_layout->itemAt(7)->widget())->text().toUInt();
     if (range_id == value) {
       delete range_frame;
       break;
     }
   }
 }
}

}
}
