#include <QStandardItemModel>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <functional>
#include <QFileDialog>

#include "PhzConfiguration/CreatePhotometryGridConfiguration.h"
#include "PhzModeling/PhotometryGridCreator.h"

#include "PhzQtUI/FormAnalysis.h"
#include "ui_FormAnalysis.h"
#include "PhzQtUI/DialogPhotCorrectionEdition.h"
#include "PhzQtUI/DialogPhotometricCorrectionComputation.h"
#include "PhzQtUI/FileUtils.h"
#include "PhzQtUI/ModelSet.h"
#include "PhzQtUI/PhotometricCorrectionHandler.h"
#include "PhzQtUI/SurveyFilterMapping.h"
#include "PhzQtUI/PhzGridInfoHandler.h"

#include "PhzUITools/ConfigurationWriter.h"

FormAnalysis::FormAnalysis(QWidget *parent) :
    QWidget(parent), ui(new Ui::FormAnalysis) {
  ui->setupUi(this);
}

FormAnalysis::~FormAnalysis() {
  delete ui;
}

///////////////////////////////////////////////////
//  Initial data load
void FormAnalysis::loadAnalysisPage() {
  m_analysis_survey_list =
      Euclid::PhosphorosUiDm::SurveyFilterMapping::loadSurveysFromFolder(
          Euclid::PhosphorosUiDm::FileUtils::getMappingRootPath(false));

  ui->cb_AnalysisSurvey->clear();
  for (auto& survey : m_analysis_survey_list) {
    ui->cb_AnalysisSurvey->addItem(
        QString::fromStdString(survey.second.getName()));
  }

  m_analysis_model_list =
      Euclid::PhosphorosUiDm::ModelSet::loadModelSetsFromFolder(
          Euclid::PhosphorosUiDm::FileUtils::getModelRootPath(false));
  ui->cb_AnalysisModel->clear();
  for (auto& model : m_analysis_model_list) {
    ui->cb_AnalysisModel->addItem(
        QString::fromStdString(model.second.getName()));
  }

  updateGridSelection();

  ui->progress_Grid->setValue(0);
  // TODO

}
///////////////////////////////////////////////////
//  Handle controls enability
void FormAnalysis::enableDisablePage(bool enabled){
   ui->btn_AnalysisToHome->setEnabled(enabled);
   // 1. Survey and Model
   ui->cb_AnalysisSurvey->setEnabled(enabled);
   ui->cb_AnalysisModel->setEnabled(enabled);
   ui->tableView_filter->setEnabled(enabled);

   // 2. Photometry Grid
   ui->cb_CompatibleGrid->setEnabled(enabled);
   adjustPhzGridButtons(enabled);

   // 3. Photometric Correction
   ui->gb_corrections->setEnabled(enabled);

   // 4. Run
   // TODO
}

void FormAnalysis::updateGridSelection() {

  ui->progress_Grid->setValue(0);
  Euclid::PhosphorosUiDm::ModelSet selected_model;

  for (auto&model : m_analysis_model_list) {
    if (model.second.getName().compare(
        ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
      selected_model = model.second;
      break;
    }
  }

  auto axis = Euclid::PhosphorosUiDm::PhzGridInfoHandler::getAxesTuple(
      selected_model);
  auto possible_files =
      Euclid::PhosphorosUiDm::PhzGridInfoHandler::getCompatibleGridFile(axis,
          getSelectedFilters(true));

  ui->cb_CompatibleGrid->clear();
  ui->cb_CompatibleGrid->addItem("<Enter a new name>");
  for (auto& file : possible_files) {
    ui->cb_CompatibleGrid->addItem(QString::fromStdString(file));
  }
}

void FormAnalysis::adjustPhzGridButtons(bool enabled){
    bool name_ok = checkGridSelection(false, true);
    ui->btn_GetConfigGrid->setEnabled(enabled && name_ok);
    ui->btn_RunGrid->setEnabled(enabled && name_ok);
}

void FormAnalysis::updateGridProgressBar(size_t step, size_t total) {
  int value = (step * 100) / total;
  ui->progress_Grid->setValue(value);
  if(value==100){
    setRunAnnalysisEnable();
  }
}

void FormAnalysis::setRunAnnalysisEnable() {
  // TODO check other constraints (

  bool grid_name_ok = checkGridSelection(false, true);
  bool grid_name_exists = checkGridSelection(true, false);
  bool correction_ok = !ui->gb_corrections->isChecked()
      || ui->cb_AnalysisCorrection->currentText().toStdString().length() > 0;

  QFileInfo info(
      QString::fromStdString(
          Euclid::PhosphorosUiDm::FileUtils::getPhotCorrectionsRootPath(false))
          + QDir::separator() + ui->cb_AnalysisCorrection->currentText());
  bool correction_exists = !ui->gb_corrections->isChecked() || info.exists();

  ui->btn_GetConfigAnalysis->setEnabled(grid_name_ok && correction_ok);
  ui->btn_RunAnalysis->setEnabled(grid_name_exists && correction_exists);
}

//////////////////////////////////////////////////
// Build and handle objects for calling the processing
std::list<std::string> FormAnalysis::getSelectedFilters(bool return_path) {
  std::list<std::string> res;
  auto model = static_cast<QStandardItemModel*>(ui->tableView_filter->model());
  for (int i = 0; i < model->rowCount(); ++i) {
    auto item = model->item(i);
    if( item->checkState() == Qt::CheckState::Checked){
        res.push_back(model->item(i, return_path)->text().toStdString());
    }
  }
  return res;
}

//  - Slot on this page
void FormAnalysis::on_btn_AnalysisToHome_clicked() {
  navigateToHome();
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
          Euclid::PhosphorosUiDm::FileUtils::getPhotmetricGridRootPath(false))
          + QDir::separator() + QString::fromStdString(file_name));

  return acceptNewFile || info.exists();
}

