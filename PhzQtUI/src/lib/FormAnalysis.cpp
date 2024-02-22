#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTextStream>
#include <QUrl>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <functional>

#include <fstream>
#include <iostream>

#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"

#include "FileUtils.h"
#include "PhzQtUI/DialogAddGalEbv.h"
#include "PhzQtUI/DialogFilterShiftGridGeneration.h"
#include "PhzQtUI/DialogGalCorrGridGeneration.h"
#include "PhzQtUI/DialogGridGeneration.h"
#include "PhzQtUI/DialogLuminosityPrior.h"
#include "PhzQtUI/DialogNz.h"
#include "PhzQtUI/DialogOutputColumnSelection.h"
#include "PhzQtUI/DialogPhotCorrectionEdition.h"
#include "PhzQtUI/DialogPhotometricCorrectionComputation.h"
#include "PhzQtUI/DialogRunAnalysis.h"
#include "PhzQtUI/DialogZeroPointName.h"
#include "PhzQtUI/DialogExtractZ.h"
#include "PhzQtUI/FormAnalysis.h"
#include "PhzQtUI/ModelSet.h"
#include "PhzQtUI/PhotometricCorrectionHandler.h"
#include "PhzQtUI/PhzGridInfoHandler.h"
#include "PhzQtUI/SurveyFilterMapping.h"
#include "PreferencesUtils.h"
#include "ui_FormAnalysis.h"

#include "PhzQtUI/DialogSelectParam.h"
#include "PhzUITools/CatalogColumnReader.h"

#include "PhzDataModel/PhzModel.h"
#include "PhzUtils/Multithreading.h"
#include <ctime>

#include "Configuration/ConfigManager.h"
#include "DefaultOptionsCompleter.h"
#include "PhzConfiguration/ComputeSedWeightConfig.h"
#include "PhzExecutables/ComputeSedWeight.h"
#include "PhzDataModel/DoubleGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzDataModel/serialization/PhotometryGridInfo.h"
#include "PhzQtUI/DialogFilterSelector.h"
#include "PhzQtUI/SedParamUtils.h"
#include "PhzUITools/ConfigurationWriter.h"
#include "GridContainer/serialize.h"
#include <CCfits/CCfits>
#include <chrono>


namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("FormAnalysis");

FormAnalysis::FormAnalysis(QWidget* parent) : QWidget(parent), ui(new Ui::FormAnalysis) {
  ui->setupUi(this);

  m_planck_file = FileUtils::getAuxRootPath() + "/GalacticDustMap/PlanckEbv.fits";
}

FormAnalysis::~FormAnalysis() {}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////          Helper function        ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

static std::string getAxisDescription(const std::map<std::string, PhzDataModel::ModelAxesTuple>& axes) {
	std::string results = "";
	for (const auto& item : axes) {
		results+="Region name ='"+item.first+"', Card(Z) ="+ std::to_string(std::get<0>(item.second).size())
			                                    +", Card(EBV) ="+ std::to_string(std::get<1>(item.second).size())
			                                    +", Card(RedCurve) ="+ std::to_string(std::get<2>(item.second).size())
			                                    +", Card(SED) ="+ std::to_string(std::get<3>(item.second).size())
												+".\n";
	}
	return results;
}

void FormAnalysis::setToolBoxButtonColor(QToolBox* toolBox, int index, QColor color) {
  int i = 0;
  foreach (QAbstractButton* button, toolBox->findChildren<QAbstractButton*>()) {
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////          Setup and Data Loading         ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//  Initial data load
void FormAnalysis::loadAnalysisPage(std::shared_ptr<SurveyModel>   survey_model_ptr,
                                    std::shared_ptr<ModelSetModel> model_set_model_ptr,
									DatasetRepo sed_repository,
									DatasetRepo redenig_curves_repository,
									DatasetRepo filter_repository,
                                    DatasetRepo luminosity_repository) {
  m_survey_model_ptr      = survey_model_ptr;
  m_model_set_model_ptr   = model_set_model_ptr;
  m_sed_repository        = sed_repository;
  m_redenig_curves_repository = redenig_curves_repository;
  m_filter_repository     = filter_repository;
  m_luminosity_repository = luminosity_repository;

  updateSelection();

}

void FormAnalysis::updateSelection() {
  logger.debug() << "Load the Analysis Page";
  auto start = std::chrono::high_resolution_clock::now();
  /// COMBO BOX MODEL  ////
  ////////////////////////

  bool has_changed_model=false;
  // Disconnect the combobox event
  disconnect(ui->cb_AnalysisModel, SIGNAL(currentIndexChanged(const QString&)), 0, 0);
  // if needed: Fill the Parameter Space Combobox and set its index
  if (ui->cb_AnalysisModel->count()==0 || m_model_set_model_ptr->doNeedReload()) {
    ui->cb_AnalysisModel->clear();
    logger.info() << "Found " << m_model_set_model_ptr->getModelSetList().size() << " Parameter Space in the provider";
    for (auto& model_name : m_model_set_model_ptr->getModelSetList()) {
      ui->cb_AnalysisModel->addItem(model_name);
    }
    has_changed_model |= true;
  }
  // select the right item
  auto target_model_name = QString::fromStdString(m_model_set_model_ptr->getSelectedModelSet().getName());
  if (ui->cb_AnalysisModel->currentText() != target_model_name) {
	  has_changed_model |=  true;
    bool found = false;
    for (int i = 0; i < ui->cb_AnalysisModel->count(); i++) {
      if (ui->cb_AnalysisModel->itemText(i) == target_model_name) {
        ui->cb_AnalysisModel->setCurrentIndex(i);
        found               = true;
        break;
      }
    }

    if (!found && ui->cb_AnalysisModel->count() > 0) {
      ui->cb_AnalysisModel->setCurrentIndex(0);
    }
  }
  m_model_set_model_ptr->reloaded();
  // reconnect the combobox event
  connect(ui->cb_AnalysisModel, SIGNAL(currentIndexChanged(const QString&)),
          SLOT(on_cb_AnalysisModel_currentIndexChanged(const QString&)));

  /// COMBO BOX CATALOG  ////
  //////////////////////////

  bool has_changed_catalog=false;
  // Disconnect the combobox event
  disconnect(ui->cb_AnalysisSurvey, SIGNAL(currentIndexChanged(const QString&)), 0, 0);

  // if needed: Fill the Parameter Space Combobox and set its index
  if (ui->cb_AnalysisSurvey->count()==0 || m_survey_model_ptr->doNeedReload()) {
	has_changed_catalog |=  true;
    ui->cb_AnalysisSurvey->clear();
    logger.info() << "Found " << m_survey_model_ptr->getSurveyList().size() << " Catalogs types in the provider";
    for (auto& survey_name : m_survey_model_ptr->getSurveyList()) {
      ui->cb_AnalysisSurvey->addItem(survey_name);
    }
  }

  auto target_catalog_name = QString::fromStdString(m_survey_model_ptr->getSelectedSurvey().getName());
  if (ui->cb_AnalysisSurvey->currentText() != target_catalog_name) {
	 has_changed_catalog |=  true;
    // select the right item
    bool found = false;
    for (int i = 0; i < ui->cb_AnalysisSurvey->count(); i++) {
      if (ui->cb_AnalysisSurvey->itemText(i) == target_catalog_name) {
        ui->cb_AnalysisSurvey->setCurrentIndex(i);
        found                = true;
        break;
      }
    }

    if (!found && ui->cb_AnalysisSurvey->count() > 0) {
      ui->cb_AnalysisSurvey->setCurrentIndex(0);
    }
  }

  m_survey_model_ptr->reloaded();
  // reconnect the combobox event
  connect(ui->cb_AnalysisSurvey, SIGNAL(currentIndexChanged(const QString&)),
          SLOT(on_cb_AnalysisSurvey_currentIndexChanged(const QString&)));

  /// LUMINOSITY FILTER  ////
  //////////////////////////

  // set the Luminosity filter
  std::string lum_filter =
      PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                          ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityFilter");
  m_is_loading=true;
  if (lum_filter != "") {
	  setLumFilter(lum_filter);
  } else {
    // use first filter with r as default value (TODO update with full name);
    for (auto& filter_name : m_filter_repository->getContent()) {
      if (filter_name.datasetName().find("r") != std::string::npos) {
        setLumFilter(filter_name.qualifiedName());
        break;
      }
    }
  }
  m_is_loading=false;

  if (ui->cb_AnalysisSurvey->currentText() != "" && has_changed_catalog) {
    on_cb_AnalysisSurvey_currentIndexChanged(ui->cb_AnalysisSurvey->currentText());
  } else if (ui->cb_AnalysisModel->currentText() != "" && has_changed_model) {
    on_cb_AnalysisModel_currentIndexChanged(ui->cb_AnalysisModel->currentText());
}

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
  logger.info() << "Analysis Page Loaded (survey = "<<ui->cb_AnalysisSurvey->currentText().toStdString()<<
		            " changed="<<has_changed_catalog<<
					", model ="<<ui->cb_AnalysisModel->currentText().toStdString()<<" changed="<<has_changed_model<<")  "
					<< duration <<"[ms]";
}

///////////////////////////////////////////////////
//  Handle controls enability

void FormAnalysis::updateGridSelection() {
  auto start = std::chrono::high_resolution_clock::now();
  logger.info() << "Entering updateGridSelection";
  try {
    auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();

    auto axis = selected_model.getAxesTuple();
    logger.debug() << "updateGridSelection => selected_model content :" << getAxisDescription(axis);
    auto stop = std::chrono::high_resolution_clock::now();
	auto duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
	logger.debug()<<"updateGridSelection => get Axis "<< duration << "[ms]";
	start=stop;

    const auto& survey_name = m_survey_model_ptr->getSelectedSurvey().getName();
    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
    logger.debug()<<"updateGridSelection => Get name "<< duration << "[ms]";
    start=stop;

    auto igm =  ui->cb_igm->currentText().toStdString();
    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
    logger.debug()<<"updateGridSelection => Get IGM "<< duration << "[ms]";
    start=stop;

    auto lum_filter = ui->lbl_lum_filter->text().toStdString();
    logger.debug() << "Luminosity filter for the Model : " << lum_filter;

    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
    logger.debug()<<"updateGridSelection => Info collected "<< duration << "[ms]";
    start=stop;

    auto possible_files = PhzGridInfoHandler::getCompatibleGridFile(survey_name, axis, getSelectedFilters(), igm, lum_filter, PhotometryGrid);
    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
    logger.debug()<<"updateGridSelection => files loaded "<< duration << "[ms]";

   	m_is_loading=true;
    ui->cb_CompatibleGrid->clear();
    bool added = false;
    for (auto& file : possible_files) {
      ui->cb_CompatibleGrid->addItem(QString::fromStdString(file));
      added = true;
    }
    if (!added) {
      ui->cb_CompatibleGrid->addItem(QString::fromStdString("Grid_" + selected_model.getName() + "_") +
                                     ui->cb_igm->currentText() + ".txt");
    }
    ui->cb_CompatibleGrid->addItem("<Enter a new name>");
    m_is_loading=false;
    ui->cb_CompatibleGrid->setCurrentIndex(0);

  } catch (Elements::Exception&) {
    if (ui->cb_AnalysisModel->currentIndex() > -1) {
      ui->cb_AnalysisModel->removeItem(ui->cb_AnalysisModel->currentIndex());
    }
  }
  auto final_stop = std::chrono::high_resolution_clock::now();
  auto  final_duration=(std::chrono::duration_cast<std::chrono::microseconds>(final_stop - start)).count()/1000;
  logger.debug()<<"updateGridSelection completed "<< final_duration << "[ms]";

}



bool FormAnalysis::checkCompatibleModelGrid(std::string file_name) {
  auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();
  auto model_name = selected_model.getName();

  if (std::get<0>(m_cache_compatible_model_grid)==model_name && std::get<1>(m_cache_compatible_model_grid)==file_name){
	  return std::get<2>(m_cache_compatible_model_grid);
  }

  QFileInfo info(QString::fromStdString(
                     FileUtils::getPhotmetricGridRootPath(true, ui->cb_AnalysisSurvey->currentText().toStdString())) +
                 QDir::separator() + QString::fromStdString(file_name));

  if (!info.exists()) {
	m_cache_compatible_model_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, false};
    return false;
  } else {

    auto  axis           = selected_model.getAxesTuple();
    logger.debug() << "checkCompatibleModelGrid => selected_model content :" << getAxisDescription(axis);

    auto  possible_files = PhzGridInfoHandler::getCompatibleGridFile(
         m_survey_model_ptr->getSelectedSurvey().getName(), axis, getSelectedFilters(),
         ui->cb_igm->currentText().toStdString(), ui->lbl_lum_filter->text().toStdString(),
         GalacticReddeningCorrectionGrid);
    bool valid = (std::find(possible_files.begin(), possible_files.end(), file_name) != possible_files.end());
    m_cache_compatible_model_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, valid};
    return valid;
  }
}

bool FormAnalysis::checkCompatibleGalacticGrid(std::string file_name) {
  auto start = std::chrono::high_resolution_clock::now();
  logger.debug()<<"checkCompatibleGalacticGrid for file "<< file_name;
  auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();
  auto model_name = selected_model.getName();

  if (std::get<0>(m_cache_compatible_galactic_grid)==model_name && std::get<1>(m_cache_compatible_galactic_grid)==file_name){
 	return std::get<2>(m_cache_compatible_galactic_grid);
  }

  QFileInfo info(QString::fromStdString(FileUtils::getGalacticCorrectionGridRootPath(
                     true, ui->cb_AnalysisSurvey->currentText().toStdString())) +
                 QDir::separator() + QString::fromStdString(file_name));

  if (!info.exists()) {
	m_cache_compatible_galactic_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, false};
    return false;
  } else {
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
	logger.debug()<<"checkCompatibleGalacticGrid => No cache, need to actually check the file "<< duration << "[ms]";
	start = stop;

    auto  axis           = selected_model.getAxesTuple();
    logger.debug() << "checkCompatibleGalacticGrid => selected_model content :" << getAxisDescription(axis);
    auto  possible_files = PhzGridInfoHandler::getCompatibleGridFile(
         m_survey_model_ptr->getSelectedSurvey().getName(), axis, getSelectedFilters(),
         ui->cb_igm->currentText().toStdString(), ui->lbl_lum_filter->text().toStdString(),
         GalacticReddeningCorrectionGrid);
    stop = std::chrono::high_resolution_clock::now();
    duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
	logger.debug()<<"checkCompatibleGalacticGrid => Files checked "<<possible_files.size()<<" compatible files "<< duration << "[ms]";
	start = stop;
    bool valid = (std::find(possible_files.begin(), possible_files.end(), file_name) != possible_files.end());
    m_cache_compatible_galactic_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, valid};
    return valid;
  }
}


