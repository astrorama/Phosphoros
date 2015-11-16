#include <chrono>
#include <QFuture>
#include <qtconcurrentrun.h>
#include <QStandardItem>
#include <QFileInfo>
#include "PhzQtUI/DialogPhotometricCorrectionComputation.h"
#include "ui_DialogPhotometricCorrectionComputation.h"
#include <qfiledialog.h>
#include <QMessageBox>
#include "PhzUITools/CatalogColumnReader.h"
#include "PhzQtUI/PhotometricCorrectionHandler.h"
#include "FileUtils.h"
#include "ElementsKernel/Exception.h"
#include "Configuration/ConfigManager.h"
#include "Configuration/CatalogConfig.h"

#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzConfiguration/ComputePhotometricCorrectionsConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"
#include "PhzPhotometricCorrection/FindBestFitModels.h"
#include "PhzPhotometricCorrection/CalculateScaleFactorMap.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionAlgorithm.h"

using namespace std;
using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzQtUI {

DialogPhotometricCorrectionComputation::DialogPhotometricCorrectionComputation(
    QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogPhotometricCorrectionComputation) {
  ui->setupUi(this);

  ui->txt_Iteration->setValidator(new QIntValidator(1, 1000, this));
  ui->txt_Tolerence->setValidator(new QDoubleValidator(0, 1, 8, this));
}

DialogPhotometricCorrectionComputation::~DialogPhotometricCorrectionComputation() {
}

void DialogPhotometricCorrectionComputation::setData(string survey,
    string id_column, string model, string grid,
    std::list<FilterMapping> selected_filters,
    std::list<std::string> excluded_filters,
    std::string default_catalog_path,
    double non_detection) {
  m_id_column = id_column;
  m_selected_filters = selected_filters;
  m_excluded_filters = excluded_filters;
  m_non_detection=non_detection;
  ui->txt_survey->setText(QString::fromStdString(survey));
  ui->txt_Model->setText(QString::fromStdString(model));
  ui->txt_grid->setText(QString::fromStdString(grid));

  QStandardItemModel* grid_model = new QStandardItemModel();
  grid_model->setColumnCount(1);

  for (auto filter : m_selected_filters) {
     QList<QStandardItem*> items;
     items.push_back(new QStandardItem(QString::fromStdString(filter.getFilterFile())));
     grid_model->appendRow(items);
   }

  ui->listView_Filter->setModel(grid_model);

  // default catalog

 if (loadTestCatalog(QString::fromStdString(default_catalog_path),false)){
   ui->txt_catalog->setText(QString::fromStdString(default_catalog_path));
 }
  //default correction name
  string default_file_name = survey+"_"+model
      +"_"+ui->cb_SelectionMethod->currentText().toStdString();
  ui->txt_FileName->setText(QString::fromStdString(default_file_name));

}


bool DialogPhotometricCorrectionComputation::loadTestCatalog(QString file_name, bool with_warning){
  // Validate catalog
    auto column_reader = PhzUITools::CatalogColumnReader(
        file_name.toStdString());
    map<string, bool> file_columns;

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
      if (with_warning){
      QMessageBox::warning(this, "Incompatible Data...",
          "The catalog file you selected has not the columns described into the Catalog and therefore cannot be used. Please select another catalog file.",
          QMessageBox::Ok);
      }
      return false;
    } else {
      ui->txt_catalog->setText(file_name);
      ui->cb_SpectroColumn->clear();
      for (auto& column_pair : file_columns) {
        if (column_pair.second) {
          ui->cb_SpectroColumn->addItem(
              QString::fromStdString(column_pair.first));
        }
      }

      return true;
    }
}

void DialogPhotometricCorrectionComputation::on_btn_TrainingCatalog_clicked() {
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);

  std::string path = ui->txt_catalog->text().toStdString();
  if (path.length()==0){
    path=FileUtils::getLastUsedPath();
  }
  dialog.selectFile(QString::fromStdString(path));
  if (dialog.exec()) {

    QString file_name = dialog.selectedFiles()[0];
    FileUtils::setLastUsedPath(file_name.toStdString());
    loadTestCatalog(file_name,true);

  }
}

void DialogPhotometricCorrectionComputation::setRunEnability() {
  bool run_ok = true;

  // A column name is selected
  run_ok &= ui->cb_SpectroColumn->currentText().length() > 0;

  // An iteration number is set
  run_ok &= ui->txt_Iteration->text().length() > 0;

  // A tolerance is set
  run_ok &= ui->txt_Tolerence->text().length() > 0;

  // A output name is set
  run_ok &= ui->txt_FileName->text().length() > 0;

  ui->bt_Run->setEnabled(run_ok);
}

