#include "PhzQtUI/DialogPhotometricCorrectionComputation.h"
#include "FileUtils.h"
#include "PhzQtUI/PhotometricCorrectionHandler.h"
#include "PhzUITools/CatalogColumnReader.h"
#include "ui_DialogPhotometricCorrectionComputation.h"
#include <QFileInfo>
#include <QFuture>
#include <QMessageBox>
#include <QStandardItem>
#include <QtCore/qfuturewatcher.h>
#include <chrono>
#include <qfiledialog.h>
#include <qtconcurrentrun.h>

#include "Configuration/ConfigManager.h"
#include "ElementsKernel/Exception.h"
#include "FormUtils.h"

#include "Configuration/CatalogConfig.h"
#include "Configuration/Utils.h"
#include "DefaultOptionsCompleter.h"
#include "PhzConfiguration/ComputeModelGridConfig.h"
#include "PhzConfiguration/ComputePhotometricCorrectionsConfig.h"
#include "PhzConfiguration/ComputeRedshiftsConfig.h"
#include "PhzConfiguration/ComputeSedWeightConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/LikelihoodGridFuncConfig.h"
#include "PhzConfiguration/MarginalizationConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzConfiguration/PhotometricCorrectionConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzExecutables/ComputePhotometricCorrections.h"
#include "PhzUtils/Multithreading.h"

#include "PhzConfiguration/CosmologicalParameterConfig.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzExecutables/ComputeSedWeight.h"
#include "PhzModeling/NoIgmFunctor.h"
#include "PhzModeling/NormalizationFunctorFactory.h"
#include "PhzModeling/SparseGridCreator.h"
#include "PhzQtUI/DialogAddGalEbv.h"
#include "PhzUITools/ConfigurationWriter.h"

#include "ElementsKernel/Logging.h"

using namespace std;
using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("DialogPhotometricCorrectionComputation");

DialogPhotometricCorrectionComputation::DialogPhotometricCorrectionComputation(QWidget* parent)
    : QDialog(parent), ui(new Ui::DialogPhotometricCorrectionComputation) {
  ui->setupUi(this);
  m_non_detection = 0.;
  ui->txt_Iteration->setValidator(new QIntValidator(1, 1000, this));
  ui->txt_Tolerence->setValidator(new QDoubleValidator(0, 1, 8, this));

  connect(this, SIGNAL(signalUpdateCurrentIteration(const QString&)), ui->txt_current_iteration,
          SLOT(setText(const QString&)));

  connect(&m_future_watcher, SIGNAL(finished()), this, SLOT(runFinished()));
  connect(&m_future_lum_watcher, SIGNAL(finished()), this, SLOT(lumFinished()));
  connect(&m_future_sed_watcher, SIGNAL(finished()), this, SLOT(sedFinished()));
}

DialogPhotometricCorrectionComputation::~DialogPhotometricCorrectionComputation() {}

void DialogPhotometricCorrectionComputation::setData(
    string survey, string id_column, string model, string grid, std::list<FilterMapping> selected_filters,
    std::list<std::string> excluded_filters, std::string default_z_column,
    std::map<std::string, boost::program_options::variable_value>        run_option,
    const std::map<std::string, boost::program_options::variable_value>& sed_config, double non_detection,
    std::string dust_map_file, std::string ra_col, std::string dec_col) {
  m_id_column        = id_column;
  m_refZ_column      = default_z_column;
  m_selected_filters = selected_filters;
  m_excluded_filters = excluded_filters;
  m_run_option       = run_option;
  m_sed_config       = sed_config;
  m_non_detection    = non_detection;
  m_dust_map_file    = dust_map_file;
  m_ra_col           = ra_col;
  m_dec_col          = dec_col;
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

  // default correction name
  string default_file_name = survey + "_" + model + "_" + ui->cb_SelectionMethod->currentText().toStdString();
  ui->txt_FileName->setText(QString::fromStdString(default_file_name));

  logger.info() << "ra_col = " << m_ra_col << " dec_col = " << m_dec_col;
}