bool FormAnalysis::checkCompatibleFilterShiftGrid(std::string file_name) {
  auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();
  auto model_name = selected_model.getName();

  if (std::get<0>(m_cache_compatible_shift_grid)==model_name && std::get<1>(m_cache_compatible_shift_grid)==file_name){
 	return std::get<2>(m_cache_compatible_shift_grid);
  }

  QString full_name = QString::fromStdString(FileUtils::getFilterShiftGridRootPath(
                          true, ui->cb_AnalysisSurvey->currentText().toStdString())) +
                      QDir::separator() + QString::fromStdString(file_name);
  QFileInfo info(full_name);

  if (!info.exists()) {
	m_cache_compatible_shift_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, false};
    return false;
  } else {

    auto  axis           = selected_model.getAxesTuple();
    logger.debug() << "checkCompatibleFilterShiftGrid => selected_model content :" << getAxisDescription(axis);
    auto  possible_files = PhzGridInfoHandler::getCompatibleGridFile(
         m_survey_model_ptr->getSelectedSurvey().getName(), axis, getSelectedFilters(),
         ui->cb_igm->currentText().toStdString(), ui->lbl_lum_filter->text().toStdString(), FilterShiftCorrectionGrid);
    //  logger.debug()<< "checkCompatibleFilterShiftGrid : there are " << possible_files.size() << " compatible file ";
    bool valid = (std::find(possible_files.begin(), possible_files.end(), file_name) != possible_files.end());
    m_cache_compatible_shift_grid =  std::tuple<std::string, std::string, bool>{model_name, file_name, valid};
    return valid;
  }
}

void FormAnalysis::updateGalCorrGridSelection() {
  try {
    auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();

    auto axis           = selected_model.getAxesTuple();
    logger.debug() << "updateGalCorrGridSelection => selected_model content :" << getAxisDescription(axis);
    auto possible_files = PhzGridInfoHandler::getCompatibleGridFile(
        m_survey_model_ptr->getSelectedSurvey().getName(), axis, getSelectedFilters(),
        ui->cb_igm->currentText().toStdString(), ui->lbl_lum_filter->text().toStdString(),
        GalacticReddeningCorrectionGrid);

    ui->cb_CompatibleGalCorrGrid->clear();
    bool added = false;
    for (auto& file : possible_files) {
      ui->cb_CompatibleGalCorrGrid->addItem(QString::fromStdString(file));
      added = true;
    }

    if (!added) {
      ui->cb_CompatibleGalCorrGrid->addItem(QString::fromStdString("Grid_" + selected_model.getName() + "_") +
                                            ui->cb_igm->currentText() + "_MW_Param.txt");
    }

    ui->cb_CompatibleGalCorrGrid->addItem("<Enter a new name>");
  } catch (Elements::Exception&) {
  }
}

void FormAnalysis::updateFilterShiftGridSelection() {
  try {
    auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();

    auto axis           = selected_model.getAxesTuple();
    logger.debug() << "updateFilterShiftGridSelection => selected_model content :" << getAxisDescription(axis);
    auto possible_files = PhzGridInfoHandler::getCompatibleGridFile(
        m_survey_model_ptr->getSelectedSurvey().getName(), axis, getSelectedFilters(),
        ui->cb_igm->currentText().toStdString(), ui->lbl_lum_filter->text().toStdString(), FilterShiftCorrectionGrid);

    ui->cb_CompatibleShiftGrid->clear();
    bool added = false;
    for (auto& file : possible_files) {
      ui->cb_CompatibleShiftGrid->addItem(QString::fromStdString(file));
      added = true;
    }

    if (!added) {
      ui->cb_CompatibleShiftGrid->addItem(QString::fromStdString("Grid_" + selected_model.getName() + "_") +
                                          ui->cb_igm->currentText() + "_FS_Param.txt");
    }

    ui->cb_CompatibleShiftGrid->addItem("<Enter a new name>");
  } catch (Elements::Exception&) {
  }
}

void FormAnalysis::setupAlgo() {

  // get the alog Values
  std::string scale_sampling =
      PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                          ui->cb_AnalysisModel->currentText().toStdString() + "_ScaleSampling");

  bool do_sample = (scale_sampling == "True");
  ui->rb_best_scaling->setChecked(!do_sample);
  ui->rb_sample_scaling->setChecked(do_sample);
  on_rb_sample_scaling_toggled(do_sample);

  std::string sampling_number =
      PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                          ui->cb_AnalysisModel->currentText().toStdString() + "_ScaleSamplingNumber");
  if (sampling_number != "") {
    ui->sb_lum_sample_number->setValue(QString::fromStdString(sampling_number).toInt());
  } else {
    ui->sb_lum_sample_number->setValue(101);
  }

  std::string sampling_range =
      PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                          ui->cb_AnalysisModel->currentText().toStdString() + "_ScaleSamplingRange");
  if (sampling_range != "") {
    ui->dsb_sample_range->setValue(QString::fromStdString(sampling_range).toDouble());
  } else {
    ui->dsb_sample_range->setValue(5.0);
  }
}



void FormAnalysis::loadLuminosityPriors() {
  auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

  auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();

  auto folder = FileUtils::getGUILuminosityPriorConfig(true, survey_name, selected_model.getName());

  m_prior_config = LuminosityPriorConfig::readFolder(folder);

  auto prior_name = PreferencesUtils::getUserPreference(survey_name, selected_model.getName() + "_LuminosityPriorName");

  ui->cb_luminosityPrior_2->clear();
  int index = -1;
  int id    = 0;
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

  auto volume_prior_enabled =
      PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                          ui->cb_AnalysisModel->currentText().toStdString() + "_VolumePriorEnabled");

  auto nz_prior_enabled =
      PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                          ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorEnabled");

  if (luminosity_prior_enabled == "1") {
    ui->rb_luminosityPrior->setChecked(true);
  } else if (volume_prior_enabled == "1") {
    ui->rb_volumePrior->setChecked(true);
  } else if (nz_prior_enabled == "1") {
    ui->rb_nzPrior->setChecked(true);
  } else {
    ui->rb_noPrior->setChecked(true);
  }
}


std::set<std::string> FormAnalysis::getPPListFromConfig() {
  auto pp_list = PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                                     ui->cb_AnalysisModel->currentText().toStdString() + "_PP");

  auto                  list_param = QString::fromStdString(pp_list);
  std::set<std::string> res{};
  if (list_param.length() > 0) {
    for (auto& part : list_param.split(";")) {
      res.insert(part.toStdString());
    }

    ui->btn_pp->setText(QString::fromStdString("Select Parameters (") + QString::number(res.size()) +
                        QString::fromStdString(")"));
  } else {
    ui->btn_pp->setText(QString::fromStdString("Select Parameters"));
  }
  return res;
}





///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////      User Interaction: Navigation       ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FormAnalysis::on_btn_ToModel_clicked() {
  navigateToParameter(false);
}
void FormAnalysis::on_btn_ToOption_clicked() {
  navigateToConfig();
}
void FormAnalysis::on_btn_ToCatalog_clicked() {
  navigateToCatalog(false);
}

void FormAnalysis::on_btn_ToPP_clicked() {
  navigateToPostProcessing(false);
}

void FormAnalysis::on_btn_exit_clicked() {
  quit(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////   User Interaction: Catalog & Model selection       //////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void FormAnalysis::on_cb_AnalysisSurvey_currentIndexChanged(const QString& selectedName) {


  logger.info() << "The selected index of the Catalog ComboBox has changed. New selected item:"
                << selectedName.toStdString();
  m_survey_model_ptr->selectSurvey(selectedName);

  SurveyFilterMapping selected_survey = m_survey_model_ptr->getSelectedSurvey();

  /// UPDATE THE SET OF FILTERS IN THE FILTER SELECTION GRID ////
  QStandardItemModel* grid_model = new QStandardItemModel();
  grid_model->setColumnCount(1);

  for (auto filter : selected_survey.getFilters()) {
    QStandardItem* item = new QStandardItem(QString::fromStdString(filter.getFilterFile()));
    item->setCheckable(true);
    item->setCheckState(Qt::CheckState::Checked);

    QList<QStandardItem*> items;
    items.push_back(item);

    grid_model->appendRow(items);
  }

  disconnect(grid_model, SIGNAL(itemChanged(QStandardItem*)), 0, 0);
  ui->tableView_filter->setModel(grid_model);
  connect(grid_model, SIGNAL(itemChanged(QStandardItem*)), SLOT(onFilterSelectionItemChanged(QStandardItem*)));

  // Check which of the Galactic correction option  are available
  ui->rb_gc_off->setChecked(true);
  ui->rb_gc_col->setChecked(false);
  ui->rb_gc_col->setEnabled(selected_survey.getGalEbvColumn().length() > 0);
  ui->rb_gc_planck->setChecked(false);
  ui->rb_gc_planck->setEnabled(selected_survey.getRaColumn().length() > 0 &&
                               selected_survey.getDecColumn().length() > 0);

  auto saved_gal_type = PreferencesUtils::getUserPreference(selected_survey.getName(), "GalCorrType");
  if (saved_gal_type == "COL" && ui->rb_gc_col->isEnabled()) {
    ui->rb_gc_col->setChecked(true);
  } else if (saved_gal_type == "MAP" && ui->rb_gc_planck->isEnabled()) {
    ui->rb_gc_planck->setChecked(true);
  }

  // update the column of the default catalog file
  fillCbColumns(selected_survey.getColumnList(), selected_survey.getRefZColumn());

  // push the last used / default catalog
  auto saved_cat = PreferencesUtils::getUserPreference(selected_survey.getName(), "LAST_USED_CAT");
  if (saved_cat.length() == 0) {
    saved_cat = selected_survey.getDefaultCatalogFile();
  }
  setInputCatalogName(saved_cat, false);

  // set the stored IGM
  auto saved_igm = PreferencesUtils::getUserPreference(selected_survey.getName(), "IGM");
  if (saved_igm.length() > 0) {
    for (int i = 0; i < ui->cb_igm->count(); i++) {
      if (ui->cb_igm->itemText(i).toStdString() == saved_igm) {
        ui->cb_igm->setCurrentIndex(i);
        break;
      }
    }
  }

  setupAlgo();
  updateGridSelection();
  updateGalCorrGridSelection();
  updateFilterShiftGridSelection();
  loadLuminosityPriors();
  updateCorrectionSelection();

  // set the correction file
  auto saved_correction = PreferencesUtils::getUserPreference(selected_survey.getName(), "Correction");
  if (saved_correction.length() > 0) {
    for (int i = 0; i < ui->cb_AnalysisCorrection->count(); i++) {
      if (ui->cb_AnalysisCorrection->itemText(i).toStdString() == saved_correction) {
        ui->cb_AnalysisCorrection->setCurrentIndex(i);
        ui->gb_corrections->setChecked(true);
        break;

      } else {
        ui->gb_corrections->setChecked(false);
      }
    }
  } else {
    ui->gb_corrections->setChecked(false);
  }

  auto saved_sed_weight = PreferencesUtils::getUserPreference(selected_survey.getName(), "ComputeSedWeight");
  ui->cb_sedweight->setChecked(saved_sed_weight != "False");

  setCopiedColumns(selected_survey.getCopiedColumns());
  setRunAnnalysisEnable(true);
  getPPListFromConfig();
}

void FormAnalysis::on_cb_AnalysisModel_currentIndexChanged(const QString& model_name) {
  logger.info() << "The selected index of the Parameter Space ComboBox has changed. New selected item:"
                << model_name.toStdString();
  m_model_set_model_ptr->selectModelSet(model_name);

  setupAlgo();
  updateGridSelection();
  updateGalCorrGridSelection();
  updateFilterShiftGridSelection();
  loadLuminosityPriors();
  getPPListFromConfig();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////   User Interaction: 1 Computation Type       //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Fill the Fixed redshift Column ComboBox
void FormAnalysis::fillCbColumns(std::set<std::string> columns, std::string default_col) {
  ui->cb_z_col->clear();
  ui->cb_z_col->addItem("");
  int index          = 1;
  int selected_index = -1;
  for (auto item : columns) {
    ui->cb_z_col->addItem(QString::fromStdString(item));
    if (item == default_col) {
      selected_index = index;
    }
    index++;
  }

  if (selected_index > 0) {
    ui->cb_z_col->setCurrentIndex(selected_index);
  }
}


// Radio button switching state
void FormAnalysis::on_rb_fixZTol_toggled(bool on){
	if (on) {
		ui->rb_scaleZTol->setChecked(false);
	}
}

void FormAnalysis::on_rb_scaleZTol_toggled(bool on){
	if (on) {
		ui->rb_fixZTol->setChecked(false);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////    User Interaction: 2 Luminosity and Absorption       //////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Set the luminosity filter on DialogFilterSelector popup closing
void FormAnalysis::setLumFilter(std::string new_filter) {
  ui->lbl_lum_filter->setText(QString::fromStdString(new_filter));

  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                      ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityFilter",
                                      new_filter);
  if (!m_is_loading) {
	  updateGridSelection();
  }
}

// Open the DialogFilterSelector popup for selecting the luminosity filter
void FormAnalysis::on_btn_lum_filter_clicked() {
  std::unique_ptr<DialogFilterSelector> dialog(new DialogFilterSelector(m_filter_repository));
  dialog->setFilter(ui->lbl_lum_filter->text().toStdString());
  connect(dialog.get(), SIGNAL(popupClosing(std::string)), SLOT(setLumFilter(std::string)));
  dialog->exec();
}

// Push the change in the IGM to the grids
void FormAnalysis::on_cb_igm_currentIndexChanged(const QString&) {
  updateGridSelection();
  updateGalCorrGridSelection();
  updateFilterShiftGridSelection();
}

// Galactic Correction selection
void FormAnalysis::on_rb_gc_off_clicked() {
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "GalCorrType", "OFF");
  adjustGridsButtons(true);
  setRunAnnalysisEnable(true);
}
void FormAnalysis::on_rb_gc_col_clicked() {
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "GalCorrType", "COL");
  adjustGridsButtons(true);
  setRunAnnalysisEnable(true);
}
void FormAnalysis::on_rb_gc_planck_clicked() {
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "GalCorrType", "MAP");
  adjustGridsButtons(true);
  setRunAnnalysisEnable(true);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////    User Interaction: 3 Grid Generation       //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Change the name of the Model Grid
void FormAnalysis::on_cb_CompatibleGrid_currentTextChanged(const QString&) {
	if (!m_is_loading){
	  std::string grid_name = ui->cb_CompatibleGrid->currentText().toStdString();
	  size_t      index     = grid_name.find_last_of("/\\");
	  if (index != string::npos) {
		grid_name = grid_name.substr(index + 1);
	  }

	  index = grid_name.find_last_of(".");
	  if (index != string::npos) {
		grid_name = grid_name.substr(0, index);
	  }

	  if (!ui->rb_gc_off->isChecked()) {
		  ui->cb_CompatibleGalCorrGrid->setItemText(ui->cb_CompatibleGalCorrGrid->currentIndex(),
		  												QString::fromStdString(grid_name + "_MW_Param.txt"));
	  }

	  if (m_survey_model_ptr->getSelectedSurvey().getDefineFilterShift()) {
		  ui->cb_CompatibleShiftGrid->setItemText(ui->cb_CompatibleShiftGrid->currentIndex(),
		  												QString::fromStdString(grid_name + "_FS_Param.txt"));
	  }



	  adjustGridsButtons(true);
	  setComputeCorrectionEnable();
	  setRunAnnalysisEnable(true);
	}
}

// Generate the config for the model grid
void FormAnalysis::on_btn_GetConfigGrid_clicked() {
  if (!checkGridSelection(true, true)) {
    QMessageBox::warning(
        this, "Unavailable name...",
        "It is not possible to save the Grid under the name you have provided. Please enter a new name.",
        QMessageBox::Ok);
  } else {
    QString filter = "Config (*.CMG.conf)";
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save Configuration File"),
                                     QString::fromStdString(FileUtils::getRootPath(true)) + "config", filter, &filter);
    if (fileName.length() > 0) {
      if (!fileName.endsWith(".CMG.conf", Qt::CaseInsensitive)) {
        fileName = fileName + ".CMG.conf";
      }
      std::list<float> zs{}; // generate the full model grid
      auto config_map = getGridConfiguration(zs);
      PhzUITools::ConfigurationWriter::writeConfiguration(config_map, fileName.toStdString());

      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "IGM",
                                          ui->cb_igm->currentText().toStdString());
    }
  }
}

