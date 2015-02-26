#include <QStandardItem>
#include <QFileInfo>
#include "PhzQtUI/DialogPhotometricCorrectionComputation.h"
#include "ui_DialogPhotometricCorrectionComputation.h"
#include <qfiledialog.h>
#include <QMessageBox>
#include "PhzUITools/CatalogColumnReader.h"
#include "PhzQtUI/PhotometricCorrectionHandler.h"
#include "PhzQtUI/FileUtils.h"

#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzConfiguration/CalculatePhotometricCorrectionConfiguration.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"
#include "PhzPhotometricCorrection/FindBestFitModels.h"
#include "PhzPhotometricCorrection/CalculateScaleFactorMap.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionAlgorithm.h"

DialogPhotometricCorrectionComputation::DialogPhotometricCorrectionComputation(
    QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogPhotometricCorrectionComputation) {
  ui->setupUi(this);

  ui->txt_Iteration->setValidator(new QIntValidator(1, 1000, this));
  ui->txt_Tolerence->setValidator(new QDoubleValidator(0, 1, 8, this));
}

DialogPhotometricCorrectionComputation::~DialogPhotometricCorrectionComputation() {
  delete ui;
}

void DialogPhotometricCorrectionComputation::setData(std::string survey,
    std::string id_column, std::string model, std::string grid,
    std::list<Euclid::PhosphorosUiDm::FilterMapping> selected_filters) {
  m_id_column = id_column;
  m_selected_filters = selected_filters;
  ui->txt_survey->setText(QString::fromStdString(survey));
  ui->txt_Model->setText(QString::fromStdString(model));
  ui->txt_grid->setText(QString::fromStdString(grid));

  QStandardItemModel* grid_model = new QStandardItemModel();
  grid_model->setColumnCount(1);
  for (auto filter : m_selected_filters) {
    QList<QStandardItem*> items { { new QStandardItem(
        QString::fromStdString(filter.getName())) } };
    grid_model->appendRow(items);
  }

  ui->listView_Filter->setModel(grid_model);
}

void DialogPhotometricCorrectionComputation::on_btn_TrainingCatalog_clicked() {
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()) {

    QString file_name = dialog.selectedFiles()[0];

    // Validate catalog
    auto column_reader = Euclid::PhzUITools::CatalogColumnReader(
        file_name.toStdString());
    std::map<std::string, bool> file_columns;

    for (auto& name : column_reader.getColumnNames()) {
      file_columns[name] = true;
    }

    bool not_found = false;

    if (file_columns.count(m_id_column) == 1) {
      file_columns[m_id_column] = false;
    } else {
      not_found = true;
    }

    for (auto& filter : m_selected_filters) {
      if (file_columns.count(filter.getFluxColumn()) == 1) {
        file_columns[filter.getFluxColumn()] = false;

      } else {
        not_found = true;
      }

      if (file_columns.count(filter.getErrorColumn()) == 1) {
        file_columns[filter.getErrorColumn()] = false;

      } else {
        not_found = true;
      }
    }

    if (not_found) {
      QMessageBox::warning(this, "Incompatible Data...",
          "The catalog file you selected has not the columns described into the Survey and therefore cannot be used. Please select anothe catalog file.",
          QMessageBox::Ok);
    } else {
      ui->txt_catalog->setText(file_name);
      ui->cb_SpectroColumn->clear();
      for (auto& column_pair : file_columns) {
        if (column_pair.second) {
          ui->cb_SpectroColumn->addItem(
              QString::fromStdString(column_pair.first));
        }
      }
    }
  }
}

void DialogPhotometricCorrectionComputation::setRunEnability() {
  bool run_ok = true;

  // A column name is selected
  run_ok &= ui->cb_SpectroColumn->currentText().length() > 0;

  // An itereation number is set
  run_ok &= ui->txt_Iteration->text().length() > 0;

  // A tolerance is set
  run_ok &= ui->txt_Tolerence->text().length() > 0;

  // A output name is set
  run_ok &= ui->txt_FileName->text().length() > 0;

  ui->bt_Run->setEnabled(run_ok);
}