bool DialogPhotometricCorrectionComputation::loadTestCatalog(QString file_name, bool with_warning) {
  // Validate catalog
  auto              column_reader = PhzUITools::CatalogColumnReader(file_name.toStdString());
  map<string, bool> file_columns;

  for (auto& name : column_reader.getColumnNames()) {
    file_columns[name] = true;
  }

  bool        not_found       = false;
  std::string missing_columns = "";
  if (file_columns.count(m_id_column) == 1) {
    file_columns[m_id_column] = false;
  } else {
    not_found = true;
    missing_columns += "'" + m_id_column + "'";
  }

  for (auto& filter : m_selected_filters) {
    if (file_columns.count(filter.getFluxColumn()) == 1) {
      file_columns[filter.getFluxColumn()] = false;

    } else {
      if (not_found) {
        missing_columns += ", ";
      }
      missing_columns += "'" + filter.getFluxColumn() + "'";
      not_found = true;
    }

    if (file_columns.count(filter.getErrorColumn()) == 1) {

      file_columns[filter.getErrorColumn()] = false;

    } else {
      if (not_found) {
        missing_columns += ", ";
      }
      missing_columns += "'" + filter.getErrorColumn() + "'";
      not_found = true;
    }
  }

  if (not_found) {
    if (with_warning) {
      QMessageBox::warning(
          this, "Incompatible Data...",
          "The catalog file you selected has not the columns described into the Catalog and therefore cannot be used.\n"
          "Missing column(s):" +
              QString::fromStdString(missing_columns) + "\n Please select another catalog file.",
          QMessageBox::Ok);
    }
    return false;
  } else {
    ui->txt_catalog->setText(file_name);
    ui->cb_SpectroColumn->clear();

    int index          = 0;
    int selected_index = -1;

    for (auto& column_pair : file_columns) {
      if (column_pair.second) {
        ui->cb_SpectroColumn->addItem(QString::fromStdString(column_pair.first));
        if (m_refZ_column == column_pair.first) {
          selected_index = index;
        }
        index++;
      }
    }

    if (selected_index >= 0) {
      ui->cb_SpectroColumn->setCurrentIndex(selected_index);
    }

    return true;
  }
}

void DialogPhotometricCorrectionComputation::on_btn_TrainingCatalog_clicked() {
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);

  std::string path = ui->txt_catalog->text().toStdString();
  if (path.length() == 0) {
    path = FileUtils::getLastUsedPath();
  }
  dialog.selectFile(QString::fromStdString(path));
  if (dialog.exec()) {

    QString file_name = dialog.selectedFiles()[0];
    FileUtils::setLastUsedPath(file_name.toStdString());
    loadTestCatalog(file_name, true);
  }
}

void DialogPhotometricCorrectionComputation::setRunEnability() {
  bool run_ok = true;

  if (m_run_option.find("dust-column-density-column-name") != m_run_option.end() &&
      m_run_option.at("dust-column-density-column-name").as<std::string>() == "PLANCK_GAL_EBV" &&
      !boost::filesystem::exists(m_dust_map_file)) {
    QMessageBox::warning(this, "Missing Dust map file...",
                         "The file containing the Milky Way dust map is missing (" +
                             QString::fromStdString(m_dust_map_file) +
                             "). \n \n Click on the button \"Save config. File\" on the main window to download it or "
                             "change the Milky Way absorption type.",
                         QMessageBox::Ok);
    run_ok = false;
  }

  // A column name is selected
  run_ok &= ui->cb_SpectroColumn->currentText().length() > 0;

  // An iteration number is set
  run_ok &= ui->txt_Iteration->text().length() > 0;

  // A tolerance is set
  run_ok &= ui->txt_Tolerence->text().length() > 0;

  // A output name is set
  run_ok &= ui->txt_FileName->text().length() > 0;

  ui->bt_Run->setEnabled(run_ok);
  ui->btn_conf->setEnabled(run_ok);
}

void DialogPhotometricCorrectionComputation::on_cb_SelectionMethod_currentIndexChanged(const QString& method) {
  QString default_file_name = ui->txt_Model->text() + "_" + method;
  ui->txt_FileName->setText(default_file_name);
}

void DialogPhotometricCorrectionComputation::on_cb_SpectroColumn_currentIndexChanged(const QString&) {
  setRunEnability();
}

void DialogPhotometricCorrectionComputation::on_txt_Iteration_textChanged(const QString&) {
  setRunEnability();
}

void DialogPhotometricCorrectionComputation::on_txt_Tolerence_textChanged(const QString&) {
  setRunEnability();
}

void DialogPhotometricCorrectionComputation::on_txt_FileName_textChanged(const QString&) {
  setRunEnability();
  string output_file_name = FileUtils::addExt(ui->txt_FileName->text().toStdString(), ".txt");
  auto   path_filename    = FileUtils::getPhotCorrectionsRootPath(true, ui->txt_survey->text().toStdString()) +
                       QString(QDir::separator()).toStdString() + output_file_name;

  if (QFileInfo(QString::fromStdString(path_filename)).exists()) {
    ui->txt_FileName->setStyleSheet("QLineEdit { color: orange }");
  } else {
    ui->txt_FileName->setStyleSheet("QLineEdit { color: black }");
  }
}