// Change the name of the MW Correction Grid
void FormAnalysis::on_cb_CompatibleGalCorrGrid_currentTextChanged(const QString&) {
  adjustGridsButtons(true);
  setComputeCorrectionEnable();
  setRunAnnalysisEnable(true);
}

// Generate the config for the MW Correction grid
void FormAnalysis::on_btn_GetGalCorrConfigGrid_clicked() {
  if (!checkGalacticGridSelection(true, true)) {
    QMessageBox::warning(this, "Unavailable name...",
                         "It is not possible to save the Galactic Correction Grid under the name you have provided. "
                         "Please enter a new name.",
                         QMessageBox::Ok);
  } else {
    QString filter = "Config (*.CGCCG.conf)";
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save Configuration File"),
                                     QString::fromStdString(FileUtils::getRootPath(true)) + "config", filter, &filter);
    if (fileName.length() > 0) {
      if (!fileName.endsWith(".CGCCG.conf", Qt::CaseInsensitive)) {
        fileName = fileName + ".CGCCG.conf";
      }
      auto config_map = getGalacticCorrectionGridConfiguration();
      if (config_map.size() > 0) {
        PhzUITools::ConfigurationWriter::writeConfiguration(config_map, fileName.toStdString());
      }
    }
  }
}

// Change the name of the Filter Variation Grid
void FormAnalysis::on_cb_CompatibleShiftGrid_currentTextChanged(const QString&) {
  adjustGridsButtons(true);
  setComputeCorrectionEnable();
  setRunAnnalysisEnable(true);
}

// Generate the config for the Filter Variation grid
void FormAnalysis::on_btn_GetShiftConfigGrid_clicked() {
  if (!checkFilterShiftGridSelection(true, true)) {
    QMessageBox::warning(this, "Unavailable name...",
                         "It is not possible to save the Filter Variation Coefficients Grid under the name you have "
                         "provided. Please enter a new name.",
                         QMessageBox::Ok);
  } else {
    QString filter = "Config (*.CFVCG.conf)";
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save Configuration File"),
                                     QString::fromStdString(FileUtils::getRootPath(true)) + "config", filter, &filter);
    if (fileName.length() > 0) {
      if (!fileName.endsWith(".CFVCG.conf", Qt::CaseInsensitive)) {
        fileName = fileName + ".CFVCG.conf";
      }
      auto config_map = getFilterShiftGridConfiguration();
      if (config_map.size() > 0) {
        PhzUITools::ConfigurationWriter::writeConfiguration(config_map, fileName.toStdString());
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////    User Interaction: 4 Prior Selection       //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Save the SED weight prior selection
void FormAnalysis::on_cb_sedweight_clicked() {
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "ComputeSedWeight",
                                      ui->cb_sedweight->isChecked() ? "True" : "False");
}

// Prior radio button toggeling
void FormAnalysis::on_rb_luminosityPrior_toggled(bool on) {
  if (on) {
    ui->rb_volumePrior->setChecked(false);
    ui->rb_noPrior->setChecked(false);
    ui->rb_nzPrior->setChecked(false);
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityPriorEnabled",
                                        QString::number(ui->rb_luminosityPrior->isChecked()).toStdString());
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_VolumePriorEnabled",
                                        QString::number(ui->rb_volumePrior->isChecked()).toStdString());
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorEnabled",
                                        QString::number(ui->rb_nzPrior->isChecked()).toStdString());
    setRunAnnalysisEnable(true);
  }
}

void FormAnalysis::on_rb_volumePrior_toggled(bool on) {
  if (on) {
    ui->rb_luminosityPrior->setChecked(false);
    ui->rb_noPrior->setChecked(false);
    ui->rb_nzPrior->setChecked(false);
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityPriorEnabled",
                                        QString::number(ui->rb_luminosityPrior->isChecked()).toStdString());
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_VolumePriorEnabled",
                                        QString::number(ui->rb_volumePrior->isChecked()).toStdString());
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorEnabled",
                                        QString::number(ui->rb_nzPrior->isChecked()).toStdString());
    setRunAnnalysisEnable(true);
  }
}

void FormAnalysis::on_rb_noPrior_toggled(bool on) {
  if (on) {
    ui->rb_luminosityPrior->setChecked(false);
    ui->rb_volumePrior->setChecked(false);
    ui->rb_nzPrior->setChecked(false);
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityPriorEnabled",
                                        QString::number(ui->rb_luminosityPrior->isChecked()).toStdString());
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_VolumePriorEnabled",
                                        QString::number(ui->rb_volumePrior->isChecked()).toStdString());
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorEnabled",
                                        QString::number(ui->rb_nzPrior->isChecked()).toStdString());
    setRunAnnalysisEnable(true);
  }
}

void FormAnalysis::on_rb_nzPrior_toggled(bool on) {
  if (on) {
    ui->rb_luminosityPrior->setChecked(false);
    ui->rb_volumePrior->setChecked(false);
    ui->rb_noPrior->setChecked(false);
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityPriorEnabled",
                                        QString::number(ui->rb_luminosityPrior->isChecked()).toStdString());
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_VolumePriorEnabled",
                                        QString::number(ui->rb_volumePrior->isChecked()).toStdString());
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorEnabled",
                                        QString::number(ui->rb_nzPrior->isChecked()).toStdString());
    setRunAnnalysisEnable(true);
  }
}


// Update the N(z) filters on DialogNz popup closing
void FormAnalysis::setNzFilters(std::string b_filter, std::string i_filter) {
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                      ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorBFilter", b_filter);
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                      ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorIFilter", i_filter);
  setRunAnnalysisEnable(true);
}

// Open the DialogNz popup
void FormAnalysis::on_btn_conf_Nz_clicked() {
  std::string nz_prior_b_filter =
      PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                          ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorBFilter");

  std::string nz_prior_i_filter =
      PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                          ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorIFilter");

  std::unique_ptr<DialogNz> popup(new DialogNz(m_survey_model_ptr->getSelectedSurvey().getFilters(),
                                               m_filter_repository, nz_prior_b_filter, nz_prior_i_filter));

  connect(popup.get(), SIGNAL(popupClosing(std::string, std::string)), SLOT(setNzFilters(std::string, std::string)));

  popup->exec();
}

// open the DialogLuminosityPrior popup
void FormAnalysis::on_btn_confLuminosityPrior_clicked() {
  std::list<float> zs{};
  if (!checkGridSelection(true, false)) {
	 if (! BuildModelGrid(zs)) {
	   return;
	 }
  }
  std::unique_ptr<DialogLuminosityPrior> dialog(
      new DialogLuminosityPrior(ui->lbl_lum_filter->text().toStdString(), m_luminosity_repository, m_sed_repository, m_redenig_curves_repository));

  std::string model_grid = ui->cb_CompatibleGrid->currentText().toStdString();

  auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

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

  dialog->loadData(selected_model, survey_name, model_grid, z_min, z_max);
  dialog->exec();
  loadLuminosityPriors();
}

// Change the luminosity prior selection
void FormAnalysis::on_cb_luminosityPrior_2_currentIndexChanged(const QString&) {
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                      ui->cb_AnalysisModel->currentText().toStdString() + "_LuminosityPriorName",
                                      ui->cb_luminosityPrior_2->currentText().toStdString());

  setRunAnnalysisEnable(true);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////    User Interaction: 5 Zero-Point Correction       ///////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Handle the Group box activation
void FormAnalysis::on_gb_corrections_clicked() {
  setRunAnnalysisEnable(true);
  setComputeCorrectionEnable();
}

// Open the DialogPhotometricCorrectionComputation popup
void FormAnalysis::on_btn_computeCorrections_clicked() {
 // Build the Model Grid if needed
  std::list<float> zs{};
   if (!checkGridSelection(true, false)) {
	 if (!BuildModelGrid(zs)) {
	   return;
	 }
   }

  // Build MW correction grid if needed
  bool need_gal_correction       = !ui->rb_gc_off->isChecked();
  bool has_gal_corr_grid         = checkGalacticGridSelection(true, false);
  if (need_gal_correction && (!has_gal_corr_grid || !checkCompatibleGalacticGrid(ui->cb_CompatibleGalCorrGrid->currentText().toStdString()))){
	  if (!BuildMwCorrGrid()) {
		   return;
		 }
  }

  // build filter shift grid if needed
  bool need_filter_shift_grid    = m_survey_model_ptr->getSelectedSurvey().getDefineFilterShift();
  bool has_filter_shift_grid     = checkFilterShiftGridSelection(true, false);
  if (need_filter_shift_grid && (!has_filter_shift_grid || !checkCompatibleFilterShiftGrid(ui->cb_CompatibleShiftGrid->currentText().toStdString()))) {
	  if (!BuildFilterShiftGrid()) {
		 return;
	  }
  }

  auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

  SurveyFilterMapping selected_survey = m_survey_model_ptr->getSelectedSurvey();

  auto config_map = getRunOptionMap();

  std::string ra_col  = "";
  std::string dec_col = "";

  if (ui->rb_gc_planck->isChecked()) {
    SurveyFilterMapping sel_survey = m_survey_model_ptr->getSelectedSurvey();
    ra_col                         = sel_survey.getRaColumn();
    dec_col                        = sel_survey.getDecColumn();
  }

  std::map<std::string, boost::program_options::variable_value> config_sed_weight{};
  if (ui->cb_sedweight->isChecked()) {
    // Compute the SED Axis Prior
    std::string sed_prior_name = getSedWeightFileName();
    if (!checkSedWeightFile(sed_prior_name)) {
      config_sed_weight = getSedWeightOptionMap(sed_prior_name);
    }
  }

  std::unique_ptr<DialogPhotometricCorrectionComputation> popup(new DialogPhotometricCorrectionComputation());
  popup->setData(survey_name, m_survey_model_ptr->getSelectedSurvey().getSourceIdColumn(),
                 ui->cb_AnalysisModel->currentText().toStdString(), ui->cb_CompatibleGrid->currentText().toStdString(),
                 getSelectedFilterMapping(), getExcludedFilters(), selected_survey.getRefZColumn(), config_map,
                 config_sed_weight, selected_survey.getNonDetection(), m_planck_file, ra_col, dec_col);

  connect(popup.get(), SIGNAL(correctionComputed(const QString&)), SLOT(onCorrectionComputed(const QString&)));
  popup->exec();

  PhzUtils::getStopThreadsFlag() = false;
}

// Update the Correction CB on DialogPhotometricCorrectionComputation popup closing
void FormAnalysis::onCorrectionComputed(const QString& new_file_name) {
  updateCorrectionSelection();
  ui->cb_AnalysisCorrection->setCurrentIndex(ui->cb_AnalysisCorrection->findText(new_file_name));
}

// Handle the change in the Correction CB
void FormAnalysis::on_cb_AnalysisCorrection_currentIndexChanged(const QString&) {
  setRunAnnalysisEnable(true);
}

// Open the popup for the edition of the Correction file
void FormAnalysis::on_btn_editCorrections_clicked() {
  bool open = true;
  if (ui->cb_AnalysisCorrection->currentText().toStdString().length() == 0) {
    open = false;
    std::unique_ptr<DialogZeroPointName> popupname(new DialogZeroPointName());

    auto catalog     = ui->cb_AnalysisSurvey->currentText().toStdString();
    auto root_folder = FileUtils::getPhotCorrectionsRootPath(true, catalog);
    popupname->setFolder(root_folder);
    // prompt for new name
    if (popupname->exec() == QDialog::Accepted) {
      // create file with correction to 1
      std::string path = root_folder + "/" + popupname->getName();
      logger.info() << "Creating Zero-Point correction file " << path;
      QFile file(QString::fromStdString(path));
      if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "# Column: Filter string\n"
            << "# Column: Correction double\n\n"
            << "#  Filter Correction\n\n";

        for (auto& filter : getSelectedFilterMapping()) {
          out << QString::fromStdString(filter.getFilterFile()) << "     1.0\n";
          open = true;
        }
        file.close();
        onCorrectionComputed(QString::fromStdString(popupname->getName()));
      }
    }
  }

  if (open) {
    std::unique_ptr<DialogPhotCorrectionEdition> popup(new DialogPhotCorrectionEdition());
    popup->setCorrectionsFile(ui->cb_AnalysisSurvey->currentText().toStdString(),
                              ui->cb_AnalysisCorrection->currentText().toStdString(), getSelectedFilterMapping());
    popup->exec();
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////    User Interaction: 6 Algorithm       /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// toggle the radio button for the model scaling
void FormAnalysis::on_rb_best_scaling_toggled(bool on) {
  ui->rb_sample_scaling->setChecked(!on);
  ui->wdg_sample->setEnabled(!on);
  std::string param = on ? "False" : "True";
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                      ui->cb_AnalysisModel->currentText().toStdString() + "_ScaleSampling", param);
}
void FormAnalysis::on_rb_sample_scaling_toggled(bool on) {
  ui->rb_best_scaling->setChecked(!on);
  ui->wdg_sample->setEnabled(on);
  std::string param = !on ? "False" : "True";
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                      ui->cb_AnalysisModel->currentText().toStdString() + "_ScaleSampling", param);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////    User Interaction: 7 Input/Output       ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Open the popup for the input catalog selection
void FormAnalysis::on_btn_BrowseInput_clicked() {
  QFileDialog dialog(this);
  std::string path = ui->txt_inputCatalog->text().toStdString();

  dialog.selectFile(QString::fromStdString(path));
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()) {
    setInputCatalogName(dialog.selectedFiles()[0].toStdString());
    ui->cb_skip->setChecked(false);
    ui->sb_skip->setValue(0);
    ui->sb_skip->setEnabled(false);
    ui->cb_process_limit->setChecked(false);
    ui->sb_process_limit->setValue(0);
    ui->sb_process_limit->setEnabled(false);

    setRunAnnalysisEnable(true);
  }
}