std::map<std::string, po::variable_value> FormAnalysis::getGridConfiguration(){
  std::string file_name = Euclid::PhosphorosUiDm::FileUtils::addExt(
               ui->cb_CompatibleGrid->currentText().toStdString(), ".dat");
  Euclid::PhosphorosUiDm::ModelSet selected_model;

     for (auto&model : m_analysis_model_list) {
       if (model.second.getName().compare(
           ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
         selected_model = model.second;
         break;
       }
     }

     auto axes = Euclid::PhosphorosUiDm::PhzGridInfoHandler::getAxesTuple(
         selected_model);

     return Euclid::PhosphorosUiDm::PhzGridInfoHandler::GetConfigurationMap(
             file_name, axes, getSelectedFilters(true));
}

//////////////////////////////////////////////////
// User interaction
//  1. Survey and Model
void FormAnalysis::on_cb_AnalysisSurvey_currentIndexChanged(
    const QString &selectedName) {

  Euclid::PhosphorosUiDm::SurveyFilterMapping selected_survey;

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
        QString::fromStdString(filter.getName()));
    item->setCheckable(true);
    item->setCheckState(Qt::CheckState::Checked);
    QStandardItem* file_item = new QStandardItem(
        QString::fromStdString(filter.getFilterFile()));

    QList<QStandardItem*> items { { item, file_item } };
    grid_model->appendRow(items);
  }

  ui->tableView_filter->setModel(grid_model);
  connect( grid_model, SIGNAL(itemChanged(QStandardItem*)),
      SLOT(onFilterSelectionItemChanged(QStandardItem*)));

  updateGridSelection();
}

void FormAnalysis::on_cb_AnalysisModel_currentIndexChanged(const QString &) {
  updateGridSelection();
}

void FormAnalysis::onFilterSelectionItemChanged(QStandardItem*) {
  auto filter_map = getSelectedFilters(true);
  auto file_list =
      Euclid::PhosphorosUiDm::PhotometricCorrectionHandler::getCompatibleCorrectionFiles(
          filter_map);
  ui->cb_AnalysisCorrection->clear();

  for (auto file : file_list) {
    ui->cb_AnalysisCorrection->addItem(QString::fromStdString(file));
  }

  updateGridSelection();
}

//  2. Photometry Grid
void FormAnalysis::on_cb_CompatibleGrid_textChanged(const QString &) {
  bool name_ok = checkGridSelection(false, true);
  ui->btn_GetConfigGrid->setEnabled(name_ok);
  ui->btn_RunGrid->setEnabled(name_ok);
  setRunAnnalysisEnable();
}

void FormAnalysis::on_btn_GetConfigGrid_clicked() {
  if (!checkGridSelection(true, true)) {
    QMessageBox::warning(this, "Unavailable name...",
        "It is not possible to save the Grid under the name you have provided. Please enter a new name.",
        QMessageBox::Ok);
  } else {
      QFileDialog dialog(this);

      dialog.setFileMode(QFileDialog::AnyFile);
      if (dialog.exec()){
          QStringList config_fileNames=dialog.selectedFiles();
          auto config_map = getGridConfiguration();
          Euclid::PhzUITools::ConfigurationWriter::writeConfiguration(config_map,config_fileNames[0].toStdString());

      }
  }
}

void FormAnalysis::on_btn_RunGrid_clicked() {
  enableDisablePage(false);

  if (!checkGridSelection(true, true)) {
    QMessageBox::warning(this, "Unavailable name...",
        "It is not possible to save the Grid under the name you have provided. Please enter a new name.",
        QMessageBox::Ok);
  } else {

    auto config_map = getGridConfiguration();
    Euclid::PhzConfiguration::CreatePhotometryGridConfiguration conf { config_map };
    Euclid::PhzModeling::PhotometryGridCreator creator { conf.getSedDatasetProvider(),
        conf.getReddeningDatasetProvider(), conf.getFilterDatasetProvider() };

    auto param_space = Euclid::PhzDataModel::createAxesTuple(conf.getZList(),
        conf.getEbvList(), conf.getReddeningCurveList(), conf.getSedList());

    std::function<void(size_t,size_t)> monitor_function = std::bind(&FormAnalysis::updateGridProgressBar, this, std::placeholders::_1, std::placeholders::_2);

    auto grid = creator.createGrid(param_space, conf.getFilterList(),monitor_function);
    auto output = conf.getOutputFunction();
    output(grid);
  }

  enableDisablePage(true);
}

//  3. Photometric Correction
void FormAnalysis::on_gb_corrections_clicked()
{
    setRunAnnalysisEnable();
}

void FormAnalysis::on_cb_AnalysisCorrection_currentIndexChanged(
    const QString &selectedText) {
  ui->btn_editCorrections->setEnabled(selectedText.length() > 0);
  setRunAnnalysisEnable();
}

void FormAnalysis::on_btn_editCorrections_clicked() {
  DialogPhotCorrectionEdition* popup = new DialogPhotCorrectionEdition();
  popup->setCorrectionsFile(
      ui->cb_AnalysisCorrection->currentText().toStdString());
  popup->exec();
}

void FormAnalysis::on_btn_computeCorrections_clicked() {
  DialogPhotometricCorrectionComputation* popup =
      new DialogPhotometricCorrectionComputation();
  popup->setData(ui->cb_AnalysisSurvey->currentText().toStdString(),
      ui->cb_AnalysisModel->currentText().toStdString(),
      ui->cb_CompatibleGrid->currentText().toStdString(), getSelectedFilters());
  popup->exec();
  // TODO
}

// 4. Run