void DialogPhotometricCorrectionComputation::disablePage() {
  ui->btn_TrainingCatalog->setEnabled(false);
  ui->cb_SpectroColumn->setEnabled(false);
  ui->txt_Iteration->setEnabled(false);
  ui->txt_Tolerence->setEnabled(false);
  ui->cb_SelectionMethod->setEnabled(false);
  ui->txt_FileName->setEnabled(false);
  ui->bt_Run->setEnabled(false);
  ui->btn_conf->setEnabled(false);
  ui->bt_Cancel->setEnabled(true);
}

void DialogPhotometricCorrectionComputation::enablePage() {
  ui->btn_TrainingCatalog->setEnabled(true);
  ui->cb_SpectroColumn->setEnabled(true);
  ui->txt_Iteration->setEnabled(true);
  ui->txt_Tolerence->setEnabled(true);
  ui->cb_SelectionMethod->setEnabled(true);
  ui->txt_FileName->setEnabled(true);
  ui->bt_Run->setEnabled(true);
  ui->btn_conf->setEnabled(true);
  ui->bt_Cancel->setEnabled(true);
  ui->txt_current_iteration->setText("");
}

std::string DialogPhotometricCorrectionComputation::runFunction() {

  try {

    int max_iter_number = ui->txt_Iteration->text().toInt();

    auto config_map = PhotometricCorrectionHandler::GetConfigurationMap(
        m_run_option, ui->txt_survey->text().toStdString(), ui->txt_FileName->text().toStdString(), max_iter_number,
        FormUtils::parseToDouble(ui->txt_Tolerence->text()), ui->cb_SelectionMethod->currentText().toStdString(),
        ui->txt_catalog->text().toStdString(), ui->cb_SpectroColumn->currentText().toStdString());

    completeWithDefaults<PhzConfiguration::ComputePhotometricCorrectionsConfig>(config_map);

    long  config_manager_id = Configuration::getUniqueManagerId();
    auto& config_manager    = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputePhotometricCorrectionsConfig>();
    config_manager.closeRegistration();
    config_manager.initialize(config_map);

    emit signalUpdateCurrentIteration(QString::fromStdString("Iteration : 0"));
    auto progress_logger = [this, max_iter_number](size_t iter_no, const PhzDataModel::PhotometricCorrectionMap&) {
      // If the user has canceled we do not want to update the progress bar,
      // because the GUI thread might have already deleted it
      if (!PhzUtils::getStopThreadsFlag()) {
        std::stringstream iter_no_message;
        iter_no_message << "Iteration : " << (iter_no + 1);
        emit signalUpdateCurrentIteration(QString::fromStdString(iter_no_message.str()));
      } else {
        emit signalUpdateCurrentIteration(QString::fromStdString("Canceling..."));
      }
    };

    PhzExecutables::ComputePhotometricCorrections{progress_logger}.run(config_manager);

    correctionComputed(ui->txt_FileName->text());
    return "";
  } catch (const Elements::Exception& e) {
    return "Sorry, an error occurred during the computation:\n" + std::string(e.what());
  } catch (const std::exception& e) {
    return "Sorry, an error occurred during the computation:\n" + std::string(e.what());
  } catch (...) {
    return "Sorry, an error occurred during the computation.";
  }
}

std::string DialogPhotometricCorrectionComputation::runSedFunction() {

  try {
    completeWithDefaults<PhzConfiguration::ComputeSedWeightConfig>(m_sed_config);
    long  config_manager_id = Configuration::getUniqueManagerId();
    auto& config_manager    = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<PhzConfiguration::ComputeSedWeightConfig>();
    config_manager.closeRegistration();
    config_manager.initialize(m_sed_config);

    auto monitor_function = [this](size_t step, size_t total) {
      int value = (step * 100) / total;
      // If the user has canceled we do not want to update the progress bar,
      // because the GUI thread might have already deleted it
      if (!PhzUtils::getStopThreadsFlag()) {
        std::stringstream progress_message;
        progress_message << "SED's Weights: " << value << "%";
        emit signalUpdateCurrentIteration(QString::fromStdString(progress_message.str()));
      } else {
        emit signalUpdateCurrentIteration(QString::fromStdString("Canceling..."));
      }
    };

    PhzExecutables::ComputeSedWeight{monitor_function}.run(config_manager);

    return "";

  } catch (const Elements::Exception& e) {
    return "Sorry, an error occurred during the SEDs' weights computation :\n" + std::string(e.what());
  } catch (const std::exception& e) {
    return "Sorry, an error occurred during the SEDs' weights computation:\n" + std::string(e.what());
  } catch (...) {
    return "Sorry, an error occurred during the SEDs' weights computation.";
  }
}