// Handle the change in the skip object Check box
void FormAnalysis::on_cb_skip_stateChanged(int) {
  ui->sb_skip->setEnabled(ui->cb_skip->isChecked());
}

// Handle the change in the process limit Check box
void FormAnalysis::on_cb_process_limit_stateChanged(int) {
  ui->sb_process_limit->setEnabled(ui->cb_process_limit->isChecked());
}

// Filter selection modification from the grid
void FormAnalysis::onFilterSelectionItemChanged(QStandardItem*) {
  updateGridSelection();
  updateGalCorrGridSelection();
  updateFilterShiftGridSelection();
  updateCorrectionSelection();
}

// Open the popup for the Output folder selection
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

// Update the copied column on the popup DialogOutputColumnSelection closing
void FormAnalysis::setCopiedColumns(std::map<std::string, std::string> columns) {
  m_copied_columns   = columns;
  std::string copy   = "Copy Columns (";
  std::string copy_2 = ")";

  ui->output_column_btn->setText(QString::fromStdString(copy) + QString::number(m_copied_columns.size()) +
                                 QString::fromStdString(copy_2));
}

// Open the DialogOutputColumnSelection column
void FormAnalysis::on_output_column_btn_clicked() {
  auto column_reader = PhzUITools::CatalogColumnReader(ui->txt_inputCatalog->text().toStdString());
  auto col_set       = column_reader.getColumnNames();

  std::list<std::string>                       all_columns(col_set.begin(), col_set.end());
  std::string                                  id_col = m_survey_model_ptr->getSelectedSurvey().getSourceIdColumn();
  std::unique_ptr<DialogOutputColumnSelection> popup(
      new DialogOutputColumnSelection(all_columns, id_col, m_copied_columns));

  connect(popup.get(), SIGNAL(selectedColumns(std::map<std::string, std::string>)),
          SLOT(setCopiedColumns(std::map<std::string, std::string>)));

  popup->exec();
}

// Update the PP on the closing of the DialogSelectParam popup
void FormAnalysis::update_pp_selection(std::vector<std::string> params) {
  logger.info() << "update_pp_selection with "<<params.size()<<" params" ;
  ui->btn_pp->setText(QString::fromStdString("Select Parameters (") + QString::number(params.size()) +
                      QString::fromStdString(")"));

  std::string pps = "";
  for (auto& param : params) {
    if (pps.length() > 0) {
      pps = pps + ";";
    }

    pps = pps + param;
  }

  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                      ui->cb_AnalysisModel->currentText().toStdString() + "_PP", pps);
}

// Progress bar for the PP computation + open the DialogSelectParam popup
void FormAnalysis::updatePpProgress(size_t current, size_t total) {

  m_progress_dialog->setMaximum(total);
  m_progress_dialog->setValue(current);

  if (total == current) {

    disconnect(m_sed_param, SIGNAL(progress(size_t, size_t)), 0, 0);
    m_progress_dialog->hide();
    delete m_progress_dialog;
    m_progress_dialog                             = 0;
    auto                               list_param = m_sed_param->getList();
    std::unique_ptr<DialogSelectParam> popup(new DialogSelectParam(list_param, this));

    popup->setParams(getPPListFromConfig());

    connect(popup.get(), SIGNAL(popupClosing(std::vector<std::string>)),
            SLOT(update_pp_selection(std::vector<std::string>)));

    popup->exec();
  }
}

// Open the PP computation popup
void FormAnalysis::on_btn_pp_clicked() {
  m_progress_dialog = new QProgressDialog(this);
  m_progress_dialog->setWindowTitle(tr("Collecting PP... "));
  m_progress_dialog->setLabelText(tr("Parsing the SED to list the available Physical Parameters."));
  QList<QPushButton*> L = m_progress_dialog->findChildren<QPushButton*>();
  L.at(0)->hide();
  m_progress_dialog->setMaximum(10);
  m_progress_dialog->setValue(0);
  m_progress_dialog->setWindowModality(Qt::WindowModal);
  connect(m_sed_param, SIGNAL(progress(size_t, size_t)), this, SLOT(updatePpProgress(size_t, size_t)));
  m_sed_param->listAvailableParam(m_model_set_model_ptr->getSelectedModelSet(), m_sed_repository, m_redenig_curves_repository);
}

