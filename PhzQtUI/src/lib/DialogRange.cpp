
#include <QMessageBox>
#include <QProcess>
#include <QStandardItemModel>
#include <QDir>
#include <QDoubleValidator>
#include "ElementsKernel/Exception.h"
#include "FileUtils.h"
#include "FormUtils.h"
#include "PhzQtUI/DialogRange.h"
#include "ui_DialogRange.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "PhzQtUI/SedTreeModel.h"

#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/MessageButton.h"
#include "PhzQtUI/ParameterRule.h"

#include "XYDataset/AsciiParser.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

#include <QProgressDialog>


namespace Euclid{
namespace PhzQtUI {


DialogRange::DialogRange(QWidget *parent) : QDialog(parent) , ui(new Ui::DialogRange) {
    ui->setupUi(this);
}


void DialogRange::setData(std::vector<Range> ranges, std::set<double> values, bool redshift) {
   m_ranges = ranges;
   m_values = values;
   m_is_redshift = redshift;

   if (ranges.size() > 0) {
     ui->rb_range->setChecked(true);
     on_rb_range_clicked();
   }

   populateRangesAndValues();
   if (redshift) {
     ui->lbl_title->setText("Redshift Range");
   } else {
     ui->lbl_title->setText("E(B-V) Range");
   }
}


DialogRange::~DialogRange() {

}


void DialogRange::populateRangesAndValues() {
    cleanRangeControl( ui->Layout_range);
    m_current_range_id = 0;
    for (auto& range : m_ranges) {
      auto del_button = new GridButton(0, m_current_range_id, "Delete");
      connect(del_button, SIGNAL(GridButtonClicked(size_t,size_t)), this,
      SLOT(onDeleteClicked(size_t,size_t)));
      ui->Layout_range->addWidget(createRangeControls(del_button, m_current_range_id, false, range));
      ++m_current_range_id;
    }

    QString list = "";
    bool first = true;
    for (auto& val : m_values) {
      if (!first) {
        list += ",";
      } else {
        first = false;
      }
      list += QString::number(val);
    }

    ui->txt_values->setText(list);
}

void DialogRange::onDeleteClicked(size_t,size_t ref){
  deleteRangeAt(ui->Layout_range, ref);
}


void DialogRange::cleanRangeControl(QVBoxLayout* ranges_layout){
  for (int i = ranges_layout->count() - 1; i >= 0; --i) {
      QWidget *widget = ranges_layout->itemAt(i)->widget();
      if (widget != NULL) {
        delete widget;
      }
   }
}

void DialogRange::deleteRangeAt(QVBoxLayout* ranges_layout, size_t range_id) {

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


void DialogRange::on_btn_add_range_clicked() {
  auto del_button = new GridButton(0, m_current_range_id, "Delete");
  connect(del_button, SIGNAL(GridButtonClicked(size_t,size_t)), this,
      SLOT(onDeleteClicked(size_t,size_t)));
  ui->Layout_range->addWidget(createRangeControls(del_button, m_current_range_id, true));
  ++m_current_range_id;
}


QFrame* DialogRange::createRangeControls(GridButton* del_button, int range_id, bool enabled ){
   return createRangeControls(del_button, range_id, enabled,{-1,-1,-1});
 }

QFrame* DialogRange::createRangeControls(GridButton* del_button, int range_id, bool enabled, const Range& range){
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


void DialogRange::SetRangeControlsEnabled(QVBoxLayout* ranges_layout, bool is_enabled) {
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

void DialogRange::on_buttonBox_rejected() {
  this->close();
}


std::vector<Range> DialogRange::getRanges(QVBoxLayout* ranges_layout) {
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


void DialogRange::on_rb_val_clicked() {
  ui->rb_range->setChecked(!ui->rb_val->isChecked());
  ui->txt_values->setEnabled( ui->rb_val->isChecked());
  ui->btn_add_range->setEnabled(!ui->rb_val->isChecked());
  SetRangeControlsEnabled(ui->Layout_range, !ui->rb_val->isChecked());
  if (ui->rb_val->isChecked()) {
     for (int i = ui->Layout_range->count()-1; i >=0; --i) {
       QWidget *range_frame =  ui->Layout_range->itemAt(i)->widget();
       if (range_frame != NULL) {
         delete range_frame;
       }
     }
     cleanRangeControl( ui->Layout_range);
  }
}

void DialogRange::on_rb_range_clicked() {
  ui->rb_val->setChecked(!ui->rb_range->isChecked());
  ui->txt_values->setEnabled(ui->rb_val->isChecked());
  ui->btn_add_range->setEnabled(!ui->rb_val->isChecked());
  SetRangeControlsEnabled(ui->Layout_range, !ui->rb_val->isChecked());
  if (ui->rb_range->isChecked()) {
     ui->txt_values->setText("");
  }
}



void DialogRange::on_buttonBox_accepted() {

    auto new_ranges = getRanges( ui->Layout_range);
    auto new_values = ParameterRule::parseValueList(ui->txt_values->text().toStdString());

    if (ui->rb_val->isChecked()) {
      new_ranges = {};
    } else {
      new_values = {};
    }

    this->popupClosing(new_ranges, new_values);
    this->close();
}


}
}
