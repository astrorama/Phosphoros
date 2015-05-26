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
#include "PhzQtUI/FileUtils.h"
#include "PhzQtUI/ModelSet.h"
#include "PhzQtUI/PhotometricCorrectionHandler.h"
#include "PhzQtUI/SurveyFilterMapping.h"
#include "PhzQtUI/PhzGridInfoHandler.h"
#include "PhzQtUI/DialogGridGeneration.h"
#include "PhzQtUI/DialogRunAnalysis.h"

#include "PhzUITools/ConfigurationWriter.h"



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


}
///////////////////////////////////////////////////
//  Handle controls enability


void FormAnalysis::updateGridSelection() {

  ModelSet selected_model;

  for (auto&model : m_analysis_model_list) {
    if (model.second.getName().compare(
        ui->cb_AnalysisModel->currentText().toStdString()) == 0) {
      selected_model = model.second;
      break;
    }
  }

  auto axis = PhzGridInfoHandler::getAxesTuple( selected_model);
  auto possible_files =
      PhzGridInfoHandler::getCompatibleGridFile(axis,
          getSelectedFilters(true),
          ui->cb_igm->currentText().toStdString());

  ui->cb_CompatibleGrid->clear();
  bool added=false;
  for (auto& file : possible_files) {
    ui->cb_CompatibleGrid->addItem(QString::fromStdString(file));
    added=true;
  }

  if (!added){
    std::string concatenated_filter_names="";
     for (auto filter : getSelectedFilters(false)) {
       concatenated_filter_names=concatenated_filter_names+filter;
     }

    ui->cb_CompatibleGrid->addItem(ui->cb_AnalysisSurvey->currentText()+QString::fromStdString("_"+selected_model.getName()+"_")+ui->cb_igm->currentText()+QString::fromStdString("_"+concatenated_filter_names));
  }
  ui->cb_CompatibleGrid->addItem("<Enter a new name>");
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

void FormAnalysis::adjustPhzGridButtons(bool enabled){
    bool name_ok = checkGridSelection(false, true);
    ui->btn_GetConfigGrid->setEnabled(enabled && name_ok);
    ui->btn_RunGrid->setEnabled(enabled && name_ok);
    QString tool_tip = "";

    if (!name_ok){
      ui->cb_CompatibleGrid->lineEdit()->setStyleSheet("QLineEdit { color: red }");
      setToolBoxButtonColor(ui->toolBox, 0, Qt::red);
      tool_tip = "Please enter a valid grid name in order to compute the Grid or export the corresponding configuration.";
    } else if (!checkGridSelection(true, false)) {
      ui->cb_CompatibleGrid->lineEdit()->setStyleSheet("QLineEdit { color: orange }");
      setToolBoxButtonColor(ui->toolBox, 0, QColor("orange"));

    } else {
      ui->cb_CompatibleGrid->lineEdit()->setStyleSheet("QLineEdit { color: black }");
      setToolBoxButtonColor(ui->toolBox, 0, ui->label_18->palette().color(QPalette::Window));
    }
    ui->btn_GetConfigGrid->setToolTip(tool_tip);
    ui->btn_RunGrid->setToolTip(tool_tip);


}

void FormAnalysis::setComputeCorrectionEnable(){
  bool name_exists = checkGridSelection(true, false);
    ui->btn_computeCorrections->setEnabled(name_exists && ui->gb_corrections->isChecked () );

    QString tool_tip = "Open the photometric zero-point correction popup.";
    if (!name_exists){
      tool_tip = "Please run the photometric grid computation before computing the photometric corrections.";
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
          FileUtils::getPhotCorrectionsRootPath(false))
          + QDir::separator() + ui->cb_AnalysisCorrection->currentText());
  bool correction_exists = !ui->gb_corrections->isChecked() || info.exists();

  QFileInfo info_input(ui->txt_inputCatalog->text());
  bool run_ok = info_input.exists();

  if (ui->gb_cat->isChecked()){
    run_ok &= ui->txt_OutputCatalog->text().length()>0;
  }

  if (ui->gb_pdf->isChecked()){
    run_ok &= ui->txt_OutputPdf->text().length()>0;
  }

  if (ui->gb_lik->isChecked()){
    run_ok &= ui->txt_likelihood->text().length()>0;
  }



  ui->btn_GetConfigAnalysis->setEnabled(grid_name_ok && correction_ok && run_ok && enabled);
  ui->btn_RunAnalysis->setEnabled(grid_name_exists && correction_exists && run_ok && enabled);


  QString tool_tip_run = "";
  QString tool_tip_conf = "";
  if (!grid_name_ok){
    tool_tip_conf = tool_tip_conf + "Please enter a valid grid name. \n";
    tool_tip_run = tool_tip_run +"Please enter a valid grid name. \n";
  };

  if (!grid_name_exists){
    tool_tip_run = tool_tip_run + "Please run the model grid computation. \n";
  }

  if (!correction_ok){
    tool_tip_conf = tool_tip_conf + "When the photometric corrections are enabled, you must provide a valid correction file name. \n";
    tool_tip_run = tool_tip_run +"When the photometric corrections are enabled, you must provide a valid correction file name. \n";
  }

  if (!correction_exists){
     tool_tip_run = tool_tip_run + "Please run the photometric correction computation. \n";
  }

  bool missing_input=false;
  if (!info_input.exists()){
    ui->txt_inputCatalog->setStyleSheet("QLineEdit { color: #F78181 }");
     tool_tip_conf = tool_tip_conf + "Please provide a compatible input catalog (at least all the columns used for the Id and filters). \n";
     tool_tip_run = tool_tip_run + "Please provide a compatible input catalog (at least all the columns used for the Id and filters). \n";
     missing_input=true;
  } else {
     ui->txt_inputCatalog->setStyleSheet("QLineEdit { color: grey }");
   }

  bool missing_path=false;
  if (ui->gb_cat->isChecked()){
    if (ui->txt_OutputCatalog->text().length()==0){
        tool_tip_conf = tool_tip_conf + "Please provide the output catalog file. \n";
        tool_tip_run = tool_tip_run + "Please provide the output catalog file. \n";
        missing_path=true;
    } else {
      if (QFileInfo(ui->txt_OutputCatalog->text()).exists()){
        ui->txt_OutputCatalog->setStyleSheet("QLineEdit { color: #FAAC58 }");
        missing_path=true;
      } else {
        ui->txt_OutputCatalog->setStyleSheet("QLineEdit { color: grey }");
      }
    }
  } else {
    ui->txt_OutputCatalog->setStyleSheet("QLineEdit { color: grey }");
  }

  if (ui->gb_pdf->isChecked()){
    if (ui->txt_OutputPdf->text().length()==0){
        tool_tip_conf = tool_tip_conf + "Please provide the output pdf file. \n";
        tool_tip_run = tool_tip_run + "Please provide the output pdf file. \n";
        missing_path=true;
    } else {
      if (QFileInfo(ui->txt_OutputPdf->text()).exists()){
        ui->txt_OutputPdf->setStyleSheet("QLineEdit { color: #FAAC58 }");
        missing_path=true;
      } else {
        ui->txt_OutputPdf->setStyleSheet("QLineEdit { color: grey }");
      }
    }
  } else {
    ui->txt_OutputPdf->setStyleSheet("QLineEdit { color: grey }");
  }

  if (ui->gb_lik->isChecked()){
     if (ui->txt_likelihood->text().length()==0){
         tool_tip_conf = tool_tip_conf + "Please provide the likelihood output folder. \n";
         tool_tip_run = tool_tip_run + "Please provide the likelihood output folder. \n";
         missing_path=true;
     }
  }


  if (!(grid_name_ok && correction_ok && run_ok)){
    tool_tip_conf = tool_tip_conf + "Before getting the configuration.";
  } else {
    tool_tip_conf =  "Get the configuration file.";
  }

  if (!(grid_name_exists && correction_exists && run_ok)){
    tool_tip_run = tool_tip_run + "Before running the analysis.";
  }else {
    tool_tip_run =  "Run the analysis.";
  }


  ui->btn_GetConfigAnalysis->setToolTip(tool_tip_conf);
  ui->btn_RunAnalysis->setToolTip(tool_tip_run);


  if (!correction_ok){
    setToolBoxButtonColor(ui->toolBox, 1, QColor("orange"));
  } else {
    setToolBoxButtonColor(ui->toolBox, 1, ui->label_18->palette().color(QPalette::Window));
  }

//  if (missing_path || missing_input){
//    setToolBoxButtonColor(ui->toolBox, 3, QColor("orange"));
//  } else {
//    setToolBoxButtonColor(ui->toolBox, 3, ui->label_18->palette().color(QPalette::Window));
//  }
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
  if (!ui->cb_AnalysisSurvey->currentText().isEmpty()){
    auto model = static_cast<QStandardItemModel*>(ui->tableView_filter->model());
    for (int i = 0; i < model->rowCount(); ++i) {
      auto item = model->item(i);
      if( item->checkState() == Qt::CheckState::Checked){
          res.push_back(model->item(i, return_path)->text().toStdString());
      }
    }
  }
  return res;
}