// Open the warning popup for slow computation
void FormAnalysis::on_cb_gen_posterior_clicked() {
  if (ui->cb_gen_posterior->isChecked()) {
    if (ui->cb_multi_out->currentText() == "Sampling") {
      QMessageBox::warning(this, "Slow computation...",
                           "Sampling the multi-dimensional posterior grids "
                           " will slow down the computation.",
                           QMessageBox::Ok);
    } else {
      QMessageBox::warning(this, "Large output volume...",
                           "Outputing multi-dimensional posterior grids (one file per source)"
                           " will generate a large output volume.",
                           QMessageBox::Ok);
    }
  }
  setRunAnnalysisEnable(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////




void FormAnalysis::updateCorrectionSelection() {
  auto filter_map = getSelectedFilters();
  auto file_list  = PhotometricCorrectionHandler::getCompatibleCorrectionFiles(
       m_survey_model_ptr->getSelectedSurvey().getName(), filter_map);
  ui->cb_AnalysisCorrection->clear();

  for (auto file : file_list) {
    ui->cb_AnalysisCorrection->addItem(QString::fromStdString(file));
  }
}

void FormAnalysis::adjustGridsButtons(bool enabled) {

  // Model grid Get config button
  bool model_grid_name_ok = checkGridSelection(false, true);

  ui->btn_GetConfigGrid->setEnabled(enabled && model_grid_name_ok);

  QString tool_tip  = "Export the configuration file needed to run Phosphoros CMG tool which produce the grid "
                      "containing the model photometries";
  QColor  tab_color = Qt::black;

  if (!model_grid_name_ok) {
    ui->cb_CompatibleGrid->lineEdit()->setStyleSheet("QLineEdit { color: orange }");
    tab_color = QColorConstants::Svg::orange;
    tool_tip = "Please enter a valid grid name in order to compute the Grid or export the corresponding configuration.";
  } else {
    ui->cb_CompatibleGrid->lineEdit()->setStyleSheet("QLineEdit { color: black }");
  }
  ui->btn_GetConfigGrid->setToolTip(tool_tip);

  // MW reddening grid Get config button
  bool mw_grid_name_ok  = checkGalacticGridSelection(false, true);
  bool needed           = !ui->rb_gc_off->isChecked();
  ui->cb_CompatibleGalCorrGrid->setEnabled(needed && enabled);
  ui->btn_GetGalCorrConfigGrid->setEnabled(needed && enabled && mw_grid_name_ok);
  QString tool_tip_mw = "Export the configuration file needed to run Phosphoros CGCCG tool which produce the grid "
                        "containing the coefficient for the correction of the Milky Way absorption";
  if (!needed) {
    ui->cb_CompatibleGalCorrGrid->lineEdit()->setStyleSheet("QLineEdit { color: black }");
    tool_tip_mw = "With 'Correction type' set to 'OFF' there is no need to generate this grid.";
  } else if (!mw_grid_name_ok) {
    ui->cb_CompatibleGalCorrGrid->lineEdit()->setStyleSheet("QLineEdit { color: orange }");
    tab_color = QColorConstants::Svg::orange;
    tool_tip_mw =
        "Please enter a valid grid name in order to compute the Grid or export the corresponding configuration.";
  } else {
    ui->cb_CompatibleGalCorrGrid->lineEdit()->setStyleSheet("QLineEdit { color: black }");
  }
  ui->btn_GetGalCorrConfigGrid->setToolTip(tool_tip_mw);

  // Filter Shift grid Get config button
  auto& curr_catalog = m_survey_model_ptr->getSelectedSurvey();
  bool fs_grid_name_ok = checkFilterShiftGridSelection(false, true);
  needed             = curr_catalog.getDefineFilterShift();
  ui->cb_CompatibleShiftGrid->setEnabled(needed && enabled);
  ui->btn_GetShiftConfigGrid->setEnabled(needed && enabled && fs_grid_name_ok);
  QString tool_tip_fs = "Export the configuration file needed to run Phosphoros CFVCG tool which produce the grid "
                        "containing the coefficient for the correction of the Filters variation";
  if (!needed) {
    ui->cb_CompatibleShiftGrid->lineEdit()->setStyleSheet("QLineEdit { color: black }");
    tool_tip_fs = "the selected catalog do not define columns for the filter variation: no need to generate this grid.";
  } else if (!fs_grid_name_ok) {
    ui->cb_CompatibleShiftGrid->lineEdit()->setStyleSheet("QLineEdit { color: orange }");
    tab_color = QColorConstants::Svg::orange;
    tool_tip_fs =
        "Please enter a valid grid name in order to compute the Grid or export the corresponding configuration.";
  } else {
    ui->cb_CompatibleShiftGrid->lineEdit()->setStyleSheet("QLineEdit { color: black }");
  }
  ui->btn_GetShiftConfigGrid->setToolTip(tool_tip_fs);

  setToolBoxButtonColor(ui->toolBox, 2, tab_color);
}


void FormAnalysis::setComputeCorrectionEnable() {
  bool model_grid_name_exists        = checkGridSelection(false, true);
  bool gal_corr_needed               = !ui->rb_gc_off->isChecked();
  bool grid_gal_corr_name_exists     = checkGalacticGridSelection(false, true);
  bool filter_shift_grid_needed      = m_survey_model_ptr->getSelectedSurvey().getDefineFilterShift();
  bool grid_filter_shift_name_exists = checkFilterShiftGridSelection(false, true);

  ui->btn_computeCorrections->setEnabled(
                  model_grid_name_exists &&
				  (!gal_corr_needed || grid_gal_corr_name_exists) &&
                  (!filter_shift_grid_needed || grid_filter_shift_name_exists) &&
                  ui->gb_corrections->isChecked());

  QString tool_tip = "Open the photometric zero-point correction popup.";

  bool valid = true;

  if (filter_shift_grid_needed && !grid_filter_shift_name_exists) {
	  if (valid){
		 tool_tip = "Please select a valid name for the Filter Shift Coefficient grid before computing the photometric corrections.";
		 valid = false;
	  } else{
		 tool_tip += " Please select a valid name for the Filter Shift Coefficient grid before computing the photometric corrections.";
	  }
  }

  if (gal_corr_needed && (!grid_gal_corr_name_exists )) {
	  if (valid){
		  tool_tip = "Please select a valid name for the Galactic Correction grid before computing the photometric corrections.";
		  valid = false;
	  } else{
		  tool_tip += " Please select a valid name for the Galactic Correction grid before computing the photometric corrections.";
	  }
  }

  if (!model_grid_name_exists) {
	  if (valid){
	      tool_tip = "Please select a valid name for Photometric grid before computing the photometric corrections.";
		  valid = false;
	  } else{
		  tool_tip += " Please select a valid name for Photometric grid before computing the photometric corrections.";
	  }

  }

  ui->btn_computeCorrections->setToolTip(tool_tip);
}

void FormAnalysis::setRunAnnalysisEnable(bool enabled) {

  bool grid_name_ok     = checkGridSelection(false, true);

  bool need_gal_correction       = !ui->rb_gc_off->isChecked();
  bool grid_gal_corr_name_ok     = checkGalacticGridSelection(false, true);

  bool filter_shift_grid_needed      = m_survey_model_ptr->getSelectedSurvey().getDefineFilterShift();
  bool grid_filter_shift_name_ok     = checkFilterShiftGridSelection(false, true);

  bool correction_ok =
      !ui->gb_corrections->isChecked() || ui->cb_AnalysisCorrection->currentText().toStdString().length() > 0;

  QFileInfo info(QString::fromStdString(
                     FileUtils::getPhotCorrectionsRootPath(false, m_survey_model_ptr->getSelectedSurvey().getName())) +
                 QDir::separator() + ui->cb_AnalysisCorrection->currentText());
  bool      correction_exists = !ui->gb_corrections->isChecked() || info.exists();

  QFileInfo info_input(ui->txt_inputCatalog->text());
  bool      run_ok = info_input.exists();

  bool lum_prior_ok = !ui->rb_luminosityPrior->isChecked() || ui->cb_luminosityPrior_2->currentText().length() > 0;
  bool lum_prior_compatible = true;
  if (lum_prior_ok && ui->rb_luminosityPrior->isChecked()) {
    LuminosityPriorConfig prior_info;
    for (auto prior_pair : m_prior_config) {
      if (ui->cb_luminosityPrior_2->currentText().toStdString() == prior_pair.first) {
        prior_info = prior_pair.second;
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

    lum_prior_compatible = prior_info.isCompatibleWithParameterSpace(z_min, z_max, selected_model.getSeds());
  }

  ui->btn_confLuminosityPrior->setEnabled(grid_name_ok);
  ui->btn_confLuminosityPrior->setToolTip("Configure the Luminosity Prior");

  bool nzPrior_ok = true;
  if (ui->rb_nzPrior->isChecked()) {

    std::string nz_prior_b_filter =
        PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                            ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorBFilter");

    std::string nz_prior_i_filter =
        PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                            ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorIFilter");

    nzPrior_ok = nz_prior_b_filter != "" && nz_prior_i_filter != "";
  }

  ui->btn_confLuminosityPrior->setEnabled(grid_name_ok);
  ui->btn_conf_Nz->setToolTip("Configure the N(z) Prior");

  bool nz_prior_ok = !ui->rb_nzPrior->isChecked() || nzPrior_ok;

  ui->btn_GetConfigAnalysis->setEnabled(grid_name_ok &&
		                                (!need_gal_correction || grid_gal_corr_name_ok) &&
                                        (!filter_shift_grid_needed || grid_filter_shift_name_ok) &&
										correction_ok &&
                                        lum_prior_ok && lum_prior_compatible &&
										nz_prior_ok && run_ok && enabled);
  ui->btn_RunAnalysis->setEnabled(grid_name_ok &&
		                          (!need_gal_correction || grid_gal_corr_name_ok) &&
                                  (!filter_shift_grid_needed || grid_filter_shift_name_ok) &&
								  correction_ok && correction_exists &&
								  lum_prior_ok && lum_prior_compatible && nz_prior_ok && run_ok && enabled);

  QString tool_tip_run  = "";
  QString tool_tip_conf = "";
  if (!grid_name_ok) {
    tool_tip_conf = tool_tip_conf + "Please enter a valid model grid name. \n";
    tool_tip_run  = tool_tip_run + "Please enter a valid model grid name. \n";
  }

  if (need_gal_correction) {
    if (!grid_gal_corr_name_ok) {
      tool_tip_conf =
          tool_tip_conf +
          "You have enabled the Galactic Absorption Correction Please enter a valid Galactic correction grid name. \n";
      tool_tip_run =
          tool_tip_run +
          "You have enabled the Galactic Absorption Correction Please enter a valid Galactic correction grid name. \n";
    }
  }

  if (filter_shift_grid_needed) {
    if (!grid_filter_shift_name_ok) {
      tool_tip_conf = tool_tip_conf + "The catalog you are using defines columns for filter shifts. Please enter a "
                                      "valid Filter Shift Coefficient grid name. \n";
      tool_tip_run = tool_tip_run + "The catalog you are using defines columns for filter shifts. Please enter a valid "
                                    "Filter Shift Coefficient grid name. \n";
    }
  }

  if (!correction_ok) {
    tool_tip_conf = tool_tip_conf +
                    "When the photometric corrections are enabled, you must provide a valid correction file name. \n";
    tool_tip_run = tool_tip_run +
                    "When the photometric corrections are enabled, you must provide a valid correction file name. \n";
  }

  if (!correction_exists) {
    tool_tip_run = tool_tip_run + "Please run the photometric correction computation. \n";
  }

  if (!lum_prior_ok) {
    tool_tip_conf =
        tool_tip_conf + "When the luminosity prior is enabled , you must provide a luminosity prior configuration. \n";
    tool_tip_run =
        tool_tip_run + "When the luminosity prior is enabled , you must provide a luminosity prior configuration. \n";
  }

  if (!lum_prior_compatible) {
    tool_tip_conf =
        tool_tip_conf + "The Luminosity Prior is no longer compatible with the Parameter Space, please update it. \n";
    tool_tip_run =
        tool_tip_run + "The Luminosity Prior is no longer compatible with the Parameter Space, please update it. \n";
  }

  if (!nz_prior_ok) {
    tool_tip_conf =
        tool_tip_conf + "When the N(z) prior is enabled , you must configure it by selecting B and I filters. \n";
    tool_tip_run =
        tool_tip_run + "When the N(z) prior is enabled , you must configure it by selecting B and I filters. \n";
  }

  if (!info_input.exists()) {
    ui->txt_inputCatalog->setStyleSheet("QLineEdit { color: #F78181 }");
    tool_tip_conf =
        tool_tip_conf +
        "Please provide a compatible input catalog (at least all the columns used for the Id and filters). \n";
    tool_tip_run =
        tool_tip_run +
        "Please provide a compatible input catalog (at least all the columns used for the Id and filters). \n";

  } else {
    ui->txt_inputCatalog->setStyleSheet("QLineEdit { color: grey }");
  }

  if (!(grid_name_ok && (!need_gal_correction || grid_gal_corr_name_ok) && (!filter_shift_grid_needed || grid_filter_shift_name_ok) &&
		  correction_ok && lum_prior_ok && lum_prior_compatible && nz_prior_ok && run_ok)) {
    tool_tip_conf = tool_tip_conf + "Before getting the configuration.";
  } else {
    tool_tip_conf =
        "Export the configuration file needed to run Phosphoros CR tool which perform the template fitting.";
  }

  if (!(grid_name_ok && (!need_gal_correction || grid_gal_corr_name_ok) && (!filter_shift_grid_needed || grid_filter_shift_name_ok) &&
		  correction_ok && correction_exists && lum_prior_ok && lum_prior_compatible && nz_prior_ok && run_ok)) {
    tool_tip_run = tool_tip_run + "Before running the analysis.";
  } else {
    tool_tip_run = "Run the analysis.";
  }

  ui->btn_GetConfigAnalysis->setToolTip(tool_tip_conf);
  ui->btn_RunAnalysis->setToolTip(tool_tip_run);

  if (!correction_ok) {
    setToolBoxButtonColor(ui->toolBox, 4, QColor("orange"));
  } else {
    setToolBoxButtonColor(ui->toolBox, 4, Qt::black);
  }

  if (!lum_prior_ok || !lum_prior_compatible || !nz_prior_ok) {
    setToolBoxButtonColor(ui->toolBox, 3, QColor("orange"));
  } else {
    setToolBoxButtonColor(ui->toolBox, 3, Qt::black);
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////          Build the grids          /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool FormAnalysis::BuildModelGrid(const std::list<float>& zs){
	if (!checkGridSelection(true, true)) {
	    QMessageBox::warning(
	        this, "Unavailable name...",
	        "It is not possible to save the Grid under the name you have provided. Please enter a new name.",
	        QMessageBox::Ok);
	    return false; // grid not generated
	  } else {

	    if (checkGridSelection(true, false)) {
	      if (QMessageBox::warning(this, "Override existing file...",
	                               "A Model Grid file with the very same name as the one you provided already exist. "
	                               "Do you want to replace it?",
	                               QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
	        return true; // grid already ready
	      }
	    }

	    auto                                  config_map = getGridConfiguration(zs);
	    std::unique_ptr<DialogGridGeneration> dialog(new DialogGridGeneration());
	    dialog->setValues(FileUtils::addExt(ui->cb_CompatibleGrid->currentText().toStdString(), ".txt"), config_map);
	    if (dialog->exec()) {
	      m_cache_compatible_model_grid = std::tuple<std::string, std::string, bool>{"","",false};
	      m_cache_compatible_galactic_grid= std::tuple<std::string, std::string, bool>{"","",false};
	      m_cache_compatible_shift_grid= std::tuple<std::string, std::string, bool>{"","",false};

	      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "IGM",
	                                          ui->cb_igm->currentText().toStdString());
	      return true; // build succeed
	    } else {
	    	 return false; // build failed
	    }
	  }
}

bool FormAnalysis::BuildMwCorrGrid(){
	if (!checkGalacticGridSelection(true, true)) {
			QMessageBox::warning(this, "Unavailable name...",
			"It is not possible to save the Galactic Correction Grid under the name you have provided. "
			"Please enter a new name.",
			QMessageBox::Ok);
		return false; // grid not generated
	} else {
		if (checkGalacticGridSelection(true, false)) {
			if (QMessageBox::warning(
					this, "Override existing file...",
					"A Galactic Correction Grid file with the very same name as the one you provided already exist. "
					"Do you want to replace it?",
					QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
				return true; // grid already ready
			}
		}

		auto config_map = getGalacticCorrectionGridConfiguration();
		if (config_map.size() > 0) {
			std::unique_ptr<DialogGalCorrGridGeneration> dialog(new DialogGalCorrGridGeneration());
			dialog->setValues(FileUtils::addExt(ui->cb_CompatibleGalCorrGrid->currentText().toStdString(), ".txt"),
			config_map);
			if (dialog->exec()) {
				m_cache_compatible_model_grid = std::tuple<std::string, std::string, bool>{"","",false};
				m_cache_compatible_galactic_grid= std::tuple<std::string, std::string, bool>{"","",false};
				m_cache_compatible_shift_grid= std::tuple<std::string, std::string, bool>{"","",false};
				return true; // build succeed
			} else {
				return false; // build failed
			}
		} else {
			return false; // No grid needed
		}
	}
}

bool FormAnalysis::BuildFilterShiftGrid(){
	if (!checkFilterShiftGridSelection(true, true)) {
						 QMessageBox::warning(this, "Unavailable name...",
						 "It is not possible to save the Filter Variation Coefficients Grid under the name you have "
						 "provided. Please enter a new name.",
						 QMessageBox::Ok);
		return false; // grid not generated
	} else {
		if (checkFilterShiftGridSelection(true, false)) {
		  if (QMessageBox::warning(this, "Override existing file...",
								   "A Filter Variation Coefficients  Grid file with the very same name as the one you "
								   "provided already exist. "
								   "Do you want to replace it?",
								   QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
			 return true; // grid already ready
		  }
		}

		auto config_map = getFilterShiftGridConfiguration();
		if (config_map.size() > 0) {

		  std::unique_ptr<DialogFilterShiftGridGeneration> dialog(new DialogFilterShiftGridGeneration());
		  dialog->setValues(FileUtils::addExt(ui->cb_CompatibleShiftGrid->currentText().toStdString(), ".txt"), config_map);
		  if (dialog->exec()) {
		      m_cache_compatible_model_grid = std::tuple<std::string, std::string, bool>{"","",false};
			  m_cache_compatible_galactic_grid= std::tuple<std::string, std::string, bool>{"","",false};
			  m_cache_compatible_shift_grid= std::tuple<std::string, std::string, bool>{"","",false};
			  return true; // build succeed
		  } else {
		      return false; // build failed
		  }
		} else {
			return false; // No grid needed
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////    Run analysis, preparation and action       /////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


std::list<std::string> FormAnalysis::getFilters() {
  std::list<std::string> res;
  auto                   survey = m_survey_model_ptr->getSelectedSurvey();
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
  auto                     filterNames     = getSelectedFilters();
  auto                     filter_mappings = m_survey_model_ptr->getSelectedSurvey().getFilters();
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

  QFileInfo info(QString::fromStdString(
                     FileUtils::getPhotmetricGridRootPath(false, ui->cb_AnalysisSurvey->currentText().toStdString())) +
                 QDir::separator() + QString::fromStdString(file_name));
  return acceptNewFile || (info.exists());
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

  QFileInfo info(QString::fromStdString(FileUtils::getGalacticCorrectionGridRootPath(
                     true, ui->cb_AnalysisSurvey->currentText().toStdString())) +
                 QDir::separator() + QString::fromStdString(file_name));

  return acceptNewFile || info.exists();
}

bool FormAnalysis::checkFilterShiftGridSelection(bool addFileCheck, bool acceptNewFile) {
  std::string file_name = ui->cb_CompatibleShiftGrid->currentText().toStdString();

  if (file_name.compare("<Enter a new name>") == 0) {
    return false;
  }

  if (file_name.compare("") == 0) {
    return false;
  }

  if (!addFileCheck) {
    return true;
  }

  QFileInfo info(QString::fromStdString(
                     FileUtils::getFilterShiftGridRootPath(true, ui->cb_AnalysisSurvey->currentText().toStdString())) +
                 QDir::separator() + QString::fromStdString(file_name));

  return acceptNewFile || info.exists();
}

std::map<std::string, boost::program_options::variable_value> FormAnalysis::getGridConfiguration(const std::list<float>& zs) {
  std::string file_name = FileUtils::addExt(ui->cb_CompatibleGrid->currentText().toStdString(), ".txt");
  ui->cb_CompatibleGrid->setItemText(ui->cb_CompatibleGrid->currentIndex(), QString::fromStdString(file_name));
  auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();

  auto config = PhzGridInfoHandler::GetConfigurationMap(
      ui->cb_AnalysisSurvey->currentText().toStdString(), file_name, selected_model, getFilters(),
      ui->lbl_lum_filter->text().toStdString(), ui->cb_igm->currentText().toStdString(), zs);

  auto cosmo_conf = PreferencesUtils::getCosmologyConfigurations();
  for (auto& pair : cosmo_conf) {
    config[pair.first] = pair.second;
  }

  std::string text_format                    = "TEXT";
  config["output-model-grid-format"].value() = boost::any(text_format);

  return config;
}

std::map<std::string, boost::program_options::variable_value> FormAnalysis::getGalacticCorrectionGridConfiguration() {
  std::string file_name = FileUtils::addExt(ui->cb_CompatibleGalCorrGrid->currentText().toStdString(), ".txt");
  ui->cb_CompatibleGalCorrGrid->setItemText(ui->cb_CompatibleGalCorrGrid->currentIndex(),
                                            QString::fromStdString(file_name));
  std::string grid_name    = ui->cb_CompatibleGrid->currentText().toStdString();
  std::string catalog_type = ui->cb_AnalysisSurvey->currentText().toStdString();
  std::string lum_filter   = ui->lbl_lum_filter->text().toStdString();
  std::string igm          = ui->cb_igm->currentText().toStdString();
  std::string mwrc         = ui->cb_MWRC->currentText().toStdString();


  QFileInfo g23_curve_info(QString::fromStdString(FileUtils::getRedCurveRootPath(false)) + QDir::separator() +
                           QString::fromStdString("Gordon23") + QDir::separator() + QString::fromStdString("G23.dat"));
  QFileInfo f99_curve_info(QString::fromStdString(FileUtils::getRedCurveRootPath(false)) + QDir::separator() +
                           QString::fromStdString("F99") + QDir::separator() + QString::fromStdString("F99_3.1.dat"));



  std::string mwrc_arg = "Gordon23/G23";
  if (mwrc=="Fitzpatrick 1999") {
	  if (!f99_curve_info.exists()) {
	      QMessageBox::warning(
	          this, "Missing Reddening curve...",
	          "The Milky Way reddening curve stored by default in <ReddeningCurves>/F99/F99_3.1.dat is missing. "
	          "This computation need it, please provide it and try again. (You may try to reload the last data pack)",
	          QMessageBox::Ok);
	      return {};
	  }
	  mwrc_arg = "F99/F99_3.1";
  } else {
	  if (!g23_curve_info.exists()) {
	      QMessageBox::warning(
	          this, "Missing Reddening curve...",
	          "The Milky Way reddening curve stored by default in <ReddeningCurves>/Gordon23/G23.dat is missing. "
	          "This computation need it, please provide it and try again. (You may try to reload the last data pack)",
	          QMessageBox::Ok);
	      return {};
	   }
  }


  std::map<std::string, boost::program_options::variable_value> options_map =
      FileUtils::getPathConfiguration(false, true, true, false);

  options_map["catalog-type"].value()                                = boost::any(catalog_type);
  options_map["output-galactic-correction-coefficient-grid"].value() = boost::any(file_name);

  std::string text_format                                                   = "TEXT";
  options_map["output-galactic-correction-coefficient-grid-format"].value() = boost::any(text_format);

  options_map["model-grid-file"].value()         = boost::any(grid_name);
  options_map["normalization-filter"].value()    = boost::any(lum_filter);
  std::string sun_sed                            = PreferencesUtils::getUserPreference("AuxData", "SUN_SED");
  options_map["normalization-solar-sed"].value() = boost::any(sun_sed);
  options_map["igm-absorption-type"].value()     = boost::any(igm);

  auto cosmo_conf = PreferencesUtils::getCosmologyConfigurations();
  for (auto& pair : cosmo_conf) {
    options_map[pair.first] = pair.second;
  }

  options_map["milky-way-reddening-curve-name"].value() = boost::any(mwrc_arg);
  auto global_options                                   = PreferencesUtils::getThreadConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  global_options = PreferencesUtils::getLogLevelConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  return options_map;
}

std::map<std::string, boost::program_options::variable_value> FormAnalysis::getFilterShiftGridConfiguration() {
  std::map<std::string, boost::program_options::variable_value> options_map =
      FileUtils::getPathConfiguration(false, true, true, false);

  double      min_value        = ui->sp_samp_min->value();
  double      max_value        = ui->sp_samp_max->value();
  int         sample_number    = ui->sp_samp_num->value();
  std::string igm              = ui->cb_igm->currentText().toStdString();
  std::string grid_name        = ui->cb_CompatibleGrid->currentText().toStdString();
  std::string output_grid_name = FileUtils::addExt(ui->cb_CompatibleShiftGrid->currentText().toStdString(), ".txt");
  std::string survey_name      = ui->cb_AnalysisSurvey->currentText().toStdString();
  std::string mwrc         = ui->cb_MWRC->currentText().toStdString();
  std::string mwrc_arg = "Gordon23/G23";
  if (mwrc=="Fitzpatrick 1999") {
 	  mwrc_arg = "F99/F99_3.1";
  }
  std::string text_format      = "TEXT";

  options_map["filter-variation-min-shift"].value()     = boost::any(min_value);
  options_map["filter-variation-max-shift"].value()     = boost::any(max_value);
  options_map["filter-variation-shift-samples"].value() = boost::any(sample_number);

  options_map["milky-way-reddening-curve-name"].value() = boost::any(mwrc_arg);

  auto global_options = PreferencesUtils::getCosmologyConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }
  global_options = PreferencesUtils::getThreadConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  global_options = PreferencesUtils::getLogLevelConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  options_map["catalog-type"].value() = boost::any(survey_name);

  std::string lum_filter                         = ui->lbl_lum_filter->text().toStdString();
  options_map["normalization-filter"].value()    = boost::any(lum_filter);
  std::string sun_sed                            = PreferencesUtils::getUserPreference("AuxData", "SUN_SED");
  options_map["normalization-solar-sed"].value() = boost::any(sun_sed);
  options_map["igm-absorption-type"].value()     = boost::any(igm);
  options_map["model-grid-file"].value()         = boost::any(grid_name);
  options_map["output-filter-variation-coefficient-grid"].value()        = boost::any(output_grid_name);
  options_map["output-filter-variation-coefficient-grid-format"].value() = boost::any(text_format);
  return options_map;
}

bool FormAnalysis::checkSedWeightFile(std::string sed_weight_file_name) {
  std::string folder = FileUtils::getSedPriorRootPath();
  QFileInfo   info(QString::fromStdString(folder) + QDir::separator() + QString::fromStdString(sed_weight_file_name));
  if (info.exists()) {

    logger.info() << "A file with the same name exists " << sed_weight_file_name << " checking if compatible...";
    try {
      auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();
      auto  igm            = ui->cb_igm->currentText().toStdString();
      auto  axis           = selected_model.getAxesTuple();
      logger.debug() << "checkSedWeightFile => selected_model content :" << getAxisDescription(axis);

      std::string file_name = info.absoluteFilePath().toStdString();
      int         hdu_count = 0;
      {
        CCfits::FITS fits{file_name, CCfits::RWmode::Read};
        hdu_count = fits.extension().size();
      }

      std::vector<PhzDataModel::DoubleGrid> sed_weight_grids{};
      for (int i = 1; i <= hdu_count; i += PhzDataModel::DoubleGrid::axisNumber() + 1) {
        sed_weight_grids.emplace_back(GridContainer::gridFitsImport<PhzDataModel::DoubleGrid>(file_name, i));
      }

      std::string survey_name     = ui->cb_AnalysisSurvey->currentText().toStdString();
      std::string model_grid_file = FileUtils::getPhotmetricGridRootPath(true, survey_name) + "/" +
                                    ui->cb_CompatibleGrid->currentText().toStdString();

      PhzDataModel::PhotometryGridInfo model_grid_info;
      std::ifstream                    in{model_grid_file};
      boost::archive::text_iarchive    bia{in};
      bia >> model_grid_info;

      // check the axis
      if (model_grid_info.region_axes_map.size() != sed_weight_grids.size()) {
        return false;
      }

      size_t found = 0;
      for (auto& current_axe : sed_weight_grids) {
        for (auto& file_axe : model_grid_info.region_axes_map) {

          // SED

          auto& sed_axis_file      = std::get<PhzDataModel::ModelParameter::SED>(file_axe.second);
          auto& sed_axis_requested = current_axe.getAxis<PhzDataModel::ModelParameter::SED>();
          if (sed_axis_file.size() != sed_axis_requested.size()) {

            continue;
          }

          bool all_found = true;
          for (auto& sed_requested : sed_axis_requested) {
            if (std::find(sed_axis_file.begin(), sed_axis_file.end(), sed_requested) == sed_axis_file.end()) {
              all_found = false;
            }
          }

          if (!all_found) {

            continue;
          }

          // RED
          auto& red_axis_file      = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(file_axe.second);
          auto& red_axis_requested = current_axe.getAxis<PhzDataModel::ModelParameter::REDDENING_CURVE>();
          if (red_axis_file.size() != red_axis_requested.size()) {

            continue;
          }
          all_found = true;
          for (auto& red_requested : red_axis_requested) {
            if (std::find(red_axis_file.begin(), red_axis_file.end(), red_requested) == red_axis_file.end()) {
              all_found = false;
            }
          }

          if (!all_found) {

            continue;
          }

          std::vector<double> z_axis_file;
          for (double value : std::get<PhzDataModel::ModelParameter::Z>(file_axe.second)) {

            z_axis_file.push_back(value);
          }

          std::vector<double> z_axis_requested;
          for (double value : current_axe.getAxis<PhzDataModel::ModelParameter::Z>()) {
            z_axis_requested.push_back(value);
          }

          if (z_axis_file.size() != z_axis_requested.size()) {
            continue;
          }

          std::sort(z_axis_file.begin(), z_axis_file.end());
          std::sort(z_axis_requested.begin(), z_axis_requested.end());

          bool match     = true;
          auto z_file_it = z_axis_file.begin();
          for (auto& z_requested : z_axis_requested) {
            if (std::fabs(*z_file_it - z_requested) > 1E-10) {
              match = false;
              break;
            }
            ++z_file_it;
          }

          if (!match) {
            continue;
          }

          std::vector<double> ebv_axis_file;
          for (double value : std::get<PhzDataModel::ModelParameter::EBV>(file_axe.second)) {
            ebv_axis_file.push_back(value);
          }

          std::vector<double> ebv_axis_requested;
          for (double value : current_axe.getAxis<PhzDataModel::ModelParameter::EBV>()) {
            ebv_axis_requested.push_back(value);
          }

          if (ebv_axis_file.size() != ebv_axis_requested.size()) {

            continue;
          }

          std::sort(ebv_axis_file.begin(), ebv_axis_file.end());
          std::sort(ebv_axis_requested.begin(), ebv_axis_requested.end());

          auto ebv_file_it = ebv_axis_file.begin();
          for (auto& ebv_requested : ebv_axis_requested) {
            if (std::fabs(*ebv_file_it - ebv_requested) > 1E-10) {
              match = false;
              break;
            }
            ++ebv_file_it;
          }

          if (!match) {

            continue;
          }

          ++found;
          break;
        }
      }

      if (sed_weight_grids.size() != found) {
        return false;
      }

      // Both grid are compatible
      return true;
    } catch (...) {
      logger.warn() << "Wrong format for sed weight file " << sed_weight_file_name;
      return false;
    }

  } else {
    return false;
  }
}

std::string FormAnalysis::getSedWeightFileName() {
  std::string            survey_name           = ui->cb_AnalysisSurvey->currentText().toStdString();
  std::string            model_name            = m_model_set_model_ptr->getSelectedModelSet().getName();
  std::list<std::string> filter_excluded       = getExcludedFilters();
  std::string            list_excluded_filters = "";
  for (auto& filter_item : filter_excluded) {
    list_excluded_filters = list_excluded_filters + filter_item;
  }
  std::size_t h1 = std::hash<std::string>{}(list_excluded_filters);

  return "SED_Prior_" + model_name + "_" + survey_name + "_" + std::to_string(h1) + ".fits";
}

std::map<std::string, boost::program_options::variable_value>
FormAnalysis::getSedWeightOptionMap(std::string output_name) {
  /// Config path
  std::map<std::string, boost::program_options::variable_value> options_map =
      FileUtils::getPathConfiguration(false, true, false, false);
  auto global_options = PreferencesUtils::getLogLevelConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  auto survey_name                    = ui->cb_AnalysisSurvey->currentText().toStdString();
  options_map["catalog-type"].value() = boost::any(survey_name);

  /// Filters
  std::list<std::string>   filters = getSelectedFilters();
  std::vector<std::string> filter_add_vector;
  for (auto& filter_item : filters) {
    filter_add_vector.push_back(filter_item);
  }
  options_map["filter-name"].value() = boost::any(filter_add_vector);

  auto filter_excluded = getExcludedFilters();
  if (filter_excluded.size() > 0) {
    std::vector<std::string> excluded;
    for (auto& filter : filter_excluded) {
      excluded.push_back(filter);
    }
    options_map["exclude-filter"].value() = boost::any(excluded);
  }

  options_map["model-grid-file"].value() = boost::any(ui->cb_CompatibleGrid->currentText().toStdString());

  /// output name
  options_map["SED-Weight-Output"].value() = boost::any(output_name);

  return options_map;
}

////////////////////////////////////////////////////////////////////////////

std::map<std::string, boost::program_options::variable_value> FormAnalysis::getRunOptionMap() {
  std::string yes_flag      = "YES";
  std::string no_flag       = "NO";
  std::string max_flag      = "MAX";
  std::string bayesian_flag = "BAYESIAN";
  double      one           = 1.0;

  auto                survey_name     = ui->cb_AnalysisSurvey->currentText().toStdString();
  SurveyFilterMapping selected_survey = m_survey_model_ptr->getSelectedSurvey();

  std::map<std::string, boost::program_options::variable_value> options_map =
      FileUtils::getPathConfiguration(true, false, true, true);

  auto global_options = PreferencesUtils::getThreadConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  global_options = PreferencesUtils::getLogLevelConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  global_options = PreferencesUtils::getCosmologyConfigurations();
  for (auto& pair : global_options) {
    options_map[pair.first] = pair.second;
  }

  if (ui->cb_skip->isChecked()) {
    int val                                = ui->sb_skip->value();
    options_map["input-skip-head"].value() = boost::any(val);
  }

  if (ui->cb_process_limit->isChecked()) {
    int val                                  = ui->sb_process_limit->value();
    options_map["input-process-max"].value() = boost::any(val);
  }

  options_map["catalog-type"].value() = boost::any(survey_name);

  options_map["phz-output-dir"].value() = boost::any(ui->txt_outputFolder->text().toStdString());

  options_map["model-grid-file"].value() = boost::any(ui->cb_CompatibleGrid->currentText().toStdString());

  // Get the number of sampling
  int sampling_luminosity_number = 1;
  if (ui->rb_sample_scaling->isChecked()) {
    sampling_luminosity_number = ui->sb_lum_sample_number->value();
  }

  // get the n model grid
  auto& selected_model = m_model_set_model_ptr->getSelectedModelSet();
  auto  model_number   = selected_model.getModelNumber();
  options_map["input-buffer-size"].value() =
      boost::any(PreferencesUtils::getCappedBufferSize(model_number, sampling_luminosity_number));

  if (ui->rb_gc_col->isChecked()) {
    options_map["galactic-correction-coefficient-grid-file"].value() =
        boost::any(ui->cb_CompatibleGalCorrGrid->currentText().toStdString());
    options_map["dust-column-density-column-name"].value() = boost::any(selected_survey.getGalEbvColumn());

    options_map["dust-map-sed-bpc"].value() = boost::any(one);

  } else if (ui->rb_gc_planck->isChecked()) {
    options_map["galactic-correction-coefficient-grid-file"].value() =
        boost::any(ui->cb_CompatibleGalCorrGrid->currentText().toStdString());
    std::string gal_ebv_default_column                     = "PLANCK_GAL_EBV";
    options_map["dust-column-density-column-name"].value() = boost::any(gal_ebv_default_column);
  }

  auto input_catalog_file                   = FileUtils::removeStart(ui->txt_inputCatalog->text().toStdString(),
                                                                     FileUtils::getCatalogRootPath(false, survey_name) +
                                                                         QString(QDir::separator()).toStdString());
  options_map["input-catalog-file"].value() = boost::any(input_catalog_file);
  options_map["source-id-column-name"].value() =
      boost::any(m_survey_model_ptr->getSelectedSurvey().getSourceIdColumn());

  options_map["enable-error-adjustment"].value() =
      boost::any(m_survey_model_ptr->getSelectedSurvey().getDoRecomputeError() ? yes_flag : no_flag);

  double non_detection                           = selected_survey.getNonDetection();
  options_map["missing-photometry-flag"].value() = boost::any(non_detection);

  double upper_limit_flin_flag                          = selected_survey.getUpperLimit();
  options_map["enable-upper-limit"].value()             = boost::any(yes_flag);
  options_map["upper-limit-use-threshold-flag"].value() = boost::any(upper_limit_flin_flag);

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
    options_map["photometric-correction-file"].value() =
        boost::any(ui->cb_AnalysisCorrection->currentText().toStdString());
  }

  options_map["axes-collapse-type"].value() = boost::any(bayesian_flag);

  options_map["likelihood-axes-collapse-type"].value() = boost::any(max_flag);

  options_map["output-catalog-format"].value() = boost::any(ui->cb_cat_output_type->currentText().toStdString());

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

  options_map["output-pdf-normalized"].value() = boost::any(yes_flag);

  if (ui->cb_multi_out->currentText() == "Sampling") {
    options_map["full-PDF-sampling"].value() = boost::any(yes_flag);
    options_map["PDF-sample-number"].value() = boost::any(ui->sb_sample_numb->value());
  } else {
    options_map["full-PDF-sampling"].value() = boost::any(no_flag);
  }

  if (ui->cb_gen_posterior->isChecked()) {
    options_map["create-output-posteriors"].value() = boost::any(yes_flag);
  }

  if (ui->rb_luminosityPrior->isChecked()) {
    std::string lum_prior_name   = ui->cb_luminosityPrior_2->currentText().toStdString();
    auto&       lum_prior_config = m_prior_config.at(lum_prior_name);
    auto        lum_prior_option = lum_prior_config.getConfigOptions();
    options_map.insert(lum_prior_option.begin(), lum_prior_option.end());
    options_map["luminosity-prior-effectiveness"].value() = boost::any(one);

    options_map["volume-prior"].value()               = boost::any(yes_flag);
    options_map["volume-prior-effectiveness"].value() = boost::any(one);
  }

  if (ui->rb_volumePrior->isChecked()) {
    options_map["volume-prior"].value()               = boost::any(yes_flag);
    options_map["volume-prior-effectiveness"].value() = boost::any(one);
  }

  if (ui->rb_nzPrior->isChecked()) {
    options_map["Nz-prior"].value() = boost::any(yes_flag);
    std::string nz_prior_b_filter =
        PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                            ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorBFilter");
    options_map["Nz-prior_B_Filter"].value() = boost::any(nz_prior_b_filter);

    std::string nz_prior_i_filter =
        PreferencesUtils::getUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                            ui->cb_AnalysisModel->currentText().toStdString() + "_NzPriorIFilter");
    options_map["Nz-prior_I_Filter"].value()      = boost::any(nz_prior_i_filter);
    options_map["Nz-prior-effectiveness"].value() = boost::any(one);
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

  if (ui->cb_sedweight->isChecked()) {
    std::string              sed_prior_name = getSedWeightFileName();
    std::vector<std::string> tablename{"SedWeight/" +
                                       boost::filesystem::path{sed_prior_name}.filename().stem().string() + ".fits"};
    options_map["generic-grid-prior"].value() = boost::any(tablename);
  }

  if (m_copied_columns.size() > 0) {
    std::string option = "";
    bool        first  = true;
    for (auto& copied_column : m_copied_columns) {
      if (!first) {
        option = option + ",";
      } else {
        first = false;
      }
      option = option + copied_column.first;
      if (copied_column.second != "") {
        option = option + ":" + copied_column.second;
      }
    }

    options_map["copy-columns"].value() = boost::any(option);
  }

  if (m_survey_model_ptr->getSelectedSurvey().getDefineFilterShift()) {
    std::string filter_grid = ui->cb_CompatibleShiftGrid->currentText().toStdString();
    options_map["filter-variation-coefficient-grid-file"].value() = boost::any(filter_grid);
  }

  if (ui->rb_sample_scaling->isChecked()) {
    options_map["scale-factor-marginalization-enabled"].value() = boost::any(yes_flag);

    options_map["scale-factor-marginalization-sample-number"].value() = boost::any(ui->sb_lum_sample_number->value());

    options_map["scale-factor-marginalization-range-size"].value() = boost::any(ui->dsb_sample_range->value());

    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_ScaleSamplingNumber",
                                        QString::number(ui->sb_lum_sample_number->value()).toStdString());

    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(),
                                        ui->cb_AnalysisModel->currentText().toStdString() + "_ScaleSamplingRange",
                                        QString::number(ui->dsb_sample_range->value()).toStdString());
  }

  std::string lum_filter                         = ui->lbl_lum_filter->text().toStdString();
  options_map["normalization-filter"].value()    = boost::any(lum_filter);
  std::string sun_sed                            = PreferencesUtils::getUserPreference("AuxData", "SUN_SED");
  options_map["normalization-solar-sed"].value() = boost::any(sun_sed);

  std::string corrected_phot                         = ui->cb_corr_phot->currentText().toStdString();
  options_map["output-corrected-photometry"].value() = boost::any(corrected_phot);

  return options_map;
}



void FormAnalysis::setInputCatalogName(std::string name, bool do_test) {
  if (do_test) {
    std::vector<std::string> needed_columns{};

    needed_columns.push_back(m_survey_model_ptr->getSelectedSurvey().getSourceIdColumn());
    for (auto& filter : getSelectedFilterMapping()) {
      needed_columns.push_back(filter.getFluxColumn());
      needed_columns.push_back(filter.getErrorColumn());
    }

    std::string missing = FileUtils::checkFileColumns(name, needed_columns);
    if (missing.size() > 0) {
      if (QMessageBox::question(this, "Incompatible Data...",
                                "The catalog file you selected has not the columns described into the Catalog and "
                                "therefore cannot be used. \n"
                                "Missing column(s):" +
                                    QString::fromStdString(missing) +
                                    "\n"
                                    "Do you want to create a new Catalog mapping for this file?",
                                QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Ok) {
        navigateToNewCatalog(name);
      }
      return;
    }
  }

  ui->txt_inputCatalog->setText(QString::fromStdString(name));
  PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "LAST_USED_CAT", name);

  auto                   column_reader = PhzUITools::CatalogColumnReader(ui->txt_inputCatalog->text().toStdString());
  auto                   col_set       = column_reader.getColumnNames();
  std::list<std::string> all_columns(col_set.begin(), col_set.end());
  updateCopiedColumns(all_columns);

  QFileInfo info(QString::fromStdString(name));
  ui->txt_outputFolder->setText(QString::fromStdString(FileUtils::getResultRootPath(
      false, ui->cb_AnalysisSurvey->currentText().toStdString(), info.completeBaseName().toStdString())));
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



void FormAnalysis::saveCopiedColumnToCatalog() {
  m_survey_model_ptr->setCopiedColumnsToSelected(m_copied_columns);
  m_survey_model_ptr->saveSelected();
}




void FormAnalysis::on_btn_GetConfigAnalysis_clicked() {

  m_httpRequestAborted = false;

  if (ui->rb_gc_planck->isChecked()) {
    auto path             = ui->txt_inputCatalog->text().toStdString();
    auto column_reader    = PhzUITools::CatalogColumnReader(path);
    auto column_from_file = column_reader.getColumnNames();
    if (column_from_file.find("PLANCK_GAL_EBV") == column_from_file.end()) {
      if (!boost::filesystem::exists(m_planck_file)) {
        if (QMessageBox::Ok ==
            QMessageBox::question(
                this, "Missing Dust map file...",
                "The file containing the Milky Way dust map is missing (" + QString::fromStdString(m_planck_file) +
                    ").   \n Do you want to download it now from \n" + QString::fromStdString(m_planck_url) +
                    "? \n\n This is needed before using the Milky Way absorption correction option that you choose.",
                QMessageBox::Ok | QMessageBox::Cancel)) {

          QDir enclosing_folder = QFileInfo(QString::fromStdString(m_planck_file)).absoluteDir();
          if (!enclosing_folder.exists()) {
            enclosing_folder.mkpath(".");
          }

          m_network_manager = new QNetworkAccessManager(this);
          QUrl url(QString::fromStdString(m_planck_url));
          m_downloaded_file = new QFile(QString::fromStdString(m_planck_file));
          if (!m_downloaded_file->open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("HTTP"),
                                     tr("Unable to save the file %1: %2.")
                                         .arg(QString::fromStdString(m_planck_file))
                                         .arg(m_downloaded_file->errorString()));
            delete m_downloaded_file;
            m_downloaded_file = 0;
            return;
          }

          logger.debug() << "Preparing the progress dialog ";
          if (m_progress_dialog) {
            logger.debug() << "Cleaning the progress dialog ";
            delete m_progress_dialog;
            m_progress_dialog = 0;
          }

          m_progress_dialog = new QProgressDialog(this);
          m_progress_dialog->setWindowTitle(tr("Dowwnloading... "));
          m_progress_dialog->setLabelText(tr("Downloading Planck dust map"));
          m_progress_dialog->setWindowModality(Qt::WindowModal);

          logger.debug() << "Connecting  the progress dialog ";
          connect(m_progress_dialog, SIGNAL(canceled()), this, SLOT(cancelDownloadPlanck()));

          logger.debug() << "Getting the reply ";
          m_reply = m_network_manager->get(QNetworkRequest(url));
          connect(m_reply, SIGNAL(readyRead()), this, SLOT(httpReadyPlanckRead()));
          connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this,
                  SLOT(updateDownloadProgress(qint64, qint64)));
          connect(m_reply, SIGNAL(finished()), this, SLOT(get_config_run_second_part()));
        } else {
          return;
        }
      } else {
        get_config_run_second_part();
      }
    } else {
      get_config_run_second_part();
    }
  } else {
    get_config_run_second_part();
  }
}

void FormAnalysis::get_config_run_second_part() {
  // when it was a download and it was canceled
  if (m_httpRequestAborted) {
    if (m_downloaded_file) {
      m_downloaded_file->close();
      m_downloaded_file->remove();
      delete m_downloaded_file;
      m_downloaded_file = 0;
    }
    m_reply->deleteLater();
    m_progress_dialog->hide();
    return;
  } else if (m_downloaded_file) {
    m_progress_dialog->hide();
    m_downloaded_file->flush();
    m_downloaded_file->close();
    if (m_reply->error()) {
      m_downloaded_file->remove();
      m_reply->deleteLater();
      m_reply = 0;
      delete m_downloaded_file;
      m_downloaded_file = 0;
      m_network_manager = 0;
      return;
    }

    m_reply->deleteLater();
    m_reply = 0;
    delete m_downloaded_file;
    m_downloaded_file = 0;
    m_network_manager = 0;
  }

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setDirectory(QString::fromStdString(FileUtils::getRootPath(true)) + "config");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setLabelText(QFileDialog::Accept, "Select Folder");
  if (dialog.exec()) {

	std::list<float> zs{};
	if (ui->gb_fix_z->isChecked()) {

		std::string  new_grid_name =  ui->cb_CompatibleGrid->currentText().toStdString();

		if (!new_grid_name.rfind("TEMP_", 0) == 0) {
		    new_grid_name = "TEMP_" + new_grid_name;
		}

		ui->cb_CompatibleGrid->setItemText(ui->cb_CompatibleGrid->currentIndex(),QString::fromStdString(new_grid_name));

		std::unique_ptr<DialogExtractZ> dialogZ(new DialogExtractZ());
		int start = 0;
		if (ui->cb_skip->isChecked()) {
		    start = ui->sb_skip->value();
		}
		int max_num=-1;
		if (ui->cb_process_limit->isChecked()) {
		    max_num = ui->sb_process_limit->value();
		}

		dialogZ->setData(ui->txt_inputCatalog->text().toStdString(), ui->cb_z_col->currentText().toStdString(), start, max_num, ui->dsb_fix_z_tol->value(), ui->rb_scaleZTol->isChecked());
		if (dialogZ->exec()) {
		    zs = dialogZ->getRedshiftList();
		} else {
		    cleanTempGrids(false);
		  return;
		}

	}

    auto selected_folder = dialog.selectedFiles()[0];

    QString cr{"\n\n"};
    QString command{""};

    // Model Grid
    auto grid_model_file_name  = selected_folder + QString::fromStdString("/ModelGrid.CMG.conf");
    auto grid_model_config_map = getGridConfiguration(zs); // TODO replace by the z list if needed
    PhzUITools::ConfigurationWriter::writeConfiguration(grid_model_config_map, grid_model_file_name.toStdString());
    command += QString::fromStdString("Phosphoros CMG --config-file ") + grid_model_file_name + cr;

    // GalCorr Grid
    if (!ui->rb_gc_off->isChecked()) {
      auto grid_galactic_corr_file_name = selected_folder + QString::fromStdString("/GalacticCorrGrid.CGCCG.conf");
      auto galactic_corr_config_map     = getGalacticCorrectionGridConfiguration();
      PhzUITools::ConfigurationWriter::writeConfiguration(galactic_corr_config_map,
                                                          grid_galactic_corr_file_name.toStdString());
      command += QString::fromStdString("Phosphoros CGCCG --config-file ") + grid_galactic_corr_file_name + cr;
    }

    // FilterShift Grid
    if (m_survey_model_ptr->getSelectedSurvey().getDefineFilterShift()) {
      auto filter_shift_corr_file_name =
          selected_folder + QString::fromStdString("/FilterVariationCoefficientGrid.CFVCG.conf");
      auto filter_shift_config_map = getFilterShiftGridConfiguration();
      PhzUITools::ConfigurationWriter::writeConfiguration(filter_shift_config_map,
                                                          filter_shift_corr_file_name.toStdString());
      command += QString::fromStdString("Phosphoros CFVCG --config-file ") + filter_shift_corr_file_name + cr;
    }

    // Sed weight
    if (ui->cb_sedweight->isChecked()) {
      auto        grid_sed_weight_file_name = selected_folder + QString::fromStdString("/SedWeightGrid.CSW.conf");
      std::string sed_prior_name            = getSedWeightFileName();
      auto        sed_weight_config_map     = getSedWeightOptionMap(sed_prior_name);
      PhzUITools::ConfigurationWriter::writeConfiguration(sed_weight_config_map,
                                                          grid_sed_weight_file_name.toStdString());
      command += QString::fromStdString("Phosphoros CSW --config-file ") + grid_sed_weight_file_name + cr;
    }

    // Lookup Galactic EBV
    if (ui->rb_gc_planck->isChecked()) {
      auto path             = ui->txt_inputCatalog->text().toStdString();
      auto column_reader    = PhzUITools::CatalogColumnReader(path);
      auto column_from_file = column_reader.getColumnNames();
      if (column_from_file.find("PLANCK_GAL_EBV") == column_from_file.end()) {
        // the E(B-V) has to be looked up in the Planck map
        SurveyFilterMapping selected_survey = m_survey_model_ptr->getSelectedSurvey();
        std::map<std::string, boost::program_options::variable_value> add_column_options_map{};
        add_column_options_map["planck-dust-map"].value() = boost::any(m_planck_file);
        add_column_options_map["galatic-ebv-col"].value() = boost::any(std::string("PLANCK_GAL_EBV"));
        add_column_options_map["input-catalog"].value()   = boost::any(path);
        add_column_options_map["ra"].value()              = boost::any(selected_survey.getRaColumn());
        add_column_options_map["dec"].value()             = boost::any(selected_survey.getDecColumn());
        add_column_options_map["output-catalog"].value()  = boost::any(path);
        auto lookup_planck_file_name = selected_folder + QString::fromStdString("/LookupGalacticEBV.AGDD.conf");
        PhzUITools::ConfigurationWriter::writeConfiguration(add_column_options_map,
                                                            lookup_planck_file_name.toStdString());
        command += QString::fromStdString("Phosphoros AGDD --config-file ") + lookup_planck_file_name + cr;
      }
    }

    // Template fitting
    auto template_fitting_file_name = selected_folder + QString::fromStdString("/TemplateFitting.CR.conf");
    auto run_config_map             = getRunOptionMap();

    auto pp_conf_file_name = selected_folder + QString::fromStdString("/PhysicalParameterConfig.fits");

    auto pp_list = getPPListFromConfig();
    if (pp_list.size() > 0) {
      SedParamUtils::createPPConfig(m_model_set_model_ptr->getSelectedModelSet(), pp_list,
                                    pp_conf_file_name.toStdString());
      std::string pp_conf_file                                 = pp_conf_file_name.toStdString();
      run_config_map["physical_parameter_config_file"].value() = boost::any(pp_conf_file);
    }

    PhzUITools::ConfigurationWriter::writeConfiguration(run_config_map, template_fitting_file_name.toStdString());
    command += QString::fromStdString("Phosphoros CR --config-file ") + template_fitting_file_name + cr;

    // command file
    auto          command_file_name = selected_folder + QString::fromStdString("/command");
    std::ofstream file;
    file.open(command_file_name.toStdString());
    file << command.toStdString();
    file.close();

    saveCopiedColumnToCatalog();
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "IGM",
                                        ui->cb_igm->currentText().toStdString());
    if (ui->gb_corrections->isChecked()) {
      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "Correction",
                                          ui->cb_AnalysisCorrection->currentText().toStdString());
    }
  }

  cleanTempGrids(false);
}

void FormAnalysis::on_btn_RunAnalysis_clicked() {
  m_httpRequestAborted = false;
  if (ui->rb_gc_planck->isChecked()) {
    auto path             = ui->txt_inputCatalog->text().toStdString();
    auto column_reader    = PhzUITools::CatalogColumnReader(path);
    auto column_from_file = column_reader.getColumnNames();
    if (column_from_file.find("PLANCK_GAL_EBV") == column_from_file.end()) {
      if (!boost::filesystem::exists(m_planck_file)) {
        if (QMessageBox::Ok ==
            QMessageBox::question(
                this, "Missing Dust map file...",
                "The file containing the Milky Way dust map is missing (" + QString::fromStdString(m_planck_file) +
                    "). \n Do you want to download it now from \n" + QString::fromStdString(m_planck_url) +
                    "? \n\n This is needed before using the Milky Way absorption correction option that you choose.",
                QMessageBox::Ok | QMessageBox::Cancel)) {

          QDir enclosing_folder = QFileInfo(QString::fromStdString(m_planck_file)).absoluteDir();
          if (!enclosing_folder.exists()) {
            enclosing_folder.mkpath(".");
          }

          m_network_manager = new QNetworkAccessManager(this);
          QUrl url(QString::fromStdString(m_planck_url));
          m_downloaded_file = new QFile(QString::fromStdString(m_planck_file));
          if (!m_downloaded_file->open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("HTTP"),
                                     tr("Unable to save the file %1: %2.")
                                         .arg(QString::fromStdString(m_planck_file))
                                         .arg(m_downloaded_file->errorString()));
            delete m_downloaded_file;
            m_downloaded_file = 0;
            return;
          }

          if (m_progress_dialog) {
            delete m_progress_dialog;
            m_progress_dialog = 0;
          }
          m_progress_dialog = new QProgressDialog(this);
          m_progress_dialog->setWindowTitle(tr("Dowwnloading... "));
          m_progress_dialog->setLabelText(tr("Downloading Planck dust map"));
          m_progress_dialog->setWindowModality(Qt::WindowModal);

          connect(m_progress_dialog, SIGNAL(canceled()), this, SLOT(cancelDownloadPlanck()));

          m_reply = m_network_manager->get(QNetworkRequest(url));
          connect(m_reply, SIGNAL(readyRead()), this, SLOT(httpReadyPlanckRead()));
          connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this,
                  SLOT(updateDownloadProgress(qint64, qint64)));
          connect(m_reply, SIGNAL(finished()), this, SLOT(run_analysis_second_part()));

        } else {
          return;
        }
      } else {
        run_analysis_second_part();
      }
    } else {
      run_analysis_second_part();
    }
  } else {
    run_analysis_second_part();
  }
}

void FormAnalysis::updateDownloadProgress(qint64 bytesRead, qint64 totalBytes) {
  if (m_httpRequestAborted) {
    return;
  }

  m_progress_dialog->setMaximum(totalBytes);
  m_progress_dialog->setValue(bytesRead);
}

void FormAnalysis::httpReadyPlanckRead() {
  if (m_downloaded_file)
    m_downloaded_file->write(m_reply->readAll());
}

void FormAnalysis::cancelDownloadPlanck() {
  m_httpRequestAborted = true;
  if (m_reply) {
    m_reply->abort();
  }
}

void FormAnalysis::run_analysis_second_part() {

  // when it was a download and it was canceled
  if (m_httpRequestAborted) {
    if (m_downloaded_file) {
      m_downloaded_file->close();
      m_downloaded_file->remove();
      delete m_downloaded_file;
      m_downloaded_file = 0;
    }
    m_reply->deleteLater();
    m_progress_dialog->hide();
    return;
  } else if (m_downloaded_file) {
    m_progress_dialog->hide();
    m_downloaded_file->flush();
    m_downloaded_file->close();
    if (m_reply->error()) {
      m_downloaded_file->remove();
      m_reply->deleteLater();
      m_reply = 0;
      delete m_downloaded_file;
      m_downloaded_file = 0;
      m_network_manager = 0;
      return;
    }

    m_reply->deleteLater();
    m_reply = 0;
    delete m_downloaded_file;
    m_downloaded_file = 0;
    m_network_manager = 0;
  }

  std::list<float> zs{};
  if (ui->gb_fix_z->isChecked()) { // TODO add  a temp name

	  std::string  new_grid_name =  ui->cb_CompatibleGrid->currentText().toStdString();

	  if (!new_grid_name.rfind("TEMP_", 0) == 0) {
		  new_grid_name = "TEMP_" + new_grid_name;
	  }

	  ui->cb_CompatibleGrid->setItemText(ui->cb_CompatibleGrid->currentIndex(),QString::fromStdString(new_grid_name));

	  std::unique_ptr<DialogExtractZ> dialogZ(new DialogExtractZ());
	  int start = 0;
	  if (ui->cb_skip->isChecked()) {
		  start = ui->sb_skip->value();
	  }
	  int max_num=-1;
	  if (ui->cb_process_limit->isChecked()) {
		  max_num = ui->sb_process_limit->value();
	  }

	  dialogZ->setData(ui->txt_inputCatalog->text().toStdString(), ui->cb_z_col->currentText().toStdString(), start, max_num, ui->dsb_fix_z_tol->value(), ui->rb_scaleZTol->isChecked());
	  if (dialogZ->exec()) {
		  zs = dialogZ->getRedshiftList();
	  } else {
		  cleanTempGrids();
		  return;
	  }

  }

  // Build the Model Grid if needed

    if (!checkGridSelection(true, false)) {
 	 if (!BuildModelGrid(zs)) {
 		cleanTempGrids();
 		return;
 	 }
    }

   // Build MW correction grid if needed
   bool need_gal_correction       = !ui->rb_gc_off->isChecked();
   bool has_gal_corr_grid         = checkGalacticGridSelection(true, false);
   if (need_gal_correction && (!has_gal_corr_grid || !checkCompatibleGalacticGrid(ui->cb_CompatibleGalCorrGrid->currentText().toStdString()))){
 	  if (!BuildMwCorrGrid()) {
 		 cleanTempGrids();
 		 return;
 		 }
   }

   // build filter shift grid if needed
   bool need_filter_shift_grid    = m_survey_model_ptr->getSelectedSurvey().getDefineFilterShift();
   bool has_filter_shift_grid     = checkFilterShiftGridSelection(true, false);
   if (need_filter_shift_grid && (!has_filter_shift_grid || !checkCompatibleFilterShiftGrid(ui->cb_CompatibleShiftGrid->currentText().toStdString()))) {
 	  if (!BuildFilterShiftGrid()) {
 		 cleanTempGrids();
 		 return;
 	  }
   }




  auto config_map = getRunOptionMap();

  if (ui->rb_gc_planck->isChecked()) {
    // User requests that we lookup Planck EBV from the position:
    // Check if the catalog contains the "GAL_EBV" column

    auto path             = ui->txt_inputCatalog->text().toStdString();
    auto column_reader    = PhzUITools::CatalogColumnReader(path);
    auto column_from_file = column_reader.getColumnNames();
    if (column_from_file.find("PLANCK_GAL_EBV") == column_from_file.end()) {
      // the E(B-V) has to be looked up in the Planck map
      SurveyFilterMapping              selected_survey = m_survey_model_ptr->getSelectedSurvey();
      std::unique_ptr<DialogAddGalEbv> dialog(new DialogAddGalEbv());
      dialog->setInputs(path, selected_survey.getRaColumn(), selected_survey.getDecColumn(), m_planck_file);
      if (dialog->exec()) {
        // new catalog contains the GAL_EBV column

        auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();
        auto input_catalog_file =
            FileUtils::removeStart(dialog->getOutputName(), FileUtils::getCatalogRootPath(false, survey_name) +
                                                                QString(QDir::separator()).toStdString());

        config_map["input-catalog-file"].value() = boost::any(input_catalog_file);
      } else {
        // user has canceled the operation
    	  cleanTempGrids();
    	  return;
      }
    }
  }

  std::string out_dir = ui->txt_outputFolder->text().toStdString();

  QDir dir(QString::fromStdString(out_dir));
  if (dir.exists()) {
    if (dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() > 0) {
      if (QMessageBox::question(
              this, "Existing Output Folder...",
              "The Output Folder you selected already exists.\n"
              "In order to avoid confusion, the Output Folder will be cleared. Do you want to proceed?",
              QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Ok) {
        FileUtils::removeDir(QString::fromStdString(out_dir));
        dir.mkpath(".");
      } else {
    	  cleanTempGrids();
    	  return;
      }
    }
  } else {
    dir.mkpath(".");
  }

  std::map<std::string, boost::program_options::variable_value> config_sed_weight{};
  if (ui->cb_sedweight->isChecked()) {
    // Compute the SED Prior
    std::string sed_prior_name = getSedWeightFileName();

    if (!checkSedWeightFile(sed_prior_name)) {
      config_sed_weight = getSedWeightOptionMap(sed_prior_name);
    }
  }

  auto pp_conf_file_name = QString::fromStdString(out_dir) + QString::fromStdString("/PhysicalParameterConfig.fits");
  auto pp_list           = getPPListFromConfig();
  if (pp_list.size() > 0) {
    /* for (auto& param : pp_list) {
       if (pp_param_list.find(param) == pp_param_list.end()) {
          QMessageBox::question(this, "Physical Parameter Configuration Problem...",
                          "Please check the Parameter selection:\n"
                          "At least one selected parameter is not available. Did you edit the model after the selection
     of the parameters?", QMessageBox::Ok); return;
       }
     }
     */

    SedParamUtils::createPPConfig(m_model_set_model_ptr->getSelectedModelSet(), pp_list,
                                  pp_conf_file_name.toStdString());
    std::string pp_conf_file                             = pp_conf_file_name.toStdString();
    config_map["physical_parameter_config_file"].value() = boost::any(pp_conf_file);
  }

  std::unique_ptr<DialogRunAnalysis> dialog(new DialogRunAnalysis());
  dialog->setValues(out_dir, config_map, config_sed_weight);
  if (dialog->exec()) {
    saveCopiedColumnToCatalog();
    PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "IGM",
                                        ui->cb_igm->currentText().toStdString());

    if (ui->gb_corrections->isChecked()) {
      PreferencesUtils::setUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "Correction",
                                          ui->cb_AnalysisCorrection->currentText().toStdString());
    } else {
      PreferencesUtils::clearUserPreference(ui->cb_AnalysisSurvey->currentText().toStdString(), "Correction");
    }
  }
  cleanTempGrids();
}

