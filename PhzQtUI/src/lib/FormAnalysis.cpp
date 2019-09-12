#include <QStandardItemModel>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <functional>
#include <QFileDialog>
#include <boost/program_options.hpp>


#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

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
#include "PhzQtUI/DialogGalCorrGridGeneration.h"
#include "PhzQtUI/DialogRunAnalysis.h"
#include "PhzQtUI/DialogAddGalEbv.h"
#include "PhzQtUI/DialogLuminosityPrior.h"
#include "PhzQtUI/DialogOutputColumnSelection.h"

#include "PhzUITools/ConfigurationWriter.h"
#include "PhzUITools/CatalogColumnReader.h"

#include "PhzDataModel/PhzModel.h"
#include <ctime>

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("FormAnalysis");

FormAnalysis::FormAnalysis(QWidget *parent) :
    QWidget(parent), ui(new Ui::FormAnalysis) {
  ui->setupUi(this);
}

FormAnalysis::~FormAnalysis() {
}

///////////////////////////////////////////////////
//  Initial data load
void FormAnalysis::loadAnalysisPage(
    std::shared_ptr<SurveyModel> survey_model_ptr,
    std::shared_ptr<ModelSetModel> model_set_model_ptr,
    DatasetRepo filter_repository,
    DatasetRepo luminosity_repository) {
  m_survey_model_ptr = survey_model_ptr;
  m_model_set_model_ptr = model_set_model_ptr;
  m_filter_repository = filter_repository;
  m_luminosity_repository = luminosity_repository;
  logger.info()<< "Load the Analysis Page";

  updateSelection();

  //
  ui->cb_z_col->clear();
  ui->cb_z_col->addItem("");
}


void FormAnalysis::updateSelection() {
  // Disconnect the combobox event
  disconnect(ui->cb_AnalysisModel, SIGNAL(currentIndexChanged(const QString &)), 0, 0);

  // if needed: Fill the Parameter Space Combobox and set its index
  if (m_model_set_model_ptr->doNeedReload()){
    ui->cb_AnalysisModel->clear();
    logger.info()<< "Found "<<m_model_set_model_ptr->getModelSetList().size() <<" Parameter Space in the provider";
    for (auto& model_name : m_model_set_model_ptr->getModelSetList()) {
        ui->cb_AnalysisModel->addItem(model_name);
    }
  }

  // select the right item

  if(m_model_set_model_ptr->doNeedReload() || ui->cb_AnalysisModel->currentText().toStdString() != m_model_set_model_ptr->getSelectedModelSet().getName()){
    bool found = false;
    for (int i = 0; i < ui->cb_AnalysisModel->count(); i++) {
      if (ui->cb_AnalysisModel->itemText(i).toStdString() == m_model_set_model_ptr->getSelectedModelSet().getName()) {
        ui->cb_AnalysisModel->setCurrentIndex(i);
        on_cb_AnalysisModel_currentIndexChanged(ui->cb_AnalysisModel->itemText(i));
        found=true;
        break;
      }
    }

    if (!found && ui->cb_AnalysisModel->count()>0){
       ui->cb_AnalysisModel->setCurrentIndex(0);
       on_cb_AnalysisModel_currentIndexChanged(ui->cb_AnalysisModel->itemText(0));
    }
  }

  m_model_set_model_ptr->reloaded();

  // reconnect the combobox event
  connect(ui->cb_AnalysisModel, SIGNAL(currentIndexChanged(const QString &)),
         SLOT(on_cb_AnalysisModel_currentIndexChanged(const QString &)));


  // Disconnect the combobox event

  disconnect(ui->cb_AnalysisSurvey, SIGNAL(currentIndexChanged(const QString &)), 0, 0);
  // if needed: Fill the Parameter Space Combobox and set its index
  if (m_survey_model_ptr->doNeedReload()){
    ui->cb_AnalysisSurvey->clear();
    logger.info()<< "Found "<<m_survey_model_ptr->getSurveyList().size() <<" Catalogs types in the provider";
    for (auto& survey_name : m_survey_model_ptr->getSurveyList()) {
       ui->cb_AnalysisSurvey->addItem(survey_name);
    }
  }

  auto saved_catalog = m_survey_model_ptr->getSelectedSurvey();
  if(m_survey_model_ptr->doNeedReload() || ui->cb_AnalysisSurvey->currentText().toStdString() != saved_catalog.getName()){
    // select the right item
    bool found = false;
    for (int i = 0; i < ui->cb_AnalysisSurvey->count(); i++) {
      if (ui->cb_AnalysisSurvey->itemText(i).toStdString() == saved_catalog.getName()) {
        ui->cb_AnalysisSurvey->setCurrentIndex(i);
        on_cb_AnalysisSurvey_currentIndexChanged(ui->cb_AnalysisSurvey->itemText(i));
        found=true;
        break;
      }
    }

    if (!found && ui->cb_AnalysisSurvey->count()>0){
      ui->cb_AnalysisSurvey->setCurrentIndex(0);
      on_cb_AnalysisSurvey_currentIndexChanged(ui->cb_AnalysisSurvey->itemText(0));
    }
  }

  m_survey_model_ptr->reloaded();
  // reconnect the combobox event
  connect(ui->cb_AnalysisSurvey, SIGNAL(currentIndexChanged(const QString &)),
        SLOT(on_cb_AnalysisSurvey_currentIndexChanged(const QString &)));

}

///////////////////////////////////////////////////
//  Handle controls enability