std::list<FilterMapping> FormAnalysis::getSelectedFilterMapping(){
  auto filterNames=getSelectedFilters(false);
  std::list<FilterMapping>  list;
  if (!ui->cb_AnalysisSurvey->currentText().isEmpty()){
    for(auto& survey: m_analysis_survey_list){
      if (survey.second.getName().compare(ui->cb_AnalysisSurvey->currentText().toStdString())==0){
        for (auto& name : filterNames){
          for (auto& filter : survey.second.getFilters()){
            if(filter.getName().compare(name)==0){
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
          FileUtils::getPhotmetricGridRootPath(false))
          + QDir::separator() + QString::fromStdString(file_name));

  return acceptNewFile || info.exists();
}

std::map<std::string, boost::program_options::variable_value> FormAnalysis::getGridConfiguration(){
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
             file_name, axes, getSelectedFilters(true),
             ui->cb_igm->currentText().toStdString());
}

std::map < std::string, boost::program_options::variable_value > FormAnalysis::getRunOptionMap(){
  std::vector<std::string> filter_mappings;
    for (auto& filter : getSelectedFilterMapping()){
      filter_mappings.push_back(filter.getFilterFile()+" "+filter.getFluxColumn()+" "+filter.getErrorColumn());
    }

   auto path_grid_filename = FileUtils::getPhotmetricGridRootPath(false)
                 + QString(QDir::separator()).toStdString() + ui->cb_CompatibleGrid->currentText().toStdString();

   std::map < std::string, boost::program_options::variable_value > options_map;
   options_map["model-grid-file"].value() = boost::any(path_grid_filename);
   options_map["input-catalog-file"].value() = boost::any(ui->txt_inputCatalog->text().toStdString());

   options_map["source-id-column-name"].value() = boost::any(getSelectedSurveySourceColumn());

   options_map["filter-name-mapping"].value() = boost::any(filter_mappings);


   if (ui->gb_corrections->isChecked ()){
     auto path_correction_filename = FileUtils::getPhotCorrectionsRootPath(true)
               + QString(QDir::separator()).toStdString() + ui->cb_AnalysisCorrection->currentText().toStdString();
     options_map["photometric-correction-file"].value() = boost::any(path_correction_filename);
   }

   options_map["axes-collapse-type"].value() = boost::any(ui->cb_marginalization->currentText().toStdString());

   if (ui->gb_cat->isChecked ()){
     options_map["output-catalog-file"].value() = boost::any(ui->txt_OutputCatalog->text().toStdString());
     options_map["output-catalog-format"].value() = boost::any(ui->cb_cat_output_type->currentText().toStdString());
   }

   if (ui->gb_pdf->isChecked ()){
     options_map["output-pdf-file"].value() = boost::any(ui->txt_OutputPdf->text().toStdString());
   }

   if (ui->gb_lik->isChecked ()){
       options_map["output-likelihood-dir"].value() = boost::any(ui->txt_likelihood->text().toStdString());
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
        QString::fromStdString(filter.getName()));
    item->setCheckable(true);
    item->setCheckState(Qt::CheckState::Checked);
    QStandardItem* file_item = new QStandardItem(
        QString::fromStdString(filter.getFilterFile()));

    QList<QStandardItem*> items;
    items.push_back(item);
    items.push_back(file_item);

    grid_model->appendRow(items);
  }

  ui->tableView_filter->setModel(grid_model);
  connect( grid_model, SIGNAL(itemChanged(QStandardItem*)),
      SLOT(onFilterSelectionItemChanged(QStandardItem*)));

  // push the default catalog
  ui->txt_inputCatalog->setText(QString::fromStdString(selected_survey.getDefaultCatalog()));

  updateGridSelection();
  updateCorrectionSelection();
}

void FormAnalysis::on_cb_AnalysisModel_currentIndexChanged(const QString &) {
  updateGridSelection();
}

void FormAnalysis::on_cb_igm_currentIndexChanged(const QString &)
{
 updateGridSelection();
}

void FormAnalysis::onFilterSelectionItemChanged(QStandardItem*) {
  updateCorrectionSelection();

  updateGridSelection();
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

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Configuration File"),QString::fromStdString(FileUtils::getLastUsedPath())
                               ,tr("Config (*.conf)"));
      if (fileName.length()>0){
          FileUtils::setLastUsedPath(fileName.toStdString());
          auto config_map = getGridConfiguration();
          PhzUITools::ConfigurationWriter::writeConfiguration(config_map,fileName.toStdString());

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
      selected_survey.getDefaultCatalog());

  connect( popup.get(), SIGNAL(correctionComputed(const QString &)),
      SLOT(onCorrectionComputed(const QString &)));
  popup->exec();

}


void FormAnalysis::onCorrectionComputed(const QString & new_file_name){
  updateCorrectionSelection();
  ui->cb_AnalysisCorrection->setCurrentIndex(ui->cb_AnalysisCorrection->findText(new_file_name));
}

// 4. Run

void FormAnalysis::on_btn_BrowseInput_clicked()
{
  QFileDialog dialog(this);
  std::string path = ui->txt_inputCatalog->text().toStdString();
  if (path.length()==0){
    path=FileUtils::getLastUsedPath();
  }
  dialog.selectFile(QString::fromStdString(path));
  dialog.setFileMode(QFileDialog::ExistingFile);
  if (dialog.exec()){
    ui->txt_inputCatalog->setText(dialog.selectedFiles()[0]);
    FileUtils::setLastUsedPath(dialog.selectedFiles()[0].toStdString());
    setRunAnnalysisEnable(true);
  }

}


 void FormAnalysis::on_gb_cat_clicked(){
   setRunAnnalysisEnable(true);
 }

 void FormAnalysis::on_gb_pdf_clicked(){
   setRunAnnalysisEnable(true);
 }

 void FormAnalysis::on_gb_lik_clicked(){
   if (ui->gb_lik->isChecked ()){
     QMessageBox::warning(this, "Large output volume...",
              "Outputing multi-dimensional likelihood grids (one file per source)"
         " will generate a large output volume.", QMessageBox::Ok );
   }
   setRunAnnalysisEnable(true);
 }

 void FormAnalysis::on_btn_BrowseLikelihood_clicked(){
       QFileDialog dialog(this);
       dialog.setFileMode(QFileDialog::DirectoryOnly);
       dialog.selectFile(QString::fromStdString(FileUtils::getLastUsedPath()));
       dialog.setOption(QFileDialog::DontUseNativeDialog);
       dialog.setLabelText( QFileDialog::Accept, "Select" );
       if (dialog.exec()){
         ui->txt_likelihood->setText(dialog.selectedFiles()[0]);
         FileUtils::setLastUsedPath(dialog.selectedFiles()[0].toStdString());
         setRunAnnalysisEnable(true);
       }
 }


void FormAnalysis::on_cb_cat_output_type_currentIndexChanged(const QString &){
  std::string current_text = ui->txt_OutputCatalog->text().toStdString();
  std::string new_text ="";
  std::string old_ext = ".fits";
  std::string new_ext = ".txt";
  if (ui->cb_cat_output_type->currentText()=="FITS"){
    old_ext = ".txt";
    new_ext = ".fits";
  }

  auto pos = current_text.find_last_of(old_ext);
  if (pos!=std::string::npos){
    new_text=current_text.substr(0,pos+1-old_ext.length())+new_ext;
  }

  ui->txt_OutputCatalog->setText(QString::fromStdString(new_text));
  setRunAnnalysisEnable(true);
}

void FormAnalysis::on_btn_BrowseOutput_clicked()
{
  QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.selectFile(QString::fromStdString(FileUtils::getLastUsedPath()));
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (ui->cb_cat_output_type->currentText()=="FITS"){
      dialog.setNameFilter("FITS-Files (*.fits)");
      dialog.setDefaultSuffix("fits");
    } else {
      dialog.setNameFilter("Text-Files (*.txt)");
      dialog.setDefaultSuffix("txt");
    }
    dialog.setLabelText( QFileDialog::Accept, "Select" );
    if (dialog.exec()){
      ui->txt_OutputCatalog->setText(dialog.selectedFiles()[0]);
      FileUtils::setLastUsedPath(dialog.selectedFiles()[0].toStdString());
      setRunAnnalysisEnable(true);
    }
}

void FormAnalysis::on_btn_BrowseOutputPdf_clicked()
{
  QFileDialog dialog(this);
      dialog.setFileMode(QFileDialog::AnyFile);
      dialog.selectFile(QString::fromStdString(FileUtils::getLastUsedPath()));
      dialog.setOption(QFileDialog::DontUseNativeDialog);
      dialog.setNameFilter("FITS-Files (*.fits)");
      dialog.setDefaultSuffix("fits");
      dialog.setLabelText( QFileDialog::Accept, "Select" );
      if (dialog.exec()){
        ui->txt_OutputPdf->setText(dialog.selectedFiles()[0]);
        FileUtils::setLastUsedPath(dialog.selectedFiles()[0].toStdString());
        setRunAnnalysisEnable(true);
      }
}

void FormAnalysis::on_btn_GetConfigAnalysis_clicked()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Configuration File"),
      QString::fromStdString(FileUtils::getLastUsedPath()),tr("Config (*.conf)"));
   if (fileName.length()>0){
       FileUtils::setLastUsedPath(fileName.toStdString());
       auto config_map = getRunOptionMap();
       PhzUITools::ConfigurationWriter::writeConfiguration(config_map,fileName.toStdString());
   }
}