void FormAnalysis::cleanTempGrids(bool test_files){
	std::string model_grid = ui->cb_CompatibleGrid->currentText().toStdString();
	std::string mw_grid = ui->cb_CompatibleGalCorrGrid->currentText().toStdString();
	std::string fv_grid = ui->cb_CompatibleShiftGrid->currentText().toStdString();

	// Reset the names
	if (model_grid.rfind("TEMP_", 0) == 0) {
		std::string new_grid_name = model_grid.substr(5);
		ui->cb_CompatibleGrid->setItemText(ui->cb_CompatibleGrid->currentIndex(),QString::fromStdString(new_grid_name));
	}

	// check if the files exists
	std::string survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();
	QFile mg_info(QString::fromStdString( FileUtils::getPhotmetricGridRootPath(false,survey_name)) + QDir::separator() + QString::fromStdString(model_grid));
	QFile mwg_info(QString::fromStdString( FileUtils::getGalacticCorrectionGridRootPath(false,survey_name)) + QDir::separator() + QString::fromStdString(mw_grid));
    QFile fvg_info(QString::fromStdString( FileUtils::getFilterShiftGridRootPath(false, survey_name)) + QDir::separator() + QString::fromStdString(fv_grid));

	if ( test_files && (mg_info.exists() || mwg_info.exists() || fvg_info.exists())) {
		if (QMessageBox::warning(this, "Temporary grid deletion...",
			 					"Temporary grids have been produced for this run. "
			 					"Unless you want to rerun with the same sources it is better to delete them. "
			 					"Do you want to delete these grids? ",
			 					QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
			mg_info.remove();
			mwg_info.remove();
			fvg_info.remove();
		}
	}
}



}  // namespace PhzQtUI
}  // namespace Euclid
