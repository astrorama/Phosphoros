#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "FileUtils.h"
#include <sstream>

#include <boost/filesystem.hpp>
#include "PhzUITools/CatalogColumnReader.h"

#include "PhzQtUI/DialogPOP.h"
#include "ui_DialogPOP.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogPOP::DialogPOP(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogPOP){
      ui->setupUi(this);
    }

DialogPOP::~DialogPOP(){}

void DialogPOP::setFolder(std::string output_folder){
  m_folder = output_folder;
  // get the file snd  check .fits
  auto basepath =  boost::filesystem::path(output_folder);
  if (boost::filesystem::exists(basepath/"phz_cat.fits")) {

    // get columns
    auto column_reader = PhzUITools::CatalogColumnReader((basepath/"phz_cat.fits").string());

    // fill the CBB
    for (auto& name : column_reader.getColumnNames()) {

      std::size_t found = name.find("Z-1D-PDF");
      if (found!=std::string::npos) {
        ui->cbb_columns->insertItem(0, QString::fromStdString(name));
      }
    }

    if (ui->cbb_columns->count()>0) {
         // Check the CB
          ui->cb_median->setCheckState(Qt::Checked);
          ui->cb_70_c_min->setCheckState(Qt::Checked);
          ui->cb_70_c_max->setCheckState(Qt::Checked);
          ui->cb_90_c_min->setCheckState(Qt::Checked);
          ui->cb_90_c_max->setCheckState(Qt::Checked);
          ui->cb_95_c_min->setCheckState(Qt::Checked);
          ui->cb_95_c_max->setCheckState(Qt::Checked);
          ui->cb_70_m_min->setCheckState(Qt::Checked);
          ui->cb_70_m_max->setCheckState(Qt::Checked);
          ui->cb_90_m_min->setCheckState(Qt::Checked);
          ui->cb_90_m_max->setCheckState(Qt::Checked);
          ui->cb_95_m_min->setCheckState(Qt::Checked);
          ui->cb_95_m_max->setCheckState(Qt::Checked);
          ui->cb_01_sampl->setCheckState(Qt::Checked);
          ui->cb_01_mean->setCheckState(Qt::Checked);
          ui->cb_01_fited->setCheckState(Qt::Checked);
          ui->cb_01_area->setCheckState(Qt::Checked);
          ui->cb_02_sampl->setCheckState(Qt::Checked);
          ui->cb_02_mean->setCheckState(Qt::Checked);
          ui->cb_02_fited->setCheckState(Qt::Checked);
          ui->cb_02_area->setCheckState(Qt::Checked);

          ui->lbl_warning->setText("");
          ui->btn_compute->setEnabled(true);

    } else {
         ui->lbl_warning->setText("File 'phz_cat.fits' do not contains PDZ column");
         ui->btn_compute->setEnabled(false);
    }


  } else {
    ui->lbl_warning->setText("File 'phz_cat' not found or not in .fits format");
    ui->btn_compute->setEnabled(false);
  }





}


void DialogPOP::on_btn_cancel_clicked(){
  reject();
}








void DialogPOP::on_btn_compute_clicked(){
  auto basepath =  boost::filesystem::path(m_folder);
  // input-cat    (basepath/"phz_cat.fits").string()
  // output-cat   (basepath/(ui->cbb_columns->currentText().toStdString()+"_Statistic.fits)).string()

  std::vector<std::string> excluded{};
  if (ui->cb_median->checkState()==Qt::Unchecked){
    excluded.push_back("MEDIAN");

  }
  if (ui->cb_70_c_min->checkState()==Qt::Unchecked){
    excluded.push_back("MED_CENTER_MIN_70");
  }
  if (ui->cb_70_c_max->checkState()==Qt::Unchecked){
    excluded.push_back("MED_CENTER_MAX_70");
  }
  if (ui->cb_90_c_min->checkState()==Qt::Unchecked){
    excluded.push_back("MED_CENTER_MIN_90");
  }
  if (ui->cb_90_c_max->checkState()==Qt::Unchecked){
    excluded.push_back("MED_CENTER_MAX_90");
  }
  if (ui->cb_95_c_min->checkState()==Qt::Unchecked){
    excluded.push_back("MED_CENTER_MIN_95");
  }
  if (ui->cb_95_c_max->checkState()==Qt::Unchecked){
    excluded.push_back("MED_CENTER_MAX_95");
  }

  if (ui->cb_70_m_min->checkState()==Qt::Unchecked){
    excluded.push_back("MIN_70");
  }
  if (ui->cb_70_m_max->checkState()==Qt::Unchecked){
    excluded.push_back("MAX_70");
  }
  if (ui->cb_90_m_min->checkState()==Qt::Unchecked){
    excluded.push_back("MIN_90");
  }
  if (ui->cb_90_m_max->checkState()==Qt::Unchecked){
    excluded.push_back("MAX_90");
  }
  if (ui->cb_95_m_min->checkState()==Qt::Unchecked){
    excluded.push_back("MIN_95");
  }
  if (ui->cb_95_m_max->checkState()==Qt::Unchecked){
    excluded.push_back("MAX_95");
  }

  if (ui->cb_01_sampl->checkState()==Qt::Unchecked){
     excluded.push_back("PHZ_MODE_1_SAMP");
  }
  if (ui->cb_01_mean->checkState()==Qt::Unchecked){
     excluded.push_back("PHZ_MODE_1_MEAN");
  }
  if (ui->cb_01_fited->checkState()==Qt::Unchecked){
     excluded.push_back("PHZ_MODE_1_FIT");
  }
  if (ui->cb_01_area->checkState()==Qt::Unchecked){
     excluded.push_back("PHZ_MODE_1_AREA");
  }
  if (ui->cb_02_sampl->checkState()==Qt::Unchecked){
       excluded.push_back("PHZ_MODE_2_SAMP");
  }
  if (ui->cb_02_mean->checkState()==Qt::Unchecked){
     excluded.push_back("PHZ_MODE_2_MEAN");
  }
  if (ui->cb_02_fited->checkState()==Qt::Unchecked){
     excluded.push_back("PHZ_MODE_2_FIT");
  }
  if (ui->cb_02_area->checkState()==Qt::Unchecked){
     excluded.push_back("PHZ_MODE_2_AREA");
  }
  // excluded-output-columns excluded

  // pdf-column  ui->cbb_columns->currentText()


  accept();
}



}
}
