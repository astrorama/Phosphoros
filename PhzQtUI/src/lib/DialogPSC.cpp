#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QThread>
#include <QStringList>
#include <QScrollBar>
#include <QMessageBox>
#include "FileUtils.h"
#include <sstream>
#include "ElementsKernel/Logging.h"

#include <boost/filesystem.hpp>
#include "PhzUITools/CatalogColumnReader.h"

#include "PhzQtUI/DialogPSC.h"
#include "ui_DialogPSC.h"
#include <QApplication>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("DialogPSC");

DialogPSC::DialogPSC(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogPSC) {
      ui->setupUi(this);
    }

DialogPSC::~DialogPSC(){}


void DialogPSC::setDefaultColumn(std::string id_column, std::string zref_column){
  m_id_column = id_column;
  m_z_ref_column = zref_column;
}

void DialogPSC::setFolder(std::string output_folder) {
  m_folder = output_folder;
  ui->out_cons->hide();
  ui->btn_close->show();
  ui->btn_compute->show();
  ui->btn_cancel->hide();
  ui->gb_ref_cat->setDisabled(true);
  ui->gb_option->setDisabled(true);
  ui->le_path->setReadOnly(true);
  // get the file snd  check .fits
  auto basepath =  boost::filesystem::path(output_folder);
  m_catalog = basepath.parent_path().filename().string();
  if (boost::filesystem::exists(basepath/"phz_cat.fits")) {
    // get columns
    auto column_reader = PhzUITools::CatalogColumnReader((basepath/"phz_cat.fits").string());

    // fill the CBB
    for (auto& name : column_reader.getColumnNames()) {
      if (name.find("Z-1D-PDF") == std::string::npos && name.find("Z") != std::string::npos) {
        ui->cbb_z_col->insertItem(-1, QString::fromStdString(name));
      }

      if (name.find("Z-1D-PDF") != std::string::npos) {
         ui->cbb_pdf_col->insertItem(-1, QString::fromStdString(name));
      }
    }

    ui->cbb_z_col->setCurrentIndex(0);
    ui->cbb_pdf_col->setCurrentIndex(0);
    checkComputePossible();

    // check if the config file exist
    if (boost::filesystem::exists(basepath/"run_config.config")) {
        // if so get the input file
        QString input_line = "";
        QFile inputFile(QString::fromStdString((basepath/"run_config.config").generic_string()));
        if (inputFile.open(QIODevice::ReadOnly)) {
           QTextStream in(&inputFile);
           while (!in.atEnd()) {
              QString line = in.readLine();
              if (line.startsWith("input-catalog-file")) {
                input_line = line;
                break;
              }
           }
           inputFile.close();
        }

        if (input_line != "") {
           auto fragments = input_line.split("=");
           QString inputfileName = fragments[1];
           if (!inputfileName.startsWith("/")) {
             inputfileName = QString::fromStdString(FileUtils::getCatalogRootPath(false, m_catalog)) + "/" + inputfileName;
           }

           setCatalogFile(inputfileName);
        }


    }



  } else {
    ui->lbl_warning->setText("File 'phz_cat' not found or not in .fits format");
    ui->btn_compute->setEnabled(false);
  }
}


void DialogPSC::setCatalogFile(QString path){
  ui->le_path->setText(path);
       auto column_reader = PhzUITools::CatalogColumnReader(path.toStdString());

       ui->cdd_ref_id_col->clear();
       ui->cbb_ref_z_col->clear();
       size_t index_id = 0;
       size_t index_z = 0;
       size_t current = 0;
       for (auto& name : column_reader.getColumnNames()) {
         ui->cdd_ref_id_col->insertItem(10000, QString::fromStdString(name));
         ui->cbb_ref_z_col->insertItem(10000, QString::fromStdString(name));

         if (name.compare(m_z_ref_column) == 0) {
           index_z = current;
         }

         if (name.compare(m_id_column) == 0) {
           index_id = current;
         }

         ++current;
       }

       if (current > 0) {
         ui->cdd_ref_id_col->setCurrentIndex(index_id);
         ui->cbb_ref_z_col->setCurrentIndex(index_z);
       }

       checkComputePossible();
}

void DialogPSC::on_btn_browse_clicked() {
   QFileDialog dialog(this);
   std::string path = ui->le_path->text().toStdString();
   if (path == "") {
     path = FileUtils::getCatalogRootPath(false, m_catalog)+"/.";
   }


   dialog.selectFile(QString::fromStdString(path));
   dialog.setFileMode(QFileDialog::ExistingFile);
   if (dialog.exec()) {
     setCatalogFile(dialog.selectedFiles()[0]);
   }
 }



void DialogPSC::on_btn_cancel_clicked(){
  if (m_processing) {
     m_P->kill();
     logger.info()<< "Processing stop by the user";
   }
  ui->btn_cancel->hide();
  ui->btn_close->show();
}

void DialogPSC::on_gb_scater_clicked(bool checked){
  ui->gb_stacked->setChecked(ui->cbb_pdf_col->count()>0 && !checked);
  ui->gb_scater->setChecked(ui->cbb_pdf_col->count()==0 || checked);

}

void DialogPSC::on_gb_stacked_clicked(bool checked){
  ui->gb_scater->setChecked(!checked);
}