void DialogPhotometricCorrectionComputation::on_cb_SelectionMethod_currentIndexChanged(const QString & method){
  QString default_file_name = ui->txt_Model->text()+"_"+method;
  ui->txt_FileName->setText(default_file_name);
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


void DialogPhotometricCorrectionComputation::on_txt_FileName_textChanged(
    const QString &) {
  setRunEnability();
  string output_file_name=FileUtils::addExt(ui->txt_FileName->text().toStdString(),".txt");
    auto path_filename = FileUtils::getPhotCorrectionsRootPath(true,ui->txt_survey->text().toStdString())
           + QString(QDir::separator()).toStdString() + output_file_name;

    if (QFileInfo(QString::fromStdString(path_filename)).exists()){
       ui->txt_FileName->setStyleSheet("QLineEdit { color: orange }");
    } else {
      ui->txt_FileName->setStyleSheet("QLineEdit { color: black }");
    }
}

void DialogPhotometricCorrectionComputation::disablePage(){
     ui->btn_TrainingCatalog->setEnabled(false);
     ui->cb_SpectroColumn->setEnabled(false);
     ui->txt_Iteration->setEnabled(false);
     ui->txt_Tolerence->setEnabled(false);
     ui->cb_SelectionMethod->setEnabled(false);
     ui->txt_FileName->setEnabled(false);
     ui->bt_Run->setEnabled(false);
     ui->buttonBox->setEnabled(false);
}
std::string DialogPhotometricCorrectionComputation::runFunction(){
  try {

    int max_iter_number = ui->txt_Iteration->text().toInt();

    auto config_map = PhotometricCorrectionHandler::GetConfigurationMap(
        ui->txt_survey->text().toStdString(),
        ui->txt_FileName->text().toStdString(), max_iter_number,
        ui->txt_Tolerence->text().toDouble(),
        m_non_detection,
        ui->cb_SelectionMethod->currentText().toStdString(),
        ui->txt_grid->text().toStdString(),
        ui->txt_catalog->text().toStdString(), m_id_column,
        ui->cb_SpectroColumn->currentText().toStdString(),
        m_excluded_filters);

    long config_manager_id = std::chrono::duration_cast<std::chrono::microseconds>(
                                    std::chrono::system_clock::now().time_since_epoch()).count();
    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputePhotometricCorrectionsConfig>();
    config_manager.closeRegistration();
    config_manager.initialize(config_map);
    
    auto& catalog = config_manager.getConfiguration<Configuration::CatalogConfig>().getCatalog();
    auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid();
    auto& output_func = config_manager.getConfiguration<ComputePhotometricCorrectionsConfig>().getOutputFunction();
    auto& stop_criteria = config_manager.getConfiguration<ComputePhotometricCorrectionsConfig>().getStopCriteria();

    PhzPhotometricCorrection::FindBestFitModels<PhzLikelihood::SourcePhzFunctor> find_best_fit_models { };
    PhzPhotometricCorrection::CalculateScaleFactorMap calculate_scale_factor_map { };
    PhzPhotometricCorrection::PhotometricCorrectionAlgorithm phot_corr_algorighm { };
    auto selector = config_manager.getConfiguration<ComputePhotometricCorrectionsConfig>().getPhotometricCorrectionSelector();

    PhzPhotometricCorrection::PhotometricCorrectionCalculator calculator {
        find_best_fit_models, calculate_scale_factor_map, phot_corr_algorighm };

    auto progress_logger =
        [this,max_iter_number](size_t iter_no, const PhzDataModel::PhotometricCorrectionMap& ) {
          int value = (iter_no*100.)/max_iter_number;
          ui->progressBar->setValue(value);
        };
    auto phot_corr_map = calculator(catalog, model_phot_grid, stop_criteria,
        selector, progress_logger);
    output_func(phot_corr_map);
    correctionComputed (ui->txt_FileName->text());
    return "";
  }
  catch (const Elements::Exception & e) {
    return "Sorry, an error occurred during the computation: "
        + std::string(e.what());
  }
  catch (const std::exception& e) {
    return "Sorry, an error occurred during the computation: "
        + std::string(e.what());
  }
  catch (...) {
    return "Sorry, an error occurred during the computation.";
  }
}

void DialogPhotometricCorrectionComputation::on_bt_Run_clicked() {
  string output_file_name = FileUtils::addExt(
      ui->txt_FileName->text().toStdString(), ".txt");
  ui->txt_FileName->setText(QString::fromStdString(output_file_name));
  auto path_filename = FileUtils::getPhotCorrectionsRootPath(true,ui->txt_survey->text().toStdString())
      + QString(QDir::separator()).toStdString() + output_file_name;

  if (QFileInfo(QString::fromStdString(path_filename)).exists()
      && QMessageBox::question(this, "Override existing file...",
          "A File with the very same name as the one you provided already exist. Do you want to replace it?",
          QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
    return;
  }

  disablePage();

  QFuture<std::string> future = QtConcurrent::run(this,
      &DialogPhotometricCorrectionComputation::runFunction);
  std::string message = future.result();
  if (message.length() == 0) {
    this->accept();
    return;
  } else {
    QMessageBox::warning(this, "Error in the computation...",
        QString::fromStdString(message), QMessageBox::Close);
    this->reject();
  }
}

}
}