void FormAnalysis::on_btn_RunAnalysis_clicked()
{
  if (ui->gb_cat->isChecked()) {
    if (QFileInfo(ui->txt_OutputCatalog->text()).exists()
        && QMessageBox::question(this, "Override existing file...",
            "A Catalog file with the very same name as the one you provided already exist. Do you want to replace it?",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
      return;
    }
  }

  if (ui->gb_pdf->isChecked()) {
    if (QFileInfo(ui->txt_OutputPdf->text()).exists()
        && QMessageBox::question(this, "Override existing file...",
            "A PDF file with the very same name as the one you provided already exist. Do you want to replace it?",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
      return;
    }
  }

  auto config_map = getRunOptionMap();

  std::unique_ptr<DialogRunAnalysis> dialog(new DialogRunAnalysis());
  std::string cat="";
  if (ui->gb_cat->isChecked()){
    cat=ui->txt_OutputCatalog->text().toStdString();
  }
  std::string pdf="";
  if (ui->gb_pdf->isChecked()){
      pdf=ui->txt_OutputPdf->text().toStdString();
    }
  std::string lik="";
  if (ui->gb_lik->isChecked()){
      lik=ui->txt_likelihood->text().toStdString();
    }
  dialog->setValues(cat,pdf,lik,config_map);
  if (dialog->exec()) {
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
        p.setColor(QPalette::Button, color);
        button->setPalette(p);
        break;
      }
      i++;
    }
  }
}

}
}