void DialogPSC::on_btn_compute_clicked(){
  if (m_processing) {
        m_P->kill();
        logger.info()<< "Processing stop by the user";
  }

  ui->btn_cancel->show();
  ui->btn_close->hide();
  ui->out_cons->show();
  ui->out_cons->setReadOnly(true);
  m_processing = true;
  qApp->processEvents();



  m_P =new QProcess;

  connect(m_P, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processingFinished(int, QProcess::ExitStatus)));

  QString cmd = "";
  if (ui->gb_scater->isChecked()) {
      QStringList s3;
      s3 << QString::fromStdString("--phosphoros-output-dir") <<  QString::fromStdString(m_folder)
         << QString::fromStdString("--phz-column") << ui->cbb_z_col->currentText()
         << QString::fromStdString("--specz-catalog") << ui->le_path->text()
         << QString::fromStdString("--specz-cat-id") << ui->cdd_ref_id_col->currentText()
         << QString::fromStdString("--specz-column") << ui->cbb_ref_z_col->currentText()
         << QString::fromStdString("--samp");

      if (ui->cb_no_plot->checkState() != Qt::Unchecked) {
        s3 << QString::fromStdString("--no-display");
      }

      cmd =  QString("Phosphoros PSC ")+s3.join(" ");

  } else {
      QStringList s3;
         s3 << QString::fromStdString("--pdz-catalog-file") <<  QString::fromStdString(m_folder + "/phz_cat.fits")
            << QString::fromStdString("--refz-catalog-file") << ui->le_path->text()
            << QString::fromStdString("--pdz-col-pe") << ui->cbb_z_col->currentText()
            << QString::fromStdString("--pdz-col-pdf") << ui->cbb_pdf_col->currentText()

            << QString::fromStdString("--refz-col-id") << ui->cdd_ref_id_col->currentText()
            << QString::fromStdString("--refz-col-ref") << ui->cbb_ref_z_col->currentText()
            << QString::fromStdString("--stack-bins") << QString::number(ui->sb_st_bins->value())
            << QString::fromStdString("--hist-bins") << QString::number(ui->sb_hist_bin->value())
            << QString::fromStdString("--stacked-point-estimate") <<ui->cb_pe_type->currentText();


         if (ui->cb_st_ref->checkState() != Qt::Checked) {
           s3 << QString::fromStdString("--ref-plot") << QString::fromStdString("False");
         }
         if (ui->cb_ref_nb_plot->checkState() != Qt::Checked) {
           s3 << QString::fromStdString("--ref-bin-plot") << QString::fromStdString("False");
         }
         if (ui->cb_ref_bias_plot->checkState() != Qt::Checked) {
           s3 << QString::fromStdString("--ref-bias-plot") << QString::fromStdString("False");
         }
         if (ui->cb_ref_frac_plot->checkState() != Qt::Checked) {
           s3 << QString::fromStdString("--ref-frac-plot") << QString::fromStdString("False");
         }


         if (ui->cb_st_shift->checkState() != Qt::Checked) {
           s3 << QString::fromStdString("--shift-plot")<< QString::fromStdString("False");
         }
         if (ui->cb_shift_nb_plot->checkState() != Qt::Checked) {
           s3 << QString::fromStdString("--shift-bin-plot")<< QString::fromStdString("False");
         }
         if (ui->cb_bias_nb_plot->checkState() != Qt::Checked) {
           s3 << QString::fromStdString("--shift-bias-plot")<< QString::fromStdString("False");
         }
         if (ui->cb_frac_nb_plot->checkState() != Qt::Checked) {
           s3 << QString::fromStdString("--shift-frac-plot")<< QString::fromStdString("False");
         }


         if (ui->cb_pit->checkState() != Qt::Checked) {
                s3 << QString::fromStdString("--pit-plot")<< QString::fromStdString("False");
         }

         if (ui->cb_crps->checkState() != Qt::Checked) {
                s3 << QString::fromStdString("--crps-plot")<< QString::fromStdString("False");
         }


         cmd =  QString("Phosphoros PSP ")+s3.join(" ");
  }
  logger.info()<< "Processing cmd:" << cmd.toStdString();

  m_P->setReadChannelMode(QProcess::MergedChannels);
  m_P->start(cmd);

  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(updateOutCons()));
  m_timer->start(100);

}


void DialogPSC::checkComputePossible() {
  ui->cbb_z_col->setEnabled(false);
  if (ui->cbb_z_col->count() > 0) {
        ui->cbb_z_col->setEnabled(true);



        ui->gb_ref_cat->setDisabled(false);
        ui->gb_option->setDisabled(false);

        if (ui->le_path->text().toStdString() != "") {
          ui->lbl_warning->setText("");
          ui->btn_compute->setEnabled(true);
        } else {
          ui->lbl_warning->setText("Please Select a reference catalog");
          ui->btn_compute->setEnabled(false);
        }

  } else {
       ui->lbl_warning->setText("File 'phz_cat.fits' contains no redshift column");
       ui->btn_compute->setEnabled(false);
  }

  if (ui->cbb_pdf_col->count()==0) {
    ui->gb_stacked->setEnabled(false);
  }
}

void DialogPSC::on_btn_close_clicked() {
  accept();
}


void DialogPSC::processingFinished(int, QProcess::ExitStatus) {
   m_timer->stop();
   m_processing = false;
   updateOutCons();
   ui->btn_cancel->hide();
   ui->btn_close->show();
   logger.info() << ui->out_cons->toPlainText().toStdString();
}


void DialogPSC::updateOutCons() {
  QString result_all = m_P->readAllStandardOutput();
  ui->out_cons->setPlainText(ui->out_cons->toPlainText() + result_all);
  ui->out_cons->verticalScrollBar()->setValue(ui->out_cons->verticalScrollBar()->maximum());
}


}
}