void DialogPhotometricCorrectionComputation::lumFinished() {
  auto message = m_future_lum_watcher.result();
  if (message.length() == 0) {
    if (m_sed_config.size() > 0) {
      m_future_sed_watcher.setFuture(QtConcurrent::run(this, &DialogPhotometricCorrectionComputation::runSedFunction));
    } else {
      m_future_watcher.setFuture(QtConcurrent::run(this, &DialogPhotometricCorrectionComputation::runFunction));
    }
  } else {
    m_computing = false;
    this->ui->bt_Cancel->setEnabled(true);
    QMessageBox::warning(this, "Error in the computation...", QString::fromStdString(message), QMessageBox::Close);
    enablePage();
  }
}

void DialogPhotometricCorrectionComputation::sedFinished() {
  auto message = m_future_sed_watcher.result();
  if (message.length() == 0) {
    m_future_watcher.setFuture(QtConcurrent::run(this, &DialogPhotometricCorrectionComputation::runFunction));
  } else {
    m_computing = false;
    this->ui->bt_Cancel->setEnabled(true);
    QMessageBox::warning(this, "Error in the computation...", QString::fromStdString(message), QMessageBox::Close);
    enablePage();
  }
}

void DialogPhotometricCorrectionComputation::runFinished() {
  std::string message = m_future_watcher.result();
  m_computing         = false;
  this->ui->bt_Cancel->setEnabled(true);
  if (message.length() == 0) {
    this->accept();
    return;
  } else {
    QMessageBox::warning(this, "Error in the computation...", QString::fromStdString(message), QMessageBox::Close);
    enablePage();
  }
}

void DialogPhotometricCorrectionComputation::on_bt_Cancel_clicked() {
  if (m_computing) {
    PhzUtils::getStopThreadsFlag() = true;
    this->ui->bt_Cancel->setEnabled(false);
  } else {
    this->reject();
  }
}

void DialogPhotometricCorrectionComputation::reject() {
  // Make sure the dialog does not close until the computation is done running/canceling
  if (m_computing) {
    QMessageBox::warning(this, "Running...",
                         "Please, cancel the computation and wait for it to stop before closing the dialog",
                         QMessageBox::Close);
  } else {
    QDialog::reject();
  }
}

