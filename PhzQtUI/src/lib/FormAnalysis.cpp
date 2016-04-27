#include <QStandardItemModel>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <functional>
#include <QFileDialog>
#include <boost/program_options.hpp>

#include "ElementsKernel/Exception.h"

#include "PhzQtUI/FormAnalysis.h"
#include "ui_FormAnalysis.h"
#include "PhzQtUI/DialogPhotCorrectionEdition.h"
#include "PhzQtUI/DialogPhotometricCorrectionComputation.h"
#include "FileUtils.h"
#include "PreferencesUtils.h"
#include "PhzQtUI/ModelSet.h"
#include "PhzQtUI/PhotometricCorrectionHandler.h"
#include "PhzQtUI/SurveyFilterMapping.h"
#include "PhzQtUI/PhzGridInfoHandler.h"
#include "PhzQtUI/DialogGridGeneration.h"
#include "PhzQtUI/DialogRunAnalysis.h"
#include "PhzQtUI/DialogLuminosityPrior.h"

#include "PhzUITools/ConfigurationWriter.h"
#include "PhzUITools/CatalogColumnReader.h"

namespace Euclid {
namespace PhzQtUI {

FormAnalysis::FormAnalysis(QWidget *parent) :
    QWidget(parent), ui(new Ui::FormAnalysis) {
  ui->setupUi(this);
}

FormAnalysis::~FormAnalysis() {
}

///////////////////////////////////////////////////
//  Initial data load
void FormAnalysis::loadAnalysisPage() {
  m_analysis_survey_list = SurveyFilterMapping::loadCatalogMappings();

    ui->cb_AnalysisSurvey->clear();
    for (auto& survey : m_analysis_survey_list) {
      ui->cb_AnalysisSurvey->addItem(
          QString::fromStdString(survey.second.getName()));
    }

    m_analysis_model_list = ModelSet::loadModelSetsFromFolder(
        FileUtils::getModelRootPath(false));
    ui->cb_AnalysisModel->clear();
    for (auto& model : m_analysis_model_list) {
      ui->cb_AnalysisModel->addItem(
          QString::fromStdString(model.second.getName()));
    }

    updateGridSelection();
}
///////////////////////////////////////////////////
//  Handle controls enability

void FormAnalysis::updateGridSelection() {
try{
  ModelSet selected_model;

  for (auto&model : m_analysis_model_list) {
    if (model.second.getName().compare(
        ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
      selected_model = model.second;
      break;
    }
  }

  auto axis = selected_model.getAxesTuple();
  auto possible_files = PhzGridInfoHandler::getCompatibleGridFile(
      ui->cb_AnalysisSurvey->currentText().toStdString(), axis,
      getSelectedFilters(), ui->cb_igm->currentText().toStdString());

  ui->cb_CompatibleGrid->clear();
  bool added = false;
  for (auto& file : possible_files) {
    ui->cb_CompatibleGrid->addItem(QString::fromStdString(file));
    added = true;
  }

  if (!added) {
    ui->cb_CompatibleGrid->addItem(
        QString::fromStdString("Grid_" + selected_model.getName() + "_")
            + ui->cb_igm->currentText());
  }

  ui->cb_CompatibleGrid->addItem("<Enter a new name>");
} catch (Elements::Exception){
  if (ui->cb_AnalysisModel->currentIndex()>-1){
    ui->cb_AnalysisModel->removeItem(ui->cb_AnalysisModel->currentIndex());
  }
  }
}

void FormAnalysis::updateCorrectionSelection() {
  auto filter_map = getSelectedFilters();
  auto file_list = PhotometricCorrectionHandler::getCompatibleCorrectionFiles(
      ui->cb_AnalysisSurvey->currentText().toStdString(), filter_map);
  ui->cb_AnalysisCorrection->clear();

  for (auto file : file_list) {
    ui->cb_AnalysisCorrection->addItem(QString::fromStdString(file));
  }
}

void FormAnalysis::adjustPhzGridButtons(bool enabled) {
  bool name_ok = checkGridSelection(false, true);
  ui->btn_GetConfigGrid->setEnabled(enabled && name_ok);
  ui->btn_RunGrid->setEnabled(enabled && name_ok);
  QString tool_tip = "";

  if (!name_ok) {
    ui->cb_CompatibleGrid->lineEdit()->setStyleSheet(
        "QLineEdit { color: red }");
    setToolBoxButtonColor(ui->toolBox, 0, Qt::red);
    tool_tip =
        "Please enter a valid grid name in order to compute the Grid or export the corresponding configuration.";
  } else if (!checkGridSelection(true, false)) {
    ui->cb_CompatibleGrid->lineEdit()->setStyleSheet(
        "QLineEdit { color: orange }");
    setToolBoxButtonColor(ui->toolBox, 0, QColor("orange"));

  } else {
    ui->cb_CompatibleGrid->lineEdit()->setStyleSheet(
        "QLineEdit { color: black }");
    setToolBoxButtonColor(ui->toolBox, 0,Qt::black);
  }
  ui->btn_GetConfigGrid->setToolTip(tool_tip);
  ui->btn_RunGrid->setToolTip(tool_tip);

}

void FormAnalysis::setComputeCorrectionEnable() {
  bool name_exists = checkGridSelection(true, false);
  ui->btn_computeCorrections->setEnabled(
      name_exists && ui->gb_corrections->isChecked());

  QString tool_tip = "Open the photometric zero-point correction popup.";
  if (!name_exists) {
    tool_tip =
        "Please run the photometric grid computation before computing the photometric corrections.";
  };
  ui->btn_computeCorrections->setToolTip(tool_tip);
}

void FormAnalysis::setRunAnnalysisEnable(bool enabled) {

  bool grid_name_ok = checkGridSelection(false, true);
  bool grid_name_exists = checkGridSelection(true, false);
  bool correction_ok = !ui->gb_corrections->isChecked()
      || ui->cb_AnalysisCorrection->currentText().toStdString().length() > 0;

  QFileInfo info(
      QString::fromStdString(
          FileUtils::getPhotCorrectionsRootPath(false,
              ui->cb_AnalysisSurvey->currentText().toStdString()))
          + QDir::separator() + ui->cb_AnalysisCorrection->currentText());
  bool correction_exists = !ui->gb_corrections->isChecked() || info.exists();

  QFileInfo info_input(ui->txt_inputCatalog->text());
  bool run_ok = info_input.exists();


  bool lum_prior_ok = !ui->cb_luminosityPrior->isChecked() || ui->cb_luminosityPrior_2->currentText().length()>0;
  bool lum_prior_compatible = true;
  if (lum_prior_ok && ui->cb_luminosityPrior->isChecked()){
    LuminosityPriorConfig info;
    for (auto prior_pair : m_prior_config) {
      if (ui->cb_luminosityPrior_2->currentText().toStdString() == prior_pair.first) {
        info = prior_pair.second;
      }
    }

    ModelSet selected_model;
    for (auto& model : m_analysis_model_list) {
      if (model.second.getName().compare(
          ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
        selected_model = model.second;
        break;
      }
    }

    std::vector<std::string> seds { };
    double z_min = 1000000;
    double z_max = 0;

    for (auto& rule : selected_model.getParameterRules()) {
      for (auto& z_range : rule.second.getZRanges()) {
        if (z_min > z_range.getMin()) {
          z_min = z_range.getMin();
        }

        if (z_max < z_range.getMax()) {
          z_max = z_range.getMax();
        }
      }

      for (double value : rule.second.getRedshiftValues()) {
        if (z_min > value) {
          z_min = value;
        }

        if (z_max < value) {
          z_max = value;
        }
      }
    }

    lum_prior_compatible=info.isCompatibleWithParameterSpace(z_min,z_max,selected_model.getSeds());

  }

  ui->btn_confLuminosityPrior->setEnabled(grid_name_exists);
  if (grid_name_exists) {
    ui->btn_confLuminosityPrior->setToolTip("Configure the Luminosity Prior");
  } else {
    ui->btn_confLuminosityPrior->setToolTip("You need to Generate the Model Grid before to configure the Luminosity Prior");
  }


  ui->btn_GetConfigAnalysis->setEnabled(
      grid_name_ok && correction_ok && lum_prior_ok && lum_prior_compatible && run_ok && enabled);
  ui->btn_RunAnalysis->setEnabled(
      grid_name_exists && correction_exists && lum_prior_ok && lum_prior_compatible&& run_ok && enabled);

  QString tool_tip_run = "";
  QString tool_tip_conf = "";
  if (!grid_name_ok) {
    tool_tip_conf = tool_tip_conf + "Please enter a valid grid name. \n";
    tool_tip_run = tool_tip_run + "Please enter a valid grid name. \n";
  };

  if (!grid_name_exists) {
    tool_tip_run = tool_tip_run + "Please run the model grid computation. \n";
  }

  if (!correction_ok) {
    tool_tip_conf =
        tool_tip_conf
            + "When the photometric corrections are enabled, you must provide a valid correction file name. \n";
    tool_tip_run =
        tool_tip_run
            + "When the photometric corrections are enabled, you must provide a valid correction file name. \n";
  }

  if (!correction_exists) {
    tool_tip_run = tool_tip_run
        + "Please run the photometric correction computation. \n";
  }

  if (!lum_prior_ok){
    tool_tip_conf = tool_tip_conf
                + "When the luminosity prior is enabled , you must provide a luminosity prior configuration. \n";
    tool_tip_run = tool_tip_run
                + "When the luminosity prior is enabled , you must provide a luminosity prior configuration. \n";
  }

  if (!lum_prior_compatible){
    tool_tip_conf = tool_tip_conf
                   + "The Prior is no longer compatible with the Parameter Space, please update it. \n";
    tool_tip_run = tool_tip_run
                   + "The Prior is no longer compatible with the Parameter Space, please update it. \n";
  }


  if (!info_input.exists()) {
    ui->txt_inputCatalog->setStyleSheet("QLineEdit { color: #F78181 }");
    tool_tip_conf =
        tool_tip_conf
            + "Please provide a compatible input catalog (at least all the columns used for the Id and filters). \n";
    tool_tip_run =
        tool_tip_run
            + "Please provide a compatible input catalog (at least all the columns used for the Id and filters). \n";

  } else {
    ui->txt_inputCatalog->setStyleSheet("QLineEdit { color: grey }");
  }

  if (!(grid_name_ok && correction_ok && lum_prior_ok && lum_prior_compatible && run_ok)) {
    tool_tip_conf = tool_tip_conf + "Before getting the configuration.";
  } else {
    tool_tip_conf = "Get the configuration file.";
  }

  if (!(grid_name_exists && correction_exists && lum_prior_ok && lum_prior_compatible && run_ok)) {
    tool_tip_run = tool_tip_run + "Before running the analysis.";
  } else {
    tool_tip_run = "Run the analysis.";
  }

  ui->btn_GetConfigAnalysis->setToolTip(tool_tip_conf);
  ui->btn_RunAnalysis->setToolTip(tool_tip_run);

  if (!correction_ok) {
    setToolBoxButtonColor(ui->toolBox, 1, QColor("orange"));
  } else {
    setToolBoxButtonColor(ui->toolBox, 1,Qt::black);
  }

  if (!lum_prior_ok || !lum_prior_compatible) {
     setToolBoxButtonColor(ui->toolBox, 2, QColor("orange"));
   } else {
     setToolBoxButtonColor(ui->toolBox, 2,Qt::black);
   }
}

//////////////////////////////////////////////////
// Build and handle objects for calling the processing

std::string FormAnalysis::getSelectedSurveySourceColumn() {

  auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();
  for (auto& survey_pair : m_analysis_survey_list) {
    if (survey_pair.second.getName().compare(survey_name) == 0) {
      return survey_pair.second.getSourceIdColumn();
    }
  }

  return "";
}

std::list<std::string> FormAnalysis::getFilters() {
  std::list<std::string> res;
  auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();
  for (auto& survey_pair : m_analysis_survey_list) {
    if (survey_pair.second.getName().compare(survey_name) == 0) {
      for (auto filter : survey_pair.second.getFilters()) {
        res.push_back(filter.getFilterFile());
      }
    }
  }
  return res;
}

std::list<std::string> FormAnalysis::getSelectedFilters() {
  std::list<std::string> res;
  if (!ui->cb_AnalysisSurvey->currentText().isEmpty()) {
    auto model = static_cast<QStandardItemModel*>(ui->tableView_filter->model());
    for (int i = 0; i < model->rowCount(); ++i) {
      auto item = model->item(i);
      if (item->checkState() == Qt::CheckState::Checked) {
        res.push_back(model->item(i, 0)->text().toStdString());
      }
    }
  }
  return res;
}

std::list<std::string> FormAnalysis::getExcludedFilters() {
  std::list<std::string> res;
  if (!ui->cb_AnalysisSurvey->currentText().isEmpty()) {
    auto model = static_cast<QStandardItemModel*>(ui->tableView_filter->model());
    for (int i = 0; i < model->rowCount(); ++i) {
      auto item = model->item(i);
      if (item->checkState() != Qt::CheckState::Checked) {
        res.push_back(model->item(i, 0)->text().toStdString());
      }
    }
  }
  return res;

}

std::list<FilterMapping> FormAnalysis::getSelectedFilterMapping() {
  auto filterNames = getSelectedFilters();
  std::list<FilterMapping> list;
  if (!ui->cb_AnalysisSurvey->currentText().isEmpty()) {
    for (auto& survey : m_analysis_survey_list) {
      if (survey.second.getName().compare(
          ui->cb_AnalysisSurvey->currentText().toStdString()) == 0) {
        for (auto& name : filterNames) {
          for (auto& filter : survey.second.getFilters()) {
            if (filter.getFilterFile().compare(name) == 0) {
              list.push_back(filter);
            }
          }
        }
      }
    }
  }

  return list;
}

bool FormAnalysis::checkGridSelection(bool addFileCheck, bool acceptNewFile) {
  std::string file_name = ui->cb_CompatibleGrid->currentText().toStdString();

  if (file_name.compare("<Enter a new name>") == 0) {
    return false;
  }

  if (!addFileCheck) {
    return true;
  }

  QFileInfo info(
      QString::fromStdString(
          FileUtils::getPhotmetricGridRootPath(false,
              ui->cb_AnalysisSurvey->currentText().toStdString()))
          + QDir::separator() + QString::fromStdString(file_name));

  return acceptNewFile || info.exists();
}

std::map<std::string, boost::program_options::variable_value> FormAnalysis::getGridConfiguration() {
  std::string file_name = FileUtils::addExt(
      ui->cb_CompatibleGrid->currentText().toStdString(), ".dat");
  ui->cb_CompatibleGrid->setItemText(ui->cb_CompatibleGrid->currentIndex(),
      QString::fromStdString(file_name));
  ModelSet selected_model;

  for (auto&model : m_analysis_model_list) {
    if (model.second.getName().compare(
        ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
      selected_model = model.second;
      break;
    }
  }



  return PhzGridInfoHandler::GetConfigurationMap(
      ui->cb_AnalysisSurvey->currentText().toStdString(), file_name, selected_model,
      getFilters(), ui->cb_igm->currentText().toStdString());
}

std::map<std::string, boost::program_options::variable_value> FormAnalysis::getRunOptionMap() {

  auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();
  double non_detection = -99.;
  for (auto& survey_pair : m_analysis_survey_list) {
    if (survey_pair.second.getName().compare(survey_name) == 0) {
      non_detection = survey_pair.second.getNonDetection();
    }
  }

  ModelSet selected_model;

   for (auto&model : m_analysis_model_list) {
     if (model.second.getName().compare(
         ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
       selected_model = model.second;
       break;
     }
   }

  std::map<std::string, boost::program_options::variable_value> options_map =
      FileUtils::getPathConfiguration(true,false,true,true);

  auto global_options = PreferencesUtils::getThreadConfigurations();
  for(auto& pair : global_options){
      options_map[pair.first]=pair.second;
  }

  global_options = PreferencesUtils::getCosmologyConfigurations();
   for(auto& pair : global_options){
       options_map[pair.first]=pair.second;
   }

  options_map["catalog-type"].value() = boost::any(survey_name);


  options_map["phz-output-dir"].value() = boost::any(
      ui->txt_outputFolder->text().toStdString());

  options_map["model-grid-file"].value() = boost::any(ui->cb_CompatibleGrid->currentText().toStdString());

  auto input_catalog_file = FileUtils::removeStart(ui->txt_inputCatalog->text().toStdString(),
      FileUtils::getCatalogRootPath(false,survey_name)+QString(QDir::separator()).toStdString());
  options_map["input-catalog-file"].value() = boost::any(input_catalog_file);



  options_map["source-id-column-name"].value() = boost::any(getSelectedSurveySourceColumn());
  options_map["missing-photometry-flag"].value() = boost::any(non_detection);


  auto filter_excluded = getExcludedFilters();
  if (filter_excluded.size() > 0) {
    std::vector<std::string> excluded;
    for (auto& filter : filter_excluded) {
      excluded.push_back(filter);
    }
    options_map["exclude-filter"].value() = boost::any(excluded);
  }

  std::string yes_flag = "YES";

  if (ui->gb_corrections->isChecked()) {
    options_map["enable-photometric-correction"].value() = boost::any(yes_flag);
    options_map["photometric-correction-file"].value() = boost::any(
        ui->cb_AnalysisCorrection->currentText().toStdString());
  }

  options_map["axes-collapse-type"].value() = boost::any(
      ui->cb_marginalization->currentText().toStdString());

  if (ui->gb_cat->isChecked()) {

    options_map["create-output-catalog"].value() = boost::any(yes_flag);
    options_map["output-catalog-format"].value() = boost::any(
        ui->cb_cat_output_type->currentText().toStdString());
  }

  if (ui->gb_pdf->isChecked()) {
    options_map["create-output-pdf"].value() = boost::any(yes_flag);
  }

  if (ui->gb_lhood->isChecked()) {
   options_map["create-output-likelihoods"].value() = boost::any(yes_flag);
  }

  if (ui->gb_lik->isChecked()) {
    options_map["create-output-posteriors"].value() = boost::any(yes_flag);
  }


  if (ui->cb_luminosityPrior->isChecked()){
    std::string lum_prior_name = ui->cb_luminosityPrior_2->currentText().toStdString();
    auto& lum_prior_config = m_prior_config.at(lum_prior_name);
    auto lum_prior_option =lum_prior_config.getConfigOptions();
    options_map.insert(lum_prior_option.begin(),lum_prior_option.end());

  }

  if (ui->cb_volumePrior->isChecked()) {
    options_map["volume-prior"].value() = boost::any(yes_flag);
  }

  return options_map;
}


std::map < std::string, boost::program_options::variable_value > FormAnalysis::getLuminosityOptionMap(){
  std::map<std::string, boost::program_options::variable_value> options_map =
      FileUtils::getPathConfiguration(false, true, true, true);

  if (ui->cb_luminosityPrior->isChecked()) {

    LuminosityPriorConfig info;
    for (auto prior_pair : m_prior_config) {
      if (ui->cb_luminosityPrior_2->currentText().toStdString()
          == prior_pair.first) {
        info = prior_pair.second;
      }
    }

    for (auto& pair : info.getBasicConfigOptions(false)) {
      options_map[pair.first] = pair.second;
    }

    auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

    auto global_options = PreferencesUtils::getThreadConfigurations();
    for(auto& pair : global_options){
          options_map[pair.first]=pair.second;
    }

    options_map["catalog-type"].value() = boost::any(std::string(survey_name));

    options_map["model-grid-file"].value() = boost::any(
        ui->cb_CompatibleGrid->currentText().toStdString());

    options_map["output-model-grid"].value() = boost::any(
        info.getLuminosityModelGridName());
  }
  return options_map;
}
//////////////////////////////////////////////////
// User interaction
void FormAnalysis::on_btn_AnalysisToHome_clicked() {
  navigateToHome();
}

void FormAnalysis::on_btn_backHome_clicked() {
  navigateToHome();
}

//  1. Survey and Model
void FormAnalysis::on_cb_AnalysisSurvey_currentIndexChanged(
    const QString &selectedName) {

  SurveyFilterMapping selected_survey;

  for (auto&survey : m_analysis_survey_list) {
    if (survey.second.getName().compare(selectedName.toStdString()) == 0) {
      selected_survey = survey.second;
      break;
    }
  }

  QStandardItemModel* grid_model = new QStandardItemModel();
  grid_model->setColumnCount(1);

  for (auto filter : selected_survey.getFilters()) {
    QStandardItem* item = new QStandardItem(
        QString::fromStdString(filter.getFilterFile()));
    item->setCheckable(true);
    item->setCheckState(Qt::CheckState::Checked);

    QList<QStandardItem*> items;
    items.push_back(item);

    grid_model->appendRow(items);

  }

  ui->tableView_filter->setModel(grid_model);
  connect(grid_model, SIGNAL(itemChanged(QStandardItem*)),
      SLOT(onFilterSelectionItemChanged(QStandardItem*)));

  // push the default catalog
  setInputCatalogName(selected_survey.getDefaultCatalogFile(), false);

  // set the stored IGM
  auto saved_igm = PreferencesUtils::getUserPreference(selected_survey.getName(),
      "IGM");
  if (saved_igm.length() > 0) {
    for (int i = 0; i < ui->cb_igm->count(); i++) {
      if (ui->cb_igm->itemText(i).toStdString() == saved_igm) {
        ui->cb_igm->setCurrentIndex(i);
        break;
      }
    }
  }

  // set the stored Collapse
  auto saved_collapse = PreferencesUtils::getUserPreference(selected_survey.getName(),
      "Collapse");
  if (saved_collapse.length() > 0) {
    for (int i = 0; i < ui->cb_marginalization->count(); i++) {
      if (ui->cb_marginalization->itemText(i).toStdString() == saved_collapse) {
        ui->cb_marginalization->setCurrentIndex(i);
        break;
      }
    }
  }

  updateGridSelection();
  loadLuminosityPriors();
  updateCorrectionSelection();

  // set the correction file
  auto saved_correction = PreferencesUtils::getUserPreference(
      selected_survey.getName(), "Correction");
  if (saved_correction.length() > 0) {
    for (int i = 0; i < ui->cb_AnalysisCorrection->count(); i++) {
      if (ui->cb_AnalysisCorrection->itemText(i).toStdString()
          == saved_correction) {
        ui->cb_AnalysisCorrection->setCurrentIndex(i);
        ui->gb_corrections->setChecked(true);
        break;

      } else {
        ui->gb_corrections->setChecked(false);
      }

    }
  }
}

  void FormAnalysis::on_cb_AnalysisModel_currentIndexChanged(const QString &) {

    updateGridSelection();
    loadLuminosityPriors();
  }

  void FormAnalysis::on_cb_igm_currentIndexChanged(const QString &)
  {
    updateGridSelection();
  }

  void FormAnalysis::onFilterSelectionItemChanged(QStandardItem*) {
    updateGridSelection();
    updateCorrectionSelection();
  }

//  2. Photometry Grid
  void FormAnalysis::on_cb_CompatibleGrid_textChanged(const QString &) {
    adjustPhzGridButtons(true);
    setComputeCorrectionEnable();
    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_btn_GetConfigGrid_clicked() {
    if (!checkGridSelection(true, true)) {
      QMessageBox::warning(this, "Unavailable name...",
          "It is not possible to save the Grid under the name you have provided. Please enter a new name.",
          QMessageBox::Ok);
    } else {

      QString fileName = QFileDialog::getSaveFileName(this,
          tr("Save Configuration File"),
          QString::fromStdString(FileUtils::getRootPath(true))+"config",
          tr("Config (*.conf)"));
      if (fileName.length()>0) {
        auto config_map = getGridConfiguration();
        PhzUITools::ConfigurationWriter::writeConfiguration(config_map,fileName.toStdString());

        PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
            "IGM",ui->cb_igm->currentText().toStdString());

      }
    }
  }

  void FormAnalysis::on_btn_RunGrid_clicked() {

    if (!checkGridSelection(true, true)) {
      QMessageBox::warning(this, "Unavailable name...",
          "It is not possible to save the Grid under the name you have provided. Please enter a new name.",
          QMessageBox::Ok);
    } else {

      if (checkGridSelection(true, false)) {
        if (QMessageBox::warning(this, "Override existing file...",
                "A Model Grid file with the very same name as the one you provided already exist. "
                "Do you want to replace it?", QMessageBox::Yes | QMessageBox::No)
            == QMessageBox::No) {
          return;
        }
      }

      auto config_map = getGridConfiguration();
      std::unique_ptr<DialogGridGeneration> dialog(new DialogGridGeneration());
      dialog->setValues(ui->cb_CompatibleGrid->currentText().toStdString(), config_map);
      if (dialog->exec()) {
        adjustPhzGridButtons(true);
        setComputeCorrectionEnable();
        setRunAnnalysisEnable(true);

        PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
            "IGM",ui->cb_igm->currentText().toStdString());
      }
    }
  }

//  3. Photometric Correction
  void FormAnalysis::on_gb_corrections_clicked()
  {
    setRunAnnalysisEnable(true);
    setComputeCorrectionEnable();
  }

  void FormAnalysis::on_cb_AnalysisCorrection_currentIndexChanged(
      const QString &selectedText) {
    ui->btn_editCorrections->setEnabled(selectedText.length() > 0);
    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_btn_editCorrections_clicked() {
    std::unique_ptr<DialogPhotCorrectionEdition> popup(new DialogPhotCorrectionEdition());
    popup->setCorrectionsFile(
        ui->cb_AnalysisSurvey->currentText().toStdString(),
        ui->cb_AnalysisCorrection->currentText().toStdString(),
        getSelectedFilterMapping());
    popup->exec();
  }

  void FormAnalysis::on_btn_computeCorrections_clicked() {

    auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

    SurveyFilterMapping selected_survey;

    for (auto&survey : m_analysis_survey_list) {
      if (survey.second.getName().compare(survey_name) == 0) {
        selected_survey = survey.second;
        break;
      }
    }

    std::unique_ptr<DialogPhotometricCorrectionComputation> popup(new DialogPhotometricCorrectionComputation());
    popup->setData(survey_name,getSelectedSurveySourceColumn(),
        ui->cb_AnalysisModel->currentText().toStdString(),
        ui->cb_CompatibleGrid->currentText().toStdString(),
        getSelectedFilterMapping(),
        getExcludedFilters(),
        selected_survey.getDefaultCatalogFile(),
        selected_survey.getNonDetection());

    connect( popup.get(), SIGNAL(correctionComputed(const QString &)),
        SLOT(onCorrectionComputed(const QString &)));
    popup->exec();

  }

  void FormAnalysis::onCorrectionComputed(const QString & new_file_name) {
    updateCorrectionSelection();
    ui->cb_AnalysisCorrection->setCurrentIndex(ui->cb_AnalysisCorrection->findText(new_file_name));
  }



// 4. algorithm options
  void FormAnalysis::on_btn_confLuminosityPrior_clicked(){
        std::unique_ptr<DialogLuminosityPrior> dialog(new DialogLuminosityPrior());

        std::string model_grid = ui->cb_CompatibleGrid->currentText().toStdString();

        auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

        ModelSet selected_model;

        for (auto&model : m_analysis_model_list) {
            if (model.second.getName().compare(
                ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
              selected_model = model.second;
              break;
            }
        }

        std::vector<std::string> seds{};

        double z_min=1000000;
        double z_max=0;

        for (auto& rule : selected_model.getParameterRules()) {
          for (auto& z_range : rule.second.getZRanges()) {
            if (z_min > z_range.getMin()) {
              z_min = z_range.getMin();
            }

            if (z_max < z_range.getMax()) {
              z_max = z_range.getMax();
            }
          }

          for (double value : rule.second.getRedshiftValues()) {
            if (z_min > value) {
              z_min = value;
            }

            if (z_max < value) {
              z_max = value;
            }
          }
        }

        dialog->loadData(selected_model,survey_name,model_grid,z_min,z_max);
        dialog->exec();
        loadLuminosityPriors();

  }

  void FormAnalysis::loadLuminosityPriors(){
    auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

    ModelSet selected_model;

         for (auto&model : m_analysis_model_list) {
             if (model.second.getName().compare(
                 ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
               selected_model = model.second;
               break;
             }
         }

    auto folder = FileUtils::getGUILuminosityPriorConfig(true,survey_name,selected_model.getName());

    m_prior_config = LuminosityPriorConfig::readFolder(folder);

    auto prior_name = PreferencesUtils::getUserPreference(survey_name,selected_model.getName()+"_LuminosityPriorName");


    ui->cb_luminosityPrior_2->clear();
    int index=-1;
    int id=0;
    for (auto prior_pair : m_prior_config){
      ui->cb_luminosityPrior_2->addItem(QString::fromStdString(prior_pair.first));

      if (prior_name==prior_pair.first){
        index=id;
      }
      ++id;
    }


    if (index>=0){
      ui->cb_luminosityPrior_2->setCurrentIndex(index);
    }

    auto luminosity_prior_enabled = PreferencesUtils::getUserPreference(
           ui->cb_AnalysisSurvey->currentText().toStdString(), ui->cb_AnalysisModel->currentText().toStdString()+"_LuminosityPriorEnabled");

    ui->cb_luminosityPrior->setChecked(luminosity_prior_enabled=="1");

    auto volume_prior_enabled = PreferencesUtils::getUserPreference(
           ui->cb_AnalysisSurvey->currentText().toStdString(), ui->cb_AnalysisModel->currentText().toStdString()+"_VolumePriorEnabled");

    ui->cb_volumePrior->setChecked(volume_prior_enabled=="1");
  }


  void FormAnalysis::on_cb_luminosityPrior_2_currentIndexChanged(const QString &){
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                ui->cb_AnalysisModel->currentText().toStdString()+"_LuminosityPriorName",ui->cb_luminosityPrior_2->currentText().toStdString());

    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_cb_luminosityPrior_stateChanged(int){
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                    ui->cb_AnalysisModel->currentText().toStdString()+"_LuminosityPriorEnabled",QString::number(ui->cb_luminosityPrior->isChecked()).toStdString());
    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_cb_volumePrior_stateChanged(int) {
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                    ui->cb_AnalysisModel->currentText().toStdString()+"_VolumePriorEnabled",QString::number(ui->cb_volumePrior->isChecked()).toStdString());
  }


// 5. Run
  void FormAnalysis::setInputCatalogName(std::string name,bool do_test) {
    bool not_found = false;
    if (do_test) {
      auto column_reader = PhzUITools::CatalogColumnReader(name);
      std::map<std::string, bool> file_columns;

      for (auto& name : column_reader.getColumnNames()) {
        file_columns[name] = true;
      }

      if (file_columns.count(getSelectedSurveySourceColumn()) == 1) {
        file_columns[getSelectedSurveySourceColumn()] = false;
      } else {
        not_found = true;
      }

      for (auto& filter : getSelectedFilterMapping()) {
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
    }

    if (not_found) {
      if (QMessageBox::question(this, "Incompatible Data...",
              "The catalog file you selected has not the columns described into the Catalog and therefore cannot be used. "
              "Do you want to create a new Catalog mapping for this file?",
              QMessageBox::Cancel|QMessageBox::Ok)==QMessageBox::Ok) {
        navigateToNewCatalog(name);
      }
    } else {
      ui->txt_inputCatalog->setText(QString::fromStdString(name));
      QFileInfo info(QString::fromStdString(name));
      ui->txt_outputFolder->setText(QString::fromStdString(FileUtils::getResultRootPath(false,ui->cb_AnalysisSurvey->currentText().toStdString(), info.baseName().toStdString())));
    }
  }

  void FormAnalysis::on_btn_BrowseInput_clicked()
  {
    QFileDialog dialog(this);
    std::string path = ui->txt_inputCatalog->text().toStdString();

    dialog.selectFile(QString::fromStdString(path));
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
      setInputCatalogName(dialog.selectedFiles()[0].toStdString());
      setRunAnnalysisEnable(true);
    }

  }

  void FormAnalysis::on_btn_BrowseOutput_clicked()
  {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.selectFile(ui->txt_outputFolder->text());
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    dialog.setLabelText( QFileDialog::Accept, "Select" );
    if (dialog.exec()) {
      ui->txt_outputFolder->setText(dialog.selectedFiles()[0]);
      setRunAnnalysisEnable(true);
    }
  }


  void FormAnalysis::on_gb_lhood_clicked(){
    if (ui->gb_lhood->isChecked ()) {
          QMessageBox::warning(this, "Large output volume...",
              "Outputing multi-dimensional likelihood grids (one file per source)"
              " will generate a large output volume.", QMessageBox::Ok );
        }
        setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_gb_lik_clicked() {
    if (ui->gb_lik->isChecked ()) {
      QMessageBox::warning(this, "Large output volume...",
          "Outputing multi-dimensional posterior grids (one file per source)"
          " will generate a large output volume.", QMessageBox::Ok );
    }
    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_btn_GetConfigAnalysis_clicked()
  {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Configuration File"),
        QString::fromStdString(FileUtils::getRootPath(true))+"config",
        tr("Config (*.conf)"));
    if (fileName.length()>0) {
      auto config_map = getRunOptionMap();
      PhzUITools::ConfigurationWriter::writeConfiguration(config_map,fileName.toStdString());

      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
          "IGM",ui->cb_igm->currentText().toStdString());
      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
          "Collapse",ui->cb_marginalization->currentText().toStdString());
      if (ui->gb_corrections->isChecked ()) {
        PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
            "Correction",ui->cb_AnalysisCorrection->currentText().toStdString());
      }
    }
  }

  void FormAnalysis::on_btn_RunAnalysis_clicked()
  {

    std::string cat="";
    if (ui->gb_cat->isChecked()) {

      cat=QString(ui->txt_outputFolder->text()+QDir::separator()+"phz_cat").toStdString();
      if (ui->cb_cat_output_type->currentText()=="ASCII") {
        cat=cat+".txt";
      }
      else {
        cat=cat+".fits";
      }

      if (QFileInfo(QString::fromStdString(cat)).exists()) {
        if (QMessageBox::question(this, "Override existing file...",
                "A Photometric Redshift Catalog for the same input catalog file already exists. Do you want to replace it?",
                QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
          return;
        }
      }

    }
    std::string pdf="";
    if (ui->gb_pdf->isChecked()) {
      pdf=QString(ui->txt_outputFolder->text()+QDir::separator()+"pdf.fits").toStdString();
      if (QFileInfo(QString::fromStdString(pdf)).exists()) {
        if (QMessageBox::question(this, "Override existing file...",
                "A PDF file for the same input catalog file already exists. Do you want to replace it?",
                QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
          return;
        }
      }
    }

    std::string lik="";
    if (ui->gb_lhood->isChecked()) {
      lik=QString(QString(ui->txt_outputFolder->text()+QDir::separator()+"likelihoods"+QDir::separator())).toStdString();
    }

    std::string pos="";
       if (ui->gb_lik->isChecked()) {
         lik=QString(QString(ui->txt_outputFolder->text()+QDir::separator()+"posteriors"+QDir::separator())).toStdString();
       }

    auto config_map = getRunOptionMap();
    auto config_map_luminosity = getLuminosityOptionMap();
    std::unique_ptr<DialogRunAnalysis> dialog(new DialogRunAnalysis());
    dialog->setValues(cat,pdf,lik,pos,config_map,config_map_luminosity);
    if (dialog->exec()) {
      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
          "IGM",ui->cb_igm->currentText().toStdString());

      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
          "Collapse",ui->cb_marginalization->currentText().toStdString());

      if (ui->gb_corrections->isChecked ()) {
        PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
            "Correction",ui->cb_AnalysisCorrection->currentText().toStdString());
      } else {
        PreferencesUtils::clearUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                    "Correction");
      }
    }
  }

  void FormAnalysis::setToolBoxButtonColor(QToolBox* toolBox, int index, QColor color)
  {
    int i = 0;
    foreach (QAbstractButton* button, toolBox->findChildren<QAbstractButton*>())
    {
      // make sure only toolbox button palettes are modified
      if (button->metaObject()->className() == QString("QToolBoxButton"))
      {
        if (i == index)
        {
          // found correct button
          QPalette p = button->palette();
          p.setColor(QPalette::ButtonText, color);
          button->setPalette(p);
          break;
        }
        i++;
      }
    }
  }



}
}
