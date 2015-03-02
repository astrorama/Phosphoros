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


#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "PhzConfiguration/CreatePhzCatalogConfiguration.h"


namespace Euclid {
namespace PhzQtUI {

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
      SurveyFilterMapping::loadSurveysFromFolder(
          FileUtils::getMappingRootPath(false));

  ui->cb_AnalysisSurvey->clear();
  for (auto& survey : m_analysis_survey_list) {
    ui->cb_AnalysisSurvey->addItem(
        QString::fromStdString(survey.second.getName()));
  }

  m_analysis_model_list =
      ModelSet::loadModelSetsFromFolder(
          FileUtils::getModelRootPath(false));
  ui->cb_AnalysisModel->clear();
  for (auto& model : m_analysis_model_list) {
    ui->cb_AnalysisModel->addItem(
        QString::fromStdString(model.second.getName()));
  }

  updateGridSelection();

  ui->progress_Grid->setValue(0);
  ui->progress_Analysis->setValue(0);

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
   ui->cb_marginalization->setEnabled(enabled);
   ui->btn_BrowseInput->setEnabled(enabled);
   ui->btn_BrowseOutputPdf->setEnabled(enabled);
   ui->btn_BrowseOutput->setEnabled(enabled);
   setRunAnnalysisEnable(enabled);
}

void FormAnalysis::updateGridSelection() {

  ui->progress_Grid->setValue(0);
  ModelSet selected_model;

  for (auto&model : m_analysis_model_list) {
    if (model.second.getName().compare(
        ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
      selected_model = model.second;
      break;
    }
  }

  auto axis = PhzGridInfoHandler::getAxesTuple(
      selected_model);
  auto possible_files =
      PhzGridInfoHandler::getCompatibleGridFile(axis,
          getSelectedFilters(true));

  ui->cb_CompatibleGrid->clear();
  for (auto& file : possible_files) {
    ui->cb_CompatibleGrid->addItem(QString::fromStdString(file));
  }
  ui->cb_CompatibleGrid->addItem("<Enter a new name>");
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
    setComputeCorrectionEnable();
    setRunAnnalysisEnable(true);
  }
}

void FormAnalysis::updateCorrectionSelection(){
  auto filter_map = getSelectedFilters(true);
   auto file_list =
       PhotometricCorrectionHandler::getCompatibleCorrectionFiles(
           filter_map);
   ui->cb_AnalysisCorrection->clear();

   for (auto file : file_list) {
     ui->cb_AnalysisCorrection->addItem(QString::fromStdString(file));
   }
}

void FormAnalysis::setComputeCorrectionEnable(){
  bool name_exists = checkGridSelection(true, false);
    ui->btn_computeCorrections->setEnabled(name_exists && ui->gb_corrections->isChecked () );
}

void FormAnalysis::setRunAnnalysisEnable(bool enabled) {


  bool grid_name_ok = checkGridSelection(false, true);
  bool grid_name_exists = checkGridSelection(true, false);
  bool correction_ok = !ui->gb_corrections->isChecked()
      || ui->cb_AnalysisCorrection->currentText().toStdString().length() > 0;

  QFileInfo info(
      QString::fromStdString(
          FileUtils::getPhotCorrectionsRootPath(false))
          + QDir::separator() + ui->cb_AnalysisCorrection->currentText());
  bool correction_exists = !ui->gb_corrections->isChecked() || info.exists();

  QFileInfo info_input(ui->txt_inputCatalog->text());
  bool run_ok = info_input.exists() && ui->txt_OutputCatalog->text().length()>0 && ui->txt_OutputPdf->text().length()>0;

  ui->btn_GetConfigAnalysis->setEnabled(grid_name_ok && correction_ok && run_ok && enabled);
  ui->btn_RunAnalysis->setEnabled(grid_name_exists && correction_exists && run_ok && enabled);
  ui->progress_Analysis->setValue(0);
}

void FormAnalysis::updateAnalysisProgressBar(size_t step, size_t total) {
  int value = (step * 100) / total;
  ui->progress_Analysis->setValue(value);
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

std::list<FilterMapping> FormAnalysis::getSelectedFilterMapping(){
  auto filterNames=getSelectedFilters(false);
  std::list<FilterMapping>  list;


  for(auto& survey: m_analysis_survey_list){
    if (survey.second.getName().compare(ui->cb_AnalysisSurvey->currentText().toStdString())==0){
      for (auto& name : filterNames){
        for (auto& filter : survey.second.getFilters()){
          if(filter.getName().compare(name)){
            list.push_back(filter);
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
          FileUtils::getPhotmetricGridRootPath(false))
          + QDir::separator() + QString::fromStdString(file_name));

  return acceptNewFile || info.exists();
}

std::map<std::string, po::variable_value> FormAnalysis::getGridConfiguration(){
  std::string file_name = FileUtils::addExt(
               ui->cb_CompatibleGrid->currentText().toStdString(), ".dat");
  ui->cb_CompatibleGrid->setItemText(ui->cb_CompatibleGrid->currentIndex (), QString::fromStdString(file_name));
  ModelSet selected_model;

     for (auto&model : m_analysis_model_list) {
       if (model.second.getName().compare(
           ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
         selected_model = model.second;
         break;
       }
     }

     auto axes = PhzGridInfoHandler::getAxesTuple(
         selected_model);

     return PhzGridInfoHandler::GetConfigurationMap(
             file_name, axes, getSelectedFilters(true));
}

std::map < std::string, po::variable_value > FormAnalysis::getRunOptionMap(){
  std::vector<std::string> filter_mappings;
    for (auto& filter : getSelectedFilterMapping()){
      filter_mappings.push_back(filter.getFilterFile()+" "+filter.getFluxColumn()+" "+filter.getErrorColumn());
    }

   auto path_grid_filename = FileUtils::getPhotmetricGridRootPath(false)
                 + QString(QDir::separator()).toStdString() + ui->cb_CompatibleGrid->currentText().toStdString();

   std::map < std::string, po::variable_value > options_map;
   options_map["photometry-grid-file"].value() = boost::any(path_grid_filename);
   options_map["input-catalog-file"].value() = boost::any(ui->txt_inputCatalog->text().toStdString());

   options_map["source-id-column-name"].value() = boost::any(getSelectedSurveySourceColumn());

   options_map["filter-name-mapping"].value() = boost::any(filter_mappings);


   if (ui->gb_corrections->isChecked ()){
     auto path_correction_filename = FileUtils::getPhotCorrectionsRootPath(true)
               + QString(QDir::separator()).toStdString() + ui->cb_AnalysisCorrection->currentText().toStdString();
     options_map["photometric-correction-file"].value() = boost::any(path_correction_filename);
   }

   options_map["marginalization-type"].value() = boost::any(ui->cb_marginalization->currentText().toStdString());

   options_map["output-catalog-file"].value() = boost::any(ui->txt_OutputCatalog->text().toStdString());
   options_map["output-pdf-file"].value() = boost::any(ui->txt_OutputPdf->text().toStdString());

   return options_map;
}
//////////////////////////////////////////////////
// User interaction
void FormAnalysis::on_btn_AnalysisToHome_clicked() {
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
  updateCorrectionSelection();
}

void FormAnalysis::on_cb_AnalysisModel_currentIndexChanged(const QString &) {
  updateGridSelection();
}

void FormAnalysis::onFilterSelectionItemChanged(QStandardItem*) {
  updateCorrectionSelection();

  updateGridSelection();
}

//  2. Photometry Grid
void FormAnalysis::on_cb_CompatibleGrid_textChanged(const QString &) {
  bool name_ok = checkGridSelection(false, true);
  ui->btn_GetConfigGrid->setEnabled(name_ok);
  ui->btn_RunGrid->setEnabled(name_ok);

  setComputeCorrectionEnable();
  setRunAnnalysisEnable(true);
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
          PhzUITools::ConfigurationWriter::writeConfiguration(config_map,config_fileNames[0].toStdString());

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
   try{
    auto config_map = getGridConfiguration();
    PhzConfiguration::CreatePhotometryGridConfiguration conf { config_map };
    PhzModeling::PhotometryGridCreator creator { conf.getSedDatasetProvider(),
        conf.getReddeningDatasetProvider(), conf.getFilterDatasetProvider() };

    auto param_space = PhzDataModel::createAxesTuple(conf.getZList(),
        conf.getEbvList(), conf.getReddeningCurveList(), conf.getSedList());

    std::function<void(size_t,size_t)> monitor_function = std::bind(&FormAnalysis::updateGridProgressBar, this, std::placeholders::_1, std::placeholders::_2);

    auto grid = creator.createGrid(param_space, conf.getFilterList(),monitor_function);
    auto output = conf.getOutputFunction();
    output(grid);
    }
    catch(...){
      QMessageBox::warning(this, "Error in the computation...",
                        "Sorry, nn error occured during the computation.",
                        QMessageBox::Close);
    }
  }

  enableDisablePage(true);
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
  DialogPhotCorrectionEdition* popup = new DialogPhotCorrectionEdition();
  popup->setCorrectionsFile(
      ui->cb_AnalysisCorrection->currentText().toStdString());
  popup->exec();
}

void FormAnalysis::on_btn_computeCorrections_clicked() {
  auto survey_name = ui->cb_AnalysisSurvey->currentText().toStdString();

  DialogPhotometricCorrectionComputation* popup =
      new DialogPhotometricCorrectionComputation();
  popup->setData(survey_name,getSelectedSurveySourceColumn(),
      ui->cb_AnalysisModel->currentText().toStdString(),
      ui->cb_CompatibleGrid->currentText().toStdString(), getSelectedFilterMapping());

  connect( popup, SIGNAL(correctionComputed(const std::string &)),
      SLOT(onCorrectionComputed(const std::string &)));
  popup->exec();

}


void FormAnalysis::onCorrectionComputed(const std::string & new_file_name){
  updateCorrectionSelection();
  ui->cb_AnalysisCorrection->setCurrentIndex(ui->cb_AnalysisCorrection->findText(QString::fromStdString(new_file_name)));
}

// 4. Run

void FormAnalysis::on_btn_BrowseInput_clicked()
{
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()){
    ui->txt_inputCatalog->setText(dialog.selectedFiles()[0]);
    setRunAnnalysisEnable(true);
  }

}

void FormAnalysis::on_btn_BrowseOutput_clicked()
{
  QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter("Text-Files (*.txt)");
    dialog.setDefaultSuffix("txt");
    if (dialog.exec()){
      ui->txt_OutputCatalog->setText(dialog.selectedFiles()[0]);
      setRunAnnalysisEnable(true);
    }
}

void FormAnalysis::on_btn_BrowseOutputPdf_clicked()
{
  QFileDialog dialog(this);
      dialog.setFileMode(QFileDialog::AnyFile);
      dialog.setNameFilter("FITS-Files (*.fits)");
      dialog.setDefaultSuffix("fits");
      if (dialog.exec()){
        ui->txt_OutputPdf->setText(dialog.selectedFiles()[0]);
        setRunAnnalysisEnable(true);
      }
}

void FormAnalysis::on_btn_GetConfigAnalysis_clicked()
{
  QFileDialog dialog(this);

   dialog.setFileMode(QFileDialog::AnyFile);
   if (dialog.exec()){
       QStringList config_fileNames=dialog.selectedFiles();
       auto config_map = getRunOptionMap();
       PhzUITools::ConfigurationWriter::writeConfiguration(config_map,config_fileNames[0].toStdString());
   }
}

void FormAnalysis::on_btn_RunAnalysis_clicked()
{

  if (QFileInfo(ui->txt_OutputCatalog->text()).exists() && QMessageBox::question(this, "Override existing file...",
               "A Catalog file with the very same name as the one you provided already exist. Do you want to replace it?",
               QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
                 return;
  }

  if (QFileInfo(ui->txt_OutputPdf->text()).exists() && QMessageBox::question(this, "Override existing file...",
                "A PDF file with the very same name as the one you provided already exist. Do you want to replace it?",
                QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
                  return;
   }
  enableDisablePage(false);
  auto config_map = getRunOptionMap();
  PhzConfiguration::CreatePhzCatalogConfiguration conf {config_map};

  auto model_phot_grid = conf.getPhotometryGrid();
  auto marginalization_func = conf.getMarginalizationFunc();

  PhzLikelihood::ParallelCatalogHandler handler {conf.getPhotometricCorrectionMap(),
                                                    model_phot_grid, marginalization_func};
  auto catalog = conf.getCatalog();
  auto out_ptr = conf.getOutputHandler();
  std::function<void(size_t,size_t)> monitor_function = std::bind(&FormAnalysis::updateAnalysisProgressBar, this, std::placeholders::_1, std::placeholders::_2);
  handler.handleSources(catalog.begin(), catalog.end(), *out_ptr, monitor_function);
  enableDisablePage(true);
  ui->progress_Analysis->setValue(100);
}

}
}