void DialogPhotometricCorrectionComputation::on_bt_Run_clicked() {
  if (m_run_option.find("dust-column-density-column-name") != m_run_option.end() &&
      m_run_option.at("dust-column-density-column-name").as<std::string>() == "PLANCK_GAL_EBV") {

    std::string path             = ui->txt_catalog->text().toStdString();
    auto        column_reader    = PhzUITools::CatalogColumnReader(path);
    auto        column_from_file = column_reader.getColumnNames();
    if (column_from_file.find("PLANCK_GAL_EBV") == column_from_file.end()) {
      // the E(B-V) has to be looked up in the Planck map
      std::unique_ptr<DialogAddGalEbv> dialog(new DialogAddGalEbv());
      dialog->setInputs(path, m_ra_col, m_dec_col, m_dust_map_file);
      if (dialog->exec()) {
        // new catalog contains the PLANCK_GAL_EBV column
        ui->txt_catalog->setText(QString::fromStdString(dialog->getOutputName()));
      } else {
        // user has canceled the operation
        return;
      }
    }
  }

  PhzUtils::getStopThreadsFlag() = false;
  m_computing                    = true;
  string output_file_name        = FileUtils::addExt(ui->txt_FileName->text().toStdString(), ".txt");
  ui->txt_FileName->setText(QString::fromStdString(output_file_name));
  auto path_filename = FileUtils::getPhotCorrectionsRootPath(true, ui->txt_survey->text().toStdString()) +
                       QString(QDir::separator()).toStdString() + output_file_name;

  if (QFileInfo(QString::fromStdString(path_filename)).exists() &&
      QMessageBox::question(
          this, "Override existing file...",
          "A File with the very same name as the one you provided already exist. Do you want to replace it?",
          QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
    m_computing = false;
    return;
  }

  disablePage();
  if (m_sed_config.size() > 0) {
    m_future_sed_watcher.setFuture(QtConcurrent::run(this, &DialogPhotometricCorrectionComputation::runSedFunction));
  } else {
    m_future_watcher.setFuture(QtConcurrent::run(this, &DialogPhotometricCorrectionComputation::runFunction));
  }
}

void DialogPhotometricCorrectionComputation::on_btn_conf_clicked() {

  int  max_iter_number = ui->txt_Iteration->text().toInt();
  auto config_map      = PhotometricCorrectionHandler::GetConfigurationMap(
      m_run_option, ui->txt_survey->text().toStdString(), ui->txt_FileName->text().toStdString(), max_iter_number,
      FormUtils::parseToDouble(ui->txt_Tolerence->text()), ui->cb_SelectionMethod->currentText().toStdString(),
      ui->txt_catalog->text().toStdString(), ui->cb_SpectroColumn->currentText().toStdString());

  completeWithDefaults<PhzConfiguration::ComputePhotometricCorrectionsConfig>(config_map);
  std::vector<std::string> correction = {"PDF-sample-number",
                                         "create-output-best-likelihood-model",
                                         "create-output-best-model",
                                         "enable-photometric-correction",
                                         "full-PDF-sampling",
                                         "input-buffer-size",
                                         "output-catalog-format",
                                         "output-pdf-format",
                                         "output-pdf-normalized",
                                         "photometric-correction-file",
                                         "phz-output-dir"};

  for (const auto& value : correction) {
    config_map.erase(value);
  }

  QString filter = "Config (*.CPC.conf)";
  QString fileName =
      QFileDialog::getSaveFileName(this, tr("Save Configuration File"),
                                   QString::fromStdString(FileUtils::getRootPath(true)) + "config", filter, &filter);
  if (fileName.length() > 0) {

    QString cr{"\n\n"};
    QString command{""};

    if (!fileName.endsWith(".CPC.conf", Qt::CaseInsensitive)) {
      fileName = fileName + ".CPC.conf";
    }

    PhzUITools::ConfigurationWriter::writeConfiguration(config_map, fileName.toStdString());
    command += QString::fromStdString("Phosphoros CPC --config-file ") + fileName;

    if (m_run_option.find("dust-column-density-column-name") != m_run_option.end() &&
        m_run_option.at("dust-column-density-column-name").as<std::string>() == "PLANCK_GAL_EBV") {
      std::string path             = ui->txt_catalog->text().toStdString();
      auto        column_reader    = PhzUITools::CatalogColumnReader(path);
      auto        column_from_file = column_reader.getColumnNames();
      if (column_from_file.find("PLANCK_GAL_EBV") == column_from_file.end()) {
        path = ui->txt_catalog->text().toStdString();
        std::map<std::string, boost::program_options::variable_value> add_column_options_map{};
        add_column_options_map["planck-dust-map"].value() = boost::any(m_dust_map_file);
        add_column_options_map["galatic-ebv-col"].value() = boost::any(std::string("PLANCK_GAL_EBV"));
        add_column_options_map["input-catalog"].value()   = boost::any(path);
        add_column_options_map["ra"].value()              = boost::any(m_ra_col);
        add_column_options_map["dec"].value()             = boost::any(m_dec_col);
        add_column_options_map["output-catalog"].value()  = boost::any(path);
        auto lookup_planck_file_name                      = fileName.replace(".CPC.conf", ".AGDD.conf");

        PhzUITools::ConfigurationWriter::writeConfiguration(add_column_options_map,
                                                            lookup_planck_file_name.toStdString());
        command = QString::fromStdString("Phosphoros AGDD --config-file ") + lookup_planck_file_name + cr + command;
      }
    }
    if (m_sed_config.size() > 0) {
      completeWithDefaults<PhzConfiguration::ComputeSedWeightConfig>(m_sed_config);
      auto sed_file_name = fileName.replace(".CPC.conf", ".CSW.conf").replace(".AGDD.conf", ".CSW.conf");
      PhzUITools::ConfigurationWriter::writeConfiguration(m_sed_config, sed_file_name.toStdString());
      command = QString::fromStdString("Phosphoros CSW --config-file ") + sed_file_name + cr + command;
    }

    auto cmd_file_name =
        fileName.replace(".CPC.conf", ".cmd").replace(".AGDD.conf", ".cmd").replace(".CSW.conf", ".cmd");
    std::ofstream file;
    file.open(cmd_file_name.toStdString());
    file << command.toStdString();
    file.close();
  }
}

}  // namespace PhzQtUI
}  // namespace Euclid