void FormAnalysis::updateGridSelection() {
try {
  auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();

  auto axis = selected_model.getAxesTuple();
  auto possible_files = PhzGridInfoHandler::getCompatibleGridFile(
      m_survey_model_ptr->getSelectedSurvey().getName(), axis,
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
} catch (Elements::Exception&) {
  if (ui->cb_AnalysisModel->currentIndex() > -1) {
    ui->cb_AnalysisModel->removeItem(ui->cb_AnalysisModel->currentIndex());
  }
  }
}

void FormAnalysis::updateGalCorrGridSelection() {
  try {
    auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();


    auto axis = selected_model.getAxesTuple();
    auto possible_files = PhzGridInfoHandler::getCompatibleGridFile(
        m_survey_model_ptr->getSelectedSurvey().getName(), axis,
        getSelectedFilters(), ui->cb_igm->currentText().toStdString(),
        false);

    ui->cb_CompatibleGalCorrGrid->clear();
    bool added = false;
    for (auto& file : possible_files) {
      ui->cb_CompatibleGalCorrGrid->addItem(QString::fromStdString(file));
      added = true;
    }

    if (!added) {
      ui->cb_CompatibleGalCorrGrid->addItem(
          QString::fromStdString("Grid_" + selected_model.getName() + "_")
              + ui->cb_igm->currentText() + "_MW_Param.dat");
    }

    ui->cb_CompatibleGalCorrGrid->addItem("<Enter a new name>");
  } catch (Elements::Exception&) {}
}

void FormAnalysis::fillCbColumns(std::set<std::string> columns) {
   ui->cb_z_col->clear();
   ui->cb_z_col->addItem("");
   for (auto item : columns) {
     ui->cb_z_col->addItem(QString::fromStdString(item));
   }

   ui->cb_z_col->setEnabled(true);
}

void FormAnalysis::updateCorrectionSelection() {
  auto filter_map = getSelectedFilters();
  auto file_list = PhotometricCorrectionHandler::getCompatibleCorrectionFiles(
      m_survey_model_ptr->getSelectedSurvey().getName(), filter_map);
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
    setToolBoxButtonColor(ui->toolBox, 0, Qt::black);
  }
  ui->btn_GetConfigGrid->setToolTip(tool_tip);
  ui->btn_RunGrid->setToolTip(tool_tip);

}

void FormAnalysis::adjustGalCorrGridButtons(bool enabled) {
  int toolbox_index = 1;
  bool name_ok = checkGalacticGridSelection(false, true);
  bool valid_model_grid = checkGridSelection(true, false);
  bool needed = !ui->rb_gc_off->isChecked();
  ui->cb_CompatibleGalCorrGrid->setEnabled(needed && enabled);
  ui->btn_GetGalCorrConfigGrid->setEnabled(needed && enabled && name_ok);
  ui->btn_RunGalCorrGrid->setEnabled(needed && enabled && valid_model_grid && name_ok);
  QString tool_tip = "";
  if (!needed) {
    ui->cb_CompatibleGalCorrGrid->lineEdit()->setStyleSheet(
           "QLineEdit { color: black }");
       setToolBoxButtonColor(ui->toolBox, toolbox_index, Qt::black);
       tool_tip =
              "With 'Correction type' set to 'OFF' there is no need to generate this grid.";
  } else if (!name_ok) {
    ui->cb_CompatibleGalCorrGrid->lineEdit()->setStyleSheet(
        "QLineEdit { color: red }");
    setToolBoxButtonColor(ui->toolBox, toolbox_index, Qt::red);
    tool_tip =
        "Please enter a valid grid name in order to compute the Grid or export the corresponding configuration.";
  } else if (!valid_model_grid) {
    ui->cb_CompatibleGalCorrGrid->lineEdit()->setStyleSheet(
        "QLineEdit { color: red }");
    setToolBoxButtonColor(ui->toolBox, toolbox_index, Qt::red);
    tool_tip =
        "You should first generate the Model Grid before the Galactic Correction one.";
  } else if (!checkGalacticGridSelection(true, false)) {
    ui->cb_CompatibleGalCorrGrid->lineEdit()->setStyleSheet(
        "QLineEdit { color: orange }");
    setToolBoxButtonColor(ui->toolBox, toolbox_index, QColor("orange"));

  } else {
    ui->cb_CompatibleGalCorrGrid->lineEdit()->setStyleSheet(
        "QLineEdit { color: black }");
    setToolBoxButtonColor(ui->toolBox, toolbox_index, Qt::black);
  }
  ui->btn_GetGalCorrConfigGrid->setToolTip(tool_tip);
  ui->btn_RunGalCorrGrid->setToolTip(tool_tip);

}

void FormAnalysis::setComputeCorrectionEnable() {
  bool name_exists = checkGridSelection(true, false);
  bool gal_corr_needed = !ui->rb_gc_off->isChecked();
  bool grid_gal_corr_name_exists = checkGalacticGridSelection(true, false);
  ui->btn_computeCorrections->setEnabled(
      name_exists &&
      (!gal_corr_needed || grid_gal_corr_name_exists) &&
      ui->gb_corrections->isChecked());

  QString tool_tip = "Open the photometric zero-point correction popup.";

  if (gal_corr_needed && !grid_gal_corr_name_exists) {
      tool_tip =
          "Please run the Galactic Correction grid computation before computing the photometric corrections.";
  }

  if (!name_exists) {
      tool_tip =
          "Please run the photometric grid computation before computing the photometric corrections.";
  }

  ui->btn_computeCorrections->setToolTip(tool_tip);
}