void DialogPhotometricCorrectionComputation::on_cb_SpectroColumn_currentIndexChanged(
    const QString &) {
  setRunEnability();
}

void DialogPhotometricCorrectionComputation::on_txt_Iteration_textChanged(
    const QString &) {
  setRunEnability();
}

void DialogPhotometricCorrectionComputation::on_txt_Tolerence_textChanged(
    const QString &) {
  setRunEnability();
}

void DialogPhotometricCorrectionComputation::on_btn_OutputFileName_clicked() {
  setRunEnability();
}

void DialogPhotometricCorrectionComputation::on_txt_FileName_textChanged(
    const QString &) {
  setRunEnability();
}

void DialogPhotometricCorrectionComputation::on_bt_Run_clicked() {
  std::string output_file_name=Euclid::PhosphorosUiDm::FileUtils::addExt(ui->txt_FileName->text().toStdString(),".txt");
  ui->txt_FileName->setText(QString::fromStdString(output_file_name));

  std::vector<std::string> filter_mapping;
  for (auto& filter : m_selected_filters){
    filter_mapping.push_back(filter.getFilterFile()+" "+filter.getFluxColumn()+" "+filter.getErrorColumn());
  }

  int max_iter_number=ui->txt_Iteration->text().toInt();

  auto config_map =Euclid::PhosphorosUiDm::PhotometricCorrectionHandler::GetConfigurationMap(
      output_file_name,
      max_iter_number,
      ui->txt_Tolerence->text().toDouble(),
      ui->cb_SelectionMethod->currentText().toStdString(),
      ui->txt_grid->text().toStdString(),
      ui->txt_catalog->text().toStdString(),
      m_id_column,
      ui->cb_SpectroColumn->currentText().toStdString(),
      filter_mapping
    );


  auto path_filename = Euclid::PhosphorosUiDm::FileUtils::getPhotCorrectionsRootPath(true)
      + QString(QDir::separator()).toStdString() + output_file_name;

  if (QFileInfo(QString::fromStdString(path_filename)).exists() && QMessageBox::question(this, "Override existing file...",
            "A File with the very same name as the one you provided already exist. Do you want to replace it?",
            QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
              return;
  }

  ui->btn_TrainingCatalog->setEnabled(false);
  ui->cb_SpectroColumn->setEnabled(false);
  ui->txt_Iteration->setEnabled(false);
  ui->txt_Tolerence->setEnabled(false);
  ui->cb_SelectionMethod->setEnabled(false);
  ui->txt_FileName->setEnabled(false);
  ui->bt_Run->setEnabled(false);
  ui->buttonBox->setEnabled(false);

  Euclid::PhzConfiguration::CalculatePhotometricCorrectionConfiguration conf {config_map};
     auto catalog = conf.getCatalog();
     auto model_phot_grid = conf.getPhotometryGrid();
     auto output_func = conf.getOutputFunction();
     auto stop_criteria = conf.getStopCriteria();

     Euclid::PhzPhotometricCorrection::FindBestFitModels<Euclid::PhzLikelihood::SourcePhzFunctor> find_best_fit_models {};
     Euclid::PhzPhotometricCorrection::CalculateScaleFactorMap calculate_scale_factor_map {};
     Euclid::PhzPhotometricCorrection::PhotometricCorrectionAlgorithm phot_corr_algorighm {};
     auto selector = conf.getPhotometricCorrectionSelector();

     Euclid::PhzPhotometricCorrection::PhotometricCorrectionCalculator calculator {find_best_fit_models,
                                 calculate_scale_factor_map, phot_corr_algorighm};



     auto progress_logger = [this,max_iter_number](size_t iter_no, const Euclid::PhzDataModel::PhotometricCorrectionMap& ) {
         int value = (iter_no*100.)/max_iter_number;
         ui->progressBar->setValue(value);
     };
     auto phot_corr_map = calculator(catalog, model_phot_grid, stop_criteria, selector, progress_logger);
     output_func(phot_corr_map);


     correctionComputed(output_file_name);
     accept();



}