void FormAnalysis::setRunAnnalysisEnable(bool enabled) {

  bool grid_name_ok = checkGridSelection(false, true);
  bool grid_name_exists = checkGridSelection(true, false);


  bool need_gal_correction = !ui->rb_gc_off->isChecked();
  bool grid_gal_corr_name_ok = checkGalacticGridSelection(false, true);
  bool grid_gal_corr_name_exists = checkGalacticGridSelection(true, false);


  bool correction_ok = !ui->gb_corrections->isChecked()
      || ui->cb_AnalysisCorrection->currentText().toStdString().length() > 0;

  QFileInfo info(
      QString::fromStdString(
          FileUtils::getPhotCorrectionsRootPath(false,
            m_survey_model_ptr->getSelectedSurvey().getName()))
          + QDir::separator() + ui->cb_AnalysisCorrection->currentText());
  bool correction_exists = !ui->gb_corrections->isChecked() || info.exists();

  QFileInfo info_input(ui->txt_inputCatalog->text());
  bool run_ok = info_input.exists();


  bool lum_prior_ok = !ui->cb_luminosityPrior->isChecked() || ui->cb_luminosityPrior_2->currentText().length() > 0;
  bool lum_prior_compatible = true;
  if (lum_prior_ok && ui->cb_luminosityPrior->isChecked()) {
    LuminosityPriorConfig info;
    for (auto prior_pair : m_prior_config) {
      if (ui->cb_luminosityPrior_2->currentText().toStdString() == prior_pair.first) {
        info = prior_pair.second;
      }
    }

    auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();


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

    lum_prior_compatible = info.isCompatibleWithParameterSpace(z_min, z_max, selected_model.getSeds());

  }

  ui->btn_confLuminosityPrior->setEnabled(grid_name_exists);
  if (grid_name_exists) {
    ui->btn_confLuminosityPrior->setToolTip("Configure the Luminosity Prior");
  } else {
    ui->btn_confLuminosityPrior->setToolTip("You need to Generate the Model Grid before to configure the Luminosity Prior");
  }


  ui->btn_GetConfigAnalysis->setEnabled(
      grid_name_ok && (!need_gal_correction || grid_gal_corr_name_ok) &&
      correction_ok && lum_prior_ok && lum_prior_compatible && run_ok && enabled);
  ui->btn_RunAnalysis->setEnabled(
      grid_name_exists && (!need_gal_correction || grid_gal_corr_name_exists) &&
      correction_ok && correction_exists && lum_prior_ok && lum_prior_compatible&& run_ok && enabled);

  QString tool_tip_run = "";
  QString tool_tip_conf = "";
  if (!grid_name_ok) {
    tool_tip_conf = tool_tip_conf + "Please enter a valid model grid name. \n";
    tool_tip_run = tool_tip_run + "Please enter a valid model grid name. \n";
  }

  if (!grid_name_exists) {
    tool_tip_run = tool_tip_run + "Please run the model grid computation. \n";
  }

  if (need_gal_correction) {
      if (!grid_gal_corr_name_ok) {
        tool_tip_conf = tool_tip_conf +
            "You have enabled the Galactic Absorption Correction Please enter a valid Galactic correction grid name. \n";
        tool_tip_run = tool_tip_run +
            "You have enabled the Galactic Absorption Correction Please enter a valid Galactic correction grid name. \n";
      }

      if (!grid_gal_corr_name_exists) {
        tool_tip_run = tool_tip_run + "Please run the Galactic Absorption Correction grid computation. \n";
      }
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

  if (!lum_prior_ok) {
    tool_tip_conf = tool_tip_conf
                + "When the luminosity prior is enabled , you must provide a luminosity prior configuration. \n";
    tool_tip_run = tool_tip_run
                + "When the luminosity prior is enabled , you must provide a luminosity prior configuration. \n";
  }

  if (!lum_prior_compatible) {
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

  if (!(grid_name_ok &&
        (!need_gal_correction || grid_gal_corr_name_ok) &&
        correction_ok && lum_prior_ok && lum_prior_compatible && run_ok)) {
    tool_tip_conf = tool_tip_conf + "Before getting the configuration.";
  } else {
    tool_tip_conf = "Get the configuration file.";
  }

  if (!(grid_name_exists &&
        (!need_gal_correction || grid_gal_corr_name_exists)  &&
        correction_ok && correction_exists &&
        lum_prior_ok && lum_prior_compatible && run_ok)) {
    tool_tip_run = tool_tip_run + "Before running the analysis.";
  } else {
    tool_tip_run = "Run the analysis.";
  }

  ui->btn_GetConfigAnalysis->setToolTip(tool_tip_conf);
  ui->btn_RunAnalysis->setToolTip(tool_tip_run);

  if (!correction_ok) {
    setToolBoxButtonColor(ui->toolBox, 3, QColor("orange"));
  } else {
    setToolBoxButtonColor(ui->toolBox, 3, Qt::black);
  }

  if (!lum_prior_ok || !lum_prior_compatible) {
     setToolBoxButtonColor(ui->toolBox, 2, QColor("orange"));
   } else {
     setToolBoxButtonColor(ui->toolBox, 2, Qt::black);
   }
}




//////////////////////////////////////////////////
// Build and handle objects for calling the processing



std::list<std::string> FormAnalysis::getFilters() {
  std::list<std::string> res;
  auto survey = m_survey_model_ptr->getSelectedSurvey();
  for (auto filter : survey.getFilters()) {
     res.push_back(filter.getFilterFile());
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
  auto filter_mappings = m_survey_model_ptr->getSelectedSurvey().getFilters();
  std::list<FilterMapping> list;
  for (auto name : filterNames) {
    for (auto& filter : filter_mappings) {
      if (filter.getFilterFile().compare(name) == 0) {
        list.push_back(filter);
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

bool FormAnalysis::checkGalacticGridSelection(bool addFileCheck, bool acceptNewFile) {
  std::string file_name = ui->cb_CompatibleGalCorrGrid->currentText().toStdString();

  if (file_name.compare("<Enter a new name>") == 0) {
    return false;
  }

  if (file_name.compare("") == 0) {
      return false;
    }

  if (!addFileCheck) {
    return true;
  }

  QFileInfo info(
      QString::fromStdString(
          FileUtils::getGalacticCorrectionGridRootPath(true,
              ui->cb_AnalysisSurvey->currentText().toStdString()))
          + QDir::separator() + QString::fromStdString(file_name));

  return acceptNewFile || info.exists();
}

std::map<std::string, boost::program_options::variable_value> FormAnalysis::getGridConfiguration() {
  std::string file_name = FileUtils::addExt(
      ui->cb_CompatibleGrid->currentText().toStdString(), ".dat");
  ui->cb_CompatibleGrid->setItemText(ui->cb_CompatibleGrid->currentIndex(),
      QString::fromStdString(file_name));
  auto& selected_model =  m_model_set_model_ptr->getSelectedModelSet();

  return PhzGridInfoHandler::GetConfigurationMap(
      ui->cb_AnalysisSurvey->currentText().toStdString(), file_name, selected_model,
      getFilters(), ui->cb_igm->currentText().toStdString());
}



std::map<std::string, boost::program_options::variable_value> FormAnalysis::getGalacticCorrectionGridConfiguration() {
  std::string file_name = FileUtils::addExt(ui->cb_CompatibleGalCorrGrid->currentText().toStdString(), ".dat");
  ui->cb_CompatibleGalCorrGrid->setItemText(ui->cb_CompatibleGalCorrGrid->currentIndex(),
        QString::fromStdString(file_name));
  std::string grid_name = ui->cb_CompatibleGrid->currentText().toStdString();
  std::string catalog_type = ui->cb_AnalysisSurvey->currentText().toStdString();
  std::string igm = ui->cb_igm->currentText().toStdString();

  QFileInfo f99_curve_info(
                QString::fromStdString(FileUtils::getRedCurveRootPath(false))
                + QDir::separator() + QString::fromStdString("F99")
                + QDir::separator() + QString::fromStdString("F99_3.1.dat"));
    if (!f99_curve_info.exists()) {
      QMessageBox::warning(this, "Missing Reddening curve...",
                      "The Milky Way reddening curve stored by default in <ReddeningCurves>/F99/F99_3.1.dat is missing. "
                      "This computation need it, please provide it and try again.",
                      QMessageBox::Ok);
      return {};
    }

    std::map<std::string, boost::program_options::variable_value> options_map =
             FileUtils::getPathConfiguration(false, true, true, false);
    options_map["catalog-type"].value() = boost::any(catalog_type);
    options_map["output-galactic-correction-coefficient-grid"].value() = boost::any(file_name);
    options_map["model-grid-file"].value() = boost::any(grid_name);
    options_map["igm-absorption-type"].value() = boost::any(igm);


    std::string f99 = "F99/F99_3.1";
    options_map["milky-way-reddening-curve-name"].value() = boost::any(f99);
    auto global_options = PreferencesUtils::getThreadConfigurations();
    for (auto& pair : global_options) {
         options_map[pair.first] = pair.second;
    }



    return options_map;
}

std::map<std::string, boost::program_options::variable_value> FormAnalysis::getRunOptionMap() {

  auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();
  SurveyFilterMapping selected_survey = m_survey_model_ptr->getSelectedSurvey();
  double non_detection = selected_survey.getNonDetection();
  bool has_Missing_data = selected_survey.getHasMissingPhotometry();
  bool has_upper_limit = selected_survey.getHasUpperLimit();

  std::map<std::string, boost::program_options::variable_value> options_map =
      FileUtils::getPathConfiguration(true, false, true, true);

  auto global_options = PreferencesUtils::getThreadConfigurations();
  for (auto& pair : global_options) {
      options_map[pair.first] = pair.second;
  }

  global_options = PreferencesUtils::getCosmologyConfigurations();
  for (auto& pair : global_options) {
       options_map[pair.first] = pair.second;
  }

  options_map["catalog-type"].value() = boost::any(survey_name);


  options_map["phz-output-dir"].value() = boost::any(
      ui->txt_outputFolder->text().toStdString());

  options_map["model-grid-file"].value() = boost::any(ui->cb_CompatibleGrid->currentText().toStdString());

  if (ui->rb_gc_col->isChecked()) {
    options_map["galactic-correction-coefficient-grid-file"].value() =
        boost::any(ui->cb_CompatibleGalCorrGrid->currentText().toStdString());
    options_map["dust-column-density-column-name"].value() =
        boost::any(selected_survey.getGalEbvColumn());

    options_map["dust-map-sed-bpc"].value() = boost::any(1.0);

  } else if (ui->rb_gc_planck->isChecked()) {
    options_map["galactic-correction-coefficient-grid-file"].value() =
            boost::any(ui->cb_CompatibleGalCorrGrid->currentText().toStdString());
    std::string gal_ebv_default_column = "PLANK_GAL_EBV";
    options_map["dust-column-density-column-name"].value() = boost::any(gal_ebv_default_column);
  }

  auto input_catalog_file = FileUtils::removeStart(
      ui->txt_inputCatalog->text().toStdString(),
      FileUtils::getCatalogRootPath(false, survey_name) +
      QString(QDir::separator()).toStdString());
  options_map["input-catalog-file"].value() = boost::any(input_catalog_file);
  options_map["source-id-column-name"].value() = boost::any(m_survey_model_ptr->getSelectedSurvey().getSourceIdColumn());
  if (has_Missing_data) {
    options_map["missing-photometry-flag"].value() = boost::any(non_detection);
  }
  std::string yes_flag = "YES";
  std::string no_flag = "NO";

  if (has_upper_limit) {
    options_map["enable-upper-limit"].value() = boost::any(yes_flag);
  } else {
    options_map["enable-upper-limit"].value() = boost::any(no_flag);
  }


  auto filter_excluded = getExcludedFilters();
  if (filter_excluded.size() > 0) {
    std::vector<std::string> excluded;
    for (auto& filter : filter_excluded) {
      excluded.push_back(filter);
    }
    options_map["exclude-filter"].value() = boost::any(excluded);
  }


  if (ui->gb_corrections->isChecked()) {
    options_map["enable-photometric-correction"].value() = boost::any(yes_flag);
    options_map["photometric-correction-file"].value() = boost::any(
        ui->cb_AnalysisCorrection->currentText().toStdString());
  }

  options_map["axes-collapse-type"].value() = boost::any(
      ui->cb_marginalization->currentText().toStdString());

  options_map["likelihood-axes-collapse-type"].value() = boost::any(
        ui->cb_marginalization_likelihood->currentText().toStdString());


  options_map["output-catalog-format"].value() = boost::any(
        ui->cb_cat_output_type->currentText().toStdString());

  if (ui->gb_fix_z->isChecked()) {
    options_map["fixed-redshift-column"].value() = boost::any(ui->cb_z_col->currentText().toStdString());
  }

  if (ui->cb_best_model_cols->isChecked()) {
     options_map["create-output-best-model"].value() = boost::any(yes_flag);
   } else {
     options_map["create-output-best-model"].value() = boost::any(no_flag);
   }

  if (ui->cb_best_likelihood->isChecked()) {
     options_map["create-output-best-likelihood-model"].value() = boost::any(yes_flag);
   } else {
     options_map["create-output-best-likelihood-model"].value() = boost::any(no_flag);
   }

  if (ui->cb_normalize_pdf->isChecked()) {
    options_map["output-pdf-normalized"].value() = boost::any(yes_flag);
      } else {
        options_map["output-pdf-normalized"].value() = boost::any(no_flag);
      }

  if (ui->cb_gen_likelihood->isChecked()) {
   options_map["create-output-likelihoods"].value() = boost::any(yes_flag);
  }

  if (ui->cb_gen_posterior->isChecked()) {
    options_map["create-output-posteriors"].value() = boost::any(yes_flag);
  }


  if (ui->cb_luminosityPrior->isChecked()) {
    std::string lum_prior_name = ui->cb_luminosityPrior_2->currentText().toStdString();
    auto& lum_prior_config = m_prior_config.at(lum_prior_name);
    auto lum_prior_option = lum_prior_config.getConfigOptions();
    options_map.insert(lum_prior_option.begin(), lum_prior_option.end());
    options_map["luminosity-prior-effectiveness"].value() = boost::any(ui->dsp_eff_lum->value());
  }

  if (ui->cb_volumePrior->isChecked()) {
    options_map["volume-prior"].value() = boost::any(yes_flag);
    options_map["volume-prior-effectiveness"].value() = boost::any(ui->dsp_eff_vol->value());
  }


  std::string pdf_output_type = "VECTOR-COLUMN";
  if (ui->cbb_pdf_out->currentIndex() == 1) {
    pdf_output_type = "INDIVIDUAL-HDUS";
  }
  options_map["output-pdf-format"].value() = boost::any(pdf_output_type);

  std::vector<std::string> pdf_output_axis{};
  if (ui->cb_pdf_z->isChecked()) {
    pdf_output_axis.push_back("Z");
  }

  if (ui->cb_pdf_ebv->isChecked()) {
    pdf_output_axis.push_back("EBV");
  }

  if (ui->cb_pdf_red->isChecked()) {
    pdf_output_axis.push_back("REDDENING-CURVE");
  }

  if (ui->cb_pdf_sed->isChecked()) {
      pdf_output_axis.push_back("SED");
  }

  if (pdf_output_axis.size() > 0) {
    options_map["create-output-pdf"].value() = boost::any(pdf_output_axis);
  }

  std::vector<std::string> likelihood_pdf_output_axis{};
   if (ui->cb_likelihood_pdf_z->isChecked()) {
     likelihood_pdf_output_axis.push_back("Z");
   }

   if (ui->cb_likelihood_pdf_ebv->isChecked()) {
     likelihood_pdf_output_axis.push_back("EBV");
   }

   if (ui->cb_likelihood_pdf_red->isChecked()) {
     likelihood_pdf_output_axis.push_back("REDDENING-CURVE");
   }

   if (ui->cb_likelihood_pdf_sed->isChecked()) {
     likelihood_pdf_output_axis.push_back("SED");
   }

   if (likelihood_pdf_output_axis.size() > 0) {
     options_map["create-output-likelihood-pdf"].value() = boost::any(likelihood_pdf_output_axis);
   }


  if (m_copied_columns.size() > 0) {
    std::string option ="";
    bool first = true;
    for (auto& copied_column : m_copied_columns) {
      if (!first) {
        option = option+",";
      } else {
        first = false;
      }
      option = option+copied_column.first;
      if (copied_column.second != "") {
        option = option+":"+copied_column.second;
      }
    }

    options_map["copy-columns"].value() = boost::any(option);
  }

  return options_map;
}



std::map < std::string, boost::program_options::variable_value > FormAnalysis::getLuminosityOptionMap() {
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
    for (auto& pair : global_options) {
          options_map[pair.first] = pair.second;
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


//  1. Survey and Model
void FormAnalysis::on_cb_AnalysisSurvey_currentIndexChanged(
    const QString &selectedName) {

  logger.info()<< "The selected index of the Catalog ComboBox has changed. New selected item:"<<selectedName.toStdString();
  m_survey_model_ptr->selectSurvey(selectedName);

  SurveyFilterMapping selected_survey = m_survey_model_ptr->getSelectedSurvey();

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

  // Check which of the Galactic correction option  are available
  ui->rb_gc_off->setChecked(true);
  ui->rb_gc_col->setChecked(false);
  ui->rb_gc_col->setEnabled(selected_survey.getGalEbvColumn().length() > 0);
  ui->rb_gc_planck->setChecked(false);
  ui->rb_gc_planck->setEnabled(selected_survey.getRaColumn().length() > 0 && selected_survey.getDecColumn().length() > 0);

  auto saved_gal_type = PreferencesUtils::getUserPreference(selected_survey.getName(),
        "GalCorrType");
  if (saved_gal_type == "COL" && ui->rb_gc_col->isEnabled()) {
    ui->rb_gc_col->setChecked(true);
  } else if (saved_gal_type == "MAP" && ui->rb_gc_planck->isEnabled()) {
    ui->rb_gc_planck->setChecked(true);
  }

  // update the column of the default catalog file
  fillCbColumns(selected_survey.getColumnList());

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
  updateGalCorrGridSelection();
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

  setCopiedColumns(selected_survey.getCopiedColumns());
}

template<typename ReturnType, int I>
 int countCompleteList(const std::map<std::string, PhzDataModel::ModelAxesTuple>& grid_axis_map) {
   std::vector<ReturnType> all_item { };
   for (auto& sub_grid : grid_axis_map) {
     for (auto& item : std::get<I>(sub_grid.second)) {
       if (std::find(all_item.begin(), all_item.end(), item) == all_item.end())
         all_item.push_back(item);
     }
   }

   return all_item.size();
 }


  void FormAnalysis::on_cb_AnalysisModel_currentIndexChanged(const QString &model_name) {
    logger.info()<< "The selected index of the Parameter Space ComboBox has changed. New selected item:"<<model_name.toStdString();
    m_model_set_model_ptr->selectModelSet(model_name);

    updateGridSelection();
    updateGalCorrGridSelection();
    loadLuminosityPriors();
  }

  void FormAnalysis::on_cb_igm_currentIndexChanged(const QString &) {
    updateGridSelection();
    updateGalCorrGridSelection();
  }

  void FormAnalysis::onFilterSelectionItemChanged(QStandardItem*) {
    updateGridSelection();
    updateGalCorrGridSelection();
    updateCorrectionSelection();
  }

//  2. Photometry Grid
  void FormAnalysis::on_cb_CompatibleGrid_currentTextChanged(const QString &) {
    adjustPhzGridButtons(true);
    std::string grid_name = ui->cb_CompatibleGrid->currentText().toStdString();
    size_t index = grid_name.find_last_of("/\\");
    if (index != string::npos) {
      grid_name = grid_name.substr(index+1);
    }

    index = grid_name.find_last_of(".");
        if (index != string::npos) {
          grid_name = grid_name.substr(0, index);
        }
     ui->cb_CompatibleGalCorrGrid->setItemText(ui->cb_CompatibleGalCorrGrid->currentIndex(),
                QString::fromStdString(grid_name+"_MW_Param.dat"));
    adjustGalCorrGridButtons(true);
    setComputeCorrectionEnable();
    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_btn_GetConfigGrid_clicked() {
    if (!checkGridSelection(true, true)) {
      QMessageBox::warning(this, "Unavailable name...",
          "It is not possible to save the Grid under the name you have provided. Please enter a new name.",
          QMessageBox::Ok);
    } else {
      QString filter = "Config (*.conf)";
      QString fileName = QFileDialog::getSaveFileName(this,
          tr("Save Configuration File"),
          QString::fromStdString(FileUtils::getRootPath(true))+"config",
          filter, &filter);
      if (fileName.length() > 0) {
        if (!fileName.endsWith(".conf", Qt::CaseInsensitive)) {
          fileName = fileName+".conf";
        }
        auto config_map = getGridConfiguration();
        PhzUITools::ConfigurationWriter::writeConfiguration(config_map, fileName.toStdString());

        PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
            "IGM", ui->cb_igm->currentText().toStdString());

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
        adjustGalCorrGridButtons(true);
        setComputeCorrectionEnable();

        PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
            "IGM", ui->cb_igm->currentText().toStdString());
      }
    }
    setRunAnnalysisEnable(true);
  }

// Galactic Correction

  void FormAnalysis::on_rb_gc_off_clicked() {
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "GalCorrType", "OFF");
    adjustGalCorrGridButtons(true);
    setRunAnnalysisEnable(true);
  }
  void FormAnalysis::on_rb_gc_col_clicked() {
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "GalCorrType", "COL");
    adjustGalCorrGridButtons(true);
    setRunAnnalysisEnable(true);
  }
  void FormAnalysis::on_rb_gc_planck_clicked() {
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "GalCorrType", "MAP");
    adjustGalCorrGridButtons(true);
    setRunAnnalysisEnable(true);
  }


  void FormAnalysis::on_cb_CompatibleGalCorrGrid_textChanged(const QString &) {
    adjustGalCorrGridButtons(true);
    setComputeCorrectionEnable();
    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_btn_GetGalCorrConfigGrid_clicked() {
    if (!checkGalacticGridSelection(true, true)) {
      QMessageBox::warning(this, "Unavailable name...",
            "It is not possible to save the Galactic Correction Grid under the name you have provided. Please enter a new name.",
            QMessageBox::Ok);
    } else {
      QString filter = "Config (*.conf)";
      QString fileName = QFileDialog::getSaveFileName(this,
          tr("Save Configuration File"),
          QString::fromStdString(FileUtils::getRootPath(true))+"config",
          filter, &filter);
      if (fileName.length() > 0) {
        if (!fileName.endsWith(".conf", Qt::CaseInsensitive)) {
          fileName = fileName + ".conf";
        }
        auto config_map = getGalacticCorrectionGridConfiguration();
        if (config_map.size() > 0) {
          PhzUITools::ConfigurationWriter::writeConfiguration(config_map, fileName.toStdString());
        }
      }
    }
  }

  void FormAnalysis::on_btn_RunGalCorrGrid_clicked() {
    if (!checkGalacticGridSelection(true, true)) {
        QMessageBox::warning(this, "Unavailable name...",
              "It is not possible to save the Galactic Correction Grid under the name you have provided. Please enter a new name.",
              QMessageBox::Ok);
      } else {
        if (checkGalacticGridSelection(true, false)) {
              if (QMessageBox::warning(this, "Override existing file...",
                      "A Galactic Correction Grid file with the very same name as the one you provided already exist. "
                      "Do you want to replace it?", QMessageBox::Yes | QMessageBox::No)
                  == QMessageBox::No) {
                return;
              }
            }

          auto config_map = getGalacticCorrectionGridConfiguration();
          if (config_map.size() > 0) {
            std::unique_ptr<DialogGalCorrGridGeneration> dialog(new DialogGalCorrGridGeneration());
                 dialog->setValues(ui->cb_CompatibleGalCorrGrid->currentText().toStdString(), config_map);
                 if (dialog->exec()) {
                   adjustGalCorrGridButtons(true);
                   setComputeCorrectionEnable();

                 }
          }
      }
      setRunAnnalysisEnable(true);
  }

//  3. Photometric Correction
  void FormAnalysis::on_gb_corrections_clicked() {
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

    SurveyFilterMapping selected_survey = m_survey_model_ptr->getSelectedSurvey();

    auto config_map = getRunOptionMap();


    if (ui->rb_gc_planck->isChecked()) {
      auto path = ui->txt_inputCatalog->text().toStdString();
      auto column_reader = PhzUITools::CatalogColumnReader(path);
      auto column_from_file = column_reader.getColumnNames();
      if (column_from_file.find("PLANK_GAL_EBV") == column_from_file.end()) {
        // the E(B-V) has to be looked up in the Planck map
        SurveyFilterMapping selected_survey = m_survey_model_ptr->getSelectedSurvey();
        std::unique_ptr<DialogAddGalEbv> dialog(new DialogAddGalEbv());
        dialog->setInputs(path, selected_survey.getRaColumn(), selected_survey.getDecColumn());
        if (dialog->exec()) {
         // new catalog contains the PLANK_GAL_EBV column

         auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();
         auto input_catalog_file = FileUtils::removeStart(dialog->getOutputName(),
                     FileUtils::getCatalogRootPath(false, survey_name) +
                     QString(QDir::separator()).toStdString());

         config_map["input-catalog-file"].value() = boost::any(input_catalog_file);
        } else {
         // user has canceled the operation
         return;
        }
      }
    }





    std::unique_ptr<DialogPhotometricCorrectionComputation> popup(new DialogPhotometricCorrectionComputation());
    popup->setData(survey_name, m_survey_model_ptr->getSelectedSurvey().getSourceIdColumn(),
        ui->cb_AnalysisModel->currentText().toStdString(),
        ui->cb_CompatibleGrid->currentText().toStdString(),
        getSelectedFilterMapping(),
        getExcludedFilters(),
        selected_survey.getDefaultCatalogFile(),
        config_map,
        selected_survey.getNonDetection());

    connect(popup.get(), SIGNAL(correctionComputed(const QString &)),
        SLOT(onCorrectionComputed(const QString &)));
    popup->exec();

  }



  void FormAnalysis::onCorrectionComputed(const QString & new_file_name) {
    updateCorrectionSelection();
    ui->cb_AnalysisCorrection->setCurrentIndex(ui->cb_AnalysisCorrection->findText(new_file_name));
  }



// 4. algorithm options
  void FormAnalysis::on_btn_confLuminosityPrior_clicked() {
        std::unique_ptr<DialogLuminosityPrior> dialog(new DialogLuminosityPrior(m_filter_repository, m_luminosity_repository));

        std::string model_grid = ui->cb_CompatibleGrid->currentText().toStdString();

        auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

        auto& selected_model =  m_model_set_model_ptr->getSelectedModelSet();

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

        dialog->loadData(selected_model, survey_name, model_grid, z_min, z_max);
        dialog->exec();
        loadLuminosityPriors();

  }

  void FormAnalysis::loadLuminosityPriors() {
    auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

    auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();

    auto folder = FileUtils::getGUILuminosityPriorConfig(true, survey_name,
        selected_model.getName());

    m_prior_config = LuminosityPriorConfig::readFolder(folder);

    auto prior_name = PreferencesUtils::getUserPreference(survey_name,
        selected_model.getName() + "_LuminosityPriorName");


    ui->cb_luminosityPrior_2->clear();
    int index = -1;
    int id = 0;
    for (auto prior_pair : m_prior_config) {
      ui->cb_luminosityPrior_2->addItem(QString::fromStdString(prior_pair.first));

      if (prior_name == prior_pair.first) {
        index = id;
      }
      ++id;
    }


    if (index >= 0) {
      ui->cb_luminosityPrior_2->setCurrentIndex(index);
    }

    auto luminosity_prior_enabled = PreferencesUtils::getUserPreference(
           ui->cb_AnalysisSurvey->currentText().toStdString(),
           ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityPriorEnabled");

    ui->cb_luminosityPrior->setChecked(luminosity_prior_enabled == "1");

    auto volume_prior_enabled = PreferencesUtils::getUserPreference(
           ui->cb_AnalysisSurvey->currentText().toStdString(),
           ui->cb_AnalysisModel->currentText().toStdString() + "_VolumePriorEnabled");

    ui->cb_volumePrior->setChecked(volume_prior_enabled == "1");
  }


  void FormAnalysis::on_cb_luminosityPrior_2_currentIndexChanged(const QString &) {
    PreferencesUtils::setUserPreference(
        ui->cb_AnalysisSurvey->currentText().toStdString(),
        ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityPriorName",
        ui->cb_luminosityPrior_2->currentText().toStdString());

    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_cb_luminosityPrior_stateChanged(int) {
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                    ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityPriorEnabled",
                    QString::number(ui->cb_luminosityPrior->isChecked()).toStdString());
    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_cb_volumePrior_stateChanged(int) {
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                    ui->cb_AnalysisModel->currentText().toStdString() + "_VolumePriorEnabled",
                    QString::number(ui->cb_volumePrior->isChecked()).toStdString());
  }


// 5. Run
void FormAnalysis::setInputCatalogName(std::string name, bool do_test) {
  if (do_test) {
    std::vector<std::string> needed_columns { };

    needed_columns.push_back(m_survey_model_ptr->getSelectedSurvey().getSourceIdColumn());
    for (auto& filter : getSelectedFilterMapping()) {
      needed_columns.push_back(filter.getFluxColumn());
      needed_columns.push_back(filter.getErrorColumn());
    }

    std::string missing = FileUtils::checkFileColumns(name, needed_columns);
    if (missing.size() > 0) {
      if (QMessageBox::question(this, "Incompatible Data...",
          "The catalog file you selected has not the columns described into the Catalog and therefore cannot be used. \n"
              "Missing column(s):" + QString::fromStdString(missing) + "\n"
              "Do you want to create a new Catalog mapping for this file?",
          QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Ok) {
        navigateToNewCatalog(name);
      }
      return;
    }
  }

  ui->txt_inputCatalog->setText(QString::fromStdString(name));

  auto column_reader = PhzUITools::CatalogColumnReader(ui->txt_inputCatalog->text().toStdString());
  auto col_set = column_reader.getColumnNames();
  std::list<std::string> all_columns(col_set.begin(), col_set.end());
  updateCopiedColumns(all_columns);

  QFileInfo info(QString::fromStdString(name));
  ui->txt_outputFolder->setText(
      QString::fromStdString(
          FileUtils::getResultRootPath(false,
              ui->cb_AnalysisSurvey->currentText().toStdString(),
              info.baseName().toStdString())));
}

  void FormAnalysis::on_btn_BrowseInput_clicked() {
    QFileDialog dialog(this);
    std::string path = ui->txt_inputCatalog->text().toStdString();

    dialog.selectFile(QString::fromStdString(path));
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
      setInputCatalogName(dialog.selectedFiles()[0].toStdString());
      setRunAnnalysisEnable(true);
    }
  }


  void FormAnalysis::updateCopiedColumns(std::list<std::string> new_columns) {
    // get the columns from the catalog
    setCopiedColumns(m_survey_model_ptr->getSelectedSurvey().getCopiedColumns());

    // ensure that they are in the selected file
    std::list<std::string> missing{};
    for (auto& iter : m_copied_columns) {
      if (std::find(new_columns.begin(), new_columns.end(), iter.first) == new_columns.end()) {
        missing.push_back(iter.first);
      }
    }

    for (auto& iter : missing) {
      auto it = m_copied_columns.find(iter);
      m_copied_columns.erase(it);
    }
  }


  void FormAnalysis::setCopiedColumns(std::map<std::string, std::string> columns) {
    m_copied_columns = columns;
    std::string copy = "Copy Columns (";
    std::string copy_2 = ")";

    ui->output_column_btn->setText(QString::fromStdString(copy) +
        QString::number(m_copied_columns.size()) +
        QString::fromStdString(copy_2));
  }

  void FormAnalysis::on_output_column_btn_clicked() {
    auto column_reader = PhzUITools::CatalogColumnReader(ui->txt_inputCatalog->text().toStdString());
    auto col_set = column_reader.getColumnNames();

    std::list<std::string> all_columns(col_set.begin(), col_set.end());
    std::string id_col = m_survey_model_ptr->getSelectedSurvey().getSourceIdColumn();
    std::unique_ptr<DialogOutputColumnSelection> popup(new DialogOutputColumnSelection(all_columns, id_col, m_copied_columns));

    connect(popup.get(), SIGNAL(selectedColumns(std::map<std::string, std::string>)),
          SLOT(setCopiedColumns(std::map<std::string, std::string>)));

    popup->exec();
  }


  void FormAnalysis::saveCopiedColumnToCatalog() {
    m_survey_model_ptr->setCopiedColumnsToSelected(m_copied_columns);
    m_survey_model_ptr->saveSelected();
  }

  void FormAnalysis::on_btn_BrowseOutput_clicked() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.selectFile(ui->txt_outputFolder->text());
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    dialog.setLabelText(QFileDialog::Accept, "Select");
    if (dialog.exec()) {
      ui->txt_outputFolder->setText(dialog.selectedFiles()[0]);
      setRunAnnalysisEnable(true);
    }
  }


  void FormAnalysis::on_cbb_pdf_out_currentIndexChanged(const QString &) {

  }

  void FormAnalysis::on_cb_pdf_z_stateChanged(int) {

  }

  void FormAnalysis::on_cb_likelihood_pdf_z_stateChanged(int) {

  }


  void FormAnalysis::on_cb_gen_likelihood_clicked() {
    if (ui->cb_gen_likelihood->isChecked()) {
          QMessageBox::warning(this, "Large output volume...",
              "Outputing multi-dimensional likelihood grids (one file per source)"
              " will generate a large output volume.", QMessageBox::Ok);
        }
        setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_cb_gen_posterior_clicked() {
    if (ui->cb_gen_posterior->isChecked()) {
      QMessageBox::warning(this, "Large output volume...",
          "Outputing multi-dimensional posterior grids (one file per source)"
          " will generate a large output volume.", QMessageBox::Ok);
    }
    setRunAnnalysisEnable(true);
  }

  void FormAnalysis::on_btn_GetConfigAnalysis_clicked() {
    QString filter = "Config (*.conf)";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Configuration File"),
        QString::fromStdString(FileUtils::getRootPath(true))+"config",
        filter, &filter);
    if (fileName.length() > 0) {
      if (!fileName.endsWith(".conf", Qt::CaseInsensitive)) {
               fileName = fileName+".conf";
      }
      auto config_map = getRunOptionMap();
      PhzUITools::ConfigurationWriter::writeConfiguration(config_map, fileName.toStdString());
      saveCopiedColumnToCatalog();
      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
          "IGM", ui->cb_igm->currentText().toStdString());
      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
          "Collapse", ui->cb_marginalization->currentText().toStdString());
      if (ui->gb_corrections->isChecked()) {
        PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
            "Correction", ui->cb_AnalysisCorrection->currentText().toStdString());
      }
    }
  }

  void FormAnalysis::on_btn_RunAnalysis_clicked() {
    auto config_map = getRunOptionMap();
    if (ui->rb_gc_planck->isChecked()) {
      // User requests that we lookup Planck EBV from the position:
      // Check if the catalog contains the "GAL_EBV" column

      auto path = ui->txt_inputCatalog->text().toStdString();
      auto column_reader = PhzUITools::CatalogColumnReader(path);
      auto column_from_file = column_reader.getColumnNames();
      if (column_from_file.find("GAL_EBV") == column_from_file.end()) {
        // the E(B-V) has to be looked up in the Planck map
        SurveyFilterMapping selected_survey = m_survey_model_ptr->getSelectedSurvey();
        std::unique_ptr<DialogAddGalEbv> dialog(new DialogAddGalEbv());
        dialog->setInputs(path, selected_survey.getRaColumn(), selected_survey.getDecColumn());
        if (dialog->exec()) {
          // new catalog contains the GAL_EBV column

          auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();
          auto input_catalog_file = FileUtils::removeStart(dialog->getOutputName(),
                      FileUtils::getCatalogRootPath(false, survey_name) +
                      QString(QDir::separator()).toStdString());

          config_map["input-catalog-file"].value() = boost::any(input_catalog_file);
        } else {
          // user has canceled the operation
          return;
        }
      }
    }

    std::string out_dir = ui->txt_outputFolder->text().toStdString();

    QDir dir(QString::fromStdString(out_dir));
    if (dir.exists() && dir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() > 0) {
      if (QMessageBox::question(this, "Existing Output Folder...",
               "The Output Folder you selected already exists.\n"
                   "In order to avoid confusion, the Output Folder will be cleared. Do you want to proceed?",
               QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Ok) {
             FileUtils::removeDir(QString::fromStdString(out_dir));
      } else {
        return;
      }
    }


    auto config_map_luminosity = getLuminosityOptionMap();

    std::unique_ptr<DialogRunAnalysis> dialog(new DialogRunAnalysis());
    dialog->setValues(out_dir, config_map, config_map_luminosity);
    if (dialog->exec()) {
      saveCopiedColumnToCatalog();
      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
          "IGM", ui->cb_igm->currentText().toStdString());

      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
          "Collapse", ui->cb_marginalization->currentText().toStdString());

      if (ui->gb_corrections->isChecked()) {
        PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
            "Correction", ui->cb_AnalysisCorrection->currentText().toStdString());
      } else {
        PreferencesUtils::clearUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                    "Correction");
      }
    }
  }

  void FormAnalysis::setToolBoxButtonColor(QToolBox* toolBox, int index, QColor color) {
    int i = 0;
    foreach(QAbstractButton* button, toolBox->findChildren<QAbstractButton*>()) {
      // make sure only toolbox button palettes are modified
      if (button->metaObject()->className() == QString("QToolBoxButton")) {
        if (i == index) {
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

  void FormAnalysis::on_btn_ToModel_clicked() {
    navigateToParameter(false);
  }
  void FormAnalysis::on_btn_ToOption_clicked() {
    navigateToConfig();
  }
  void FormAnalysis::on_btn_ToCatalog_clicked() {
    navigateToCatalog(false);
  }
  void FormAnalysis::on_btn_exit_clicked() {
    quit(true);
  }

}  // namespace PhzQtUI
}  // namespace Euclid
