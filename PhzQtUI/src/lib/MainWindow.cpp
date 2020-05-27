#include <QDir>
#include <fstream>
#include <QProcess>
#include <QFileInfo>
#include <QMessageBox>
#include "FileUtils.h"
#include "PhzQtUI/MainWindow.h"
#include "ui_MainWindow.h"
#include "XYDataset/AsciiParser.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <stdio.h>
#include "PhzQtUI/DialogConflictingFilesHandling.h"

#include "ThisProject.h"             // for the name and version of this very project

namespace Euclid {
namespace PhzQtUI {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
  QString title = QString::fromStdString(THIS_PROJECT_NAME_STRING+ " " + THIS_PROJECT_VERSION_STRING);

  setWindowTitle(title);

  ui->setupUi(this);
  this->layout()->setSpacing(0);
  this->layout()->setContentsMargins(0, 0, 0, 0);
  ui->mainStackedWidget->setContentsMargins(0, 0, 0, 0);
//  ui->page_layout->setSpacing(0);
//  ui->page_layout->setContentsMargins(0,0,0,0);
//  this->setStyleSheet("background-color: green");

  QPixmap pixmap(":/logoPhUI.png");
  ui->image_label->setTopMargin(20);
  ui->image_label->setPixmap(pixmap);

  connect(this, SIGNAL(changeMainStackedWidgetIndex(int)), ui->mainStackedWidget, SLOT(setCurrentIndex(int)));


  connect(ui->widget_ModelSet, SIGNAL(navigateToConfig()), SLOT(navigateToConfig()));
  connect(ui->widget_ModelSet, SIGNAL(navigateToCatalog(bool)), SLOT(navigateToCatalog(bool)));
  connect(ui->widget_ModelSet, SIGNAL(navigateToComputeRedshift(bool)), SLOT(navigateToComputeRedshift(bool)));
  connect(ui->widget_ModelSet, SIGNAL(navigateToPostProcessing(bool)), SLOT(navigateToPostProcessing(bool)));
  connect(ui->widget_ModelSet, SIGNAL(quit(bool)), SLOT(quit(bool)));

  connect(ui->widget_Catalog, SIGNAL(navigateToConfig()), SLOT(navigateToConfig()));
  connect(ui->widget_Catalog, SIGNAL(navigateToParameter(bool)), SLOT(navigateToParameter(bool)));
  connect(ui->widget_Catalog, SIGNAL(navigateToComputeRedshift(bool)), SLOT(navigateToComputeRedshift(bool)));
  connect(ui->widget_Catalog, SIGNAL(navigateToPostProcessing(bool)), SLOT(navigateToPostProcessing(bool)));
  connect(ui->widget_Catalog, SIGNAL(quit(bool)), SLOT(quit(bool)));

  connect(ui->widget_configuration, SIGNAL(navigateToCatalog(bool)), SLOT(navigateToCatalog(bool)));
  connect(ui->widget_configuration, SIGNAL(navigateToParameter(bool)), SLOT(navigateToParameter(bool)));
  connect(ui->widget_configuration, SIGNAL(navigateToComputeRedshift(bool)), SLOT(navigateToComputeRedshift(bool)));
  connect(ui->widget_configuration, SIGNAL(navigateToPostProcessing(bool)), SLOT(navigateToPostProcessing(bool)));
  connect(ui->widget_configuration, SIGNAL(quit(bool)), SLOT(quit(bool)));

  connect(ui->widget_Analysis, SIGNAL(navigateToCatalog(bool)), SLOT(navigateToCatalog(bool)));
  connect(ui->widget_Analysis, SIGNAL(navigateToParameter(bool)), SLOT(navigateToParameter(bool)));
  connect(ui->widget_Analysis, SIGNAL(navigateToConfig()), SLOT(navigateToConfig()));
  connect(ui->widget_Analysis, SIGNAL(navigateToPostProcessing(bool)), SLOT(navigateToPostProcessing(bool)));
  connect(ui->widget_Analysis, SIGNAL(quit(bool)), SLOT(quit(bool)));

  connect(ui->widget_postprocessing, SIGNAL(navigateToCatalog(bool)), SLOT(navigateToCatalog(bool)));
  connect(ui->widget_postprocessing, SIGNAL(navigateToParameter(bool)), SLOT(navigateToParameter(bool)));
  connect(ui->widget_postprocessing, SIGNAL(navigateToConfig()), SLOT(navigateToConfig()));
  connect(ui->widget_postprocessing, SIGNAL(navigateToComputeRedshift(bool)), SLOT(navigateToComputeRedshift(bool)));
  connect(ui->widget_postprocessing, SIGNAL(quit(bool)), SLOT(quit(bool)));

  connect(ui->widget_Analysis, SIGNAL(navigateToNewCatalog(std::string)), SLOT(navigateToNewCatalog(std::string)));




  auto root_path = QString::fromStdString(FileUtils::getRootPath(false));
  QFileInfo info(root_path);
  if (!info.exists()) {
    if (QMessageBox::Apply== QMessageBox::question(this, "Missing Phosphoros directory...",
        "The Phosphoros directory (specified by the environment variable 'PHOSPHOROS_ROOT') is missing.\n "
        "Click 'Apply' to create the directory '"+root_path+"' or 'Abort' to close Phosphoros.",
        QMessageBox::Apply|QMessageBox::Abort)) {
      QDir().mkpath(root_path);
    } else {
      exit(0);
    }
  }

  FileUtils::buildDirectories();

  /*  DataPack handling */
  auto dataPackVersionFile =  boost::filesystem::path(FileUtils::getGUIConfigPath()+"/dp_version.json");
  std::remove(dataPackVersionFile.c_str());
  auto get_version_process =new QProcess ;

  connect(get_version_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(getDPVersionFinished(int, QProcess::ExitStatus)));

   QStringList s3;
   s3
   /*dbg*/   << QString::fromStdString("--repo-url") << QString::fromStdString("http://localhost:8001")
      << QString::fromStdString("--output-version-match") <<  QString::fromStdString((dataPackVersionFile).string());

  // get_version_process->setReadChannelMode(QProcess::MergedChannels);
   const QString& command = QString("Phosphoros UDP ") + s3.join(" ");
   get_version_process->start(command);
   /*  DataPack handling */





}

MainWindow::~MainWindow() {
}



void MainWindow::loadAuxData() {
   std::unique_ptr <XYDataset::FileParser > filter_file_parser {new XYDataset::AsciiParser { } };
   std::unique_ptr<XYDataset::FileSystemProvider> filter_provider(
       new XYDataset::FileSystemProvider{FileUtils::getFilterRootPath(true),
       std::move(filter_file_parser) });
   m_filter_repository.reset(new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>(std::move(filter_provider)));
   m_filter_repository->reload();

   std::unique_ptr <XYDataset::FileParser > sed_file_parser {new XYDataset::AsciiParser { } };
   std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(true),
     std::move(sed_file_parser) });
   m_seds_repository.reset(new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>(std::move(sed_provider)));
   m_seds_repository->reload();

   std::unique_ptr <XYDataset::FileParser > reddening_file_parser {new XYDataset::AsciiParser { } };
   std::unique_ptr<XYDataset::FileSystemProvider> red_curve_provider(
       new XYDataset::FileSystemProvider{FileUtils::getRedCurveRootPath(true),
       std::move(reddening_file_parser) });
   m_redenig_curves_repository.reset(
       new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>{std::move(red_curve_provider)});
   m_redenig_curves_repository->reload();

   std::unique_ptr <XYDataset::FileParser > luminosity_file_parser {new XYDataset::AsciiParser { } };
   std::unique_ptr<XYDataset::FileSystemProvider> luminosity_curve_provider(
       new XYDataset::FileSystemProvider{  FileUtils::getLuminosityFunctionCurveRootPath(true),
       std::move(luminosity_file_parser) });
   m_luminosity_repository.reset(new DatasetRepository<
       std::unique_ptr<XYDataset::FileSystemProvider>>{std::move(luminosity_curve_provider)});
   m_luminosity_repository->reload();

   m_option_model_ptr->loadOption(m_filter_repository, m_seds_repository, m_redenig_curves_repository, m_luminosity_repository);
   ui->widget_configuration->loadOptionPage(m_option_model_ptr);

   ui->btn_HomeToAnalysis->setEnabled(true);
   ui->btn_HomeToCatalog->setEnabled(true);
   ui->btn_HomeToModel->setEnabled(true);
   ui->btn_HomeToOption->setEnabled(true);
   ui->btn_HomeToPP->setEnabled(true);


   resetRepo();
}

void MainWindow::resetRepo() {
  m_survey_model_ptr->loadSurvey();




  ui->widget_Catalog->loadMappingPage(m_survey_model_ptr, m_filter_repository, "");

  m_model_set_model_ptr->loadSets();
  ui->widget_ModelSet->loadSetPage(m_model_set_model_ptr, m_seds_repository, m_redenig_curves_repository);

  ui->widget_Analysis->loadAnalysisPage(m_survey_model_ptr, m_model_set_model_ptr, m_filter_repository, m_luminosity_repository);
  ui->widget_postprocessing->loadPostProcessingPage(m_survey_model_ptr);

}

//- Home Page
//  - Slots landing on this page
 void MainWindow::navigateToHome() {
     changeMainStackedWidgetIndex(0);
 }

 void MainWindow::navigateToHomeWithReset(bool reset) {
     changeMainStackedWidgetIndex(0);
     if (reset) {
       resetRepo();
     }
 }

void MainWindow::navigateToParameter(bool reset) {
  if (reset) {
        resetRepo();
  }
  on_btn_HomeToModel_clicked();
}

void MainWindow::navigateToCatalog(bool reset) {
  if (reset) {
        resetRepo();
  }
  on_btn_HomeToCatalog_clicked();
}

void MainWindow::navigateToComputeRedshift(bool reset) {
  if (reset) {
        resetRepo();
  }
  on_btn_HomeToAnalysis_clicked();
}


void MainWindow::navigateToPostProcessing(bool reset) {
  if (reset) {
        resetRepo();
  }

  on_btn_HomeToPP_clicked();
}



///// Data Pack handling

void MainWindow::getDPVersionFinished(int, QProcess::ExitStatus){
  auto version_file = FileUtils::getGUIConfigPath()+"/dp_version.json";

  std::ifstream f(version_file.c_str());
  if (f.good()) {
    boost::property_tree::ptree loadPtreeRoot;
    boost::property_tree::read_json(version_file, loadPtreeRoot);

    std::string remote = loadPtreeRoot.get_child("version_remote").get_value<std::string>();
    std::string local = loadPtreeRoot.get_child("version_local").get_value<std::string>();

    if (remote != local && QMessageBox::Yes == QMessageBox::question(this, "New Data Package Avalable ...",
           QString::fromStdString("The version "+remote+" of the data package is available. Do you want to download it ?"),
           QMessageBox::Yes|QMessageBox::Ignore)) {

        auto dataPackTemp =  FileUtils::getRootPath(true)+"Temp/";
        QFileInfo info(QString::fromStdString(dataPackTemp));
        if (!info.exists()){
             QDir().mkpath(QString::fromStdString(dataPackTemp));
        }

        auto conflict_file =  dataPackTemp + "conflict.json";
        std::remove(conflict_file.c_str());

        auto get_conflict_process =new QProcess ;

        connect(get_conflict_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(getConflictFinished(int, QProcess::ExitStatus)));

         QStringList s3;
         s3
         /*dbg*/   << QString::fromStdString("--repo-url") << QString::fromStdString("http://localhost:8001")
            << QString::fromStdString("--force") <<  QString::fromStdString("True")
           << QString::fromStdString("--download") <<  QString::fromStdString("True")
         << QString::fromStdString("--temp-folder") <<  QString::fromStdString(dataPackTemp)
           << QString::fromStdString("--output-conflict") <<  QString::fromStdString(conflict_file);

         const QString& command = QString("Phosphoros UDP ") + s3.join(" ");
         get_conflict_process->start(command);


    } else {
      loadAuxData();
    }
  } else {
    loadAuxData();
  }
}


void MainWindow::getConflictFinished(int, QProcess::ExitStatus) {
   auto dataPackTemp =  FileUtils::getRootPath(true)+"Temp/";
   auto conflict_file =  dataPackTemp + "conflict.json";
   auto resolution_file =  dataPackTemp + "resolution.json";
   std::remove(resolution_file.c_str());

   QFileInfo info(QString::fromStdString(conflict_file));
   if (info.exists()) {
      std::ifstream in(conflict_file.c_str());
      std::ostringstream sstr;
      sstr << in.rdbuf();
      std::string conflict_content = sstr.str();

      std::unique_ptr<DialogConflictingFilesHandling> popUp(new DialogConflictingFilesHandling());
      popUp->setFilesPath(dataPackTemp, conflict_file, resolution_file);
      popUp->loadConflicts();
      if (popUp->exec() == QDialog::Accepted) {
        auto get_resolution_process =new QProcess ;
        connect(get_resolution_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(getResolutionFinished(int, QProcess::ExitStatus)));

        QStringList s3;
        s3
        /*dbg*/   << QString::fromStdString("--repo-url") << QString::fromStdString("http://localhost:8001")
            << QString::fromStdString("--force") <<  QString::fromStdString("True")
            << QString::fromStdString("--download") <<  QString::fromStdString("True")
            << QString::fromStdString("--temp-folder") <<  QString::fromStdString(dataPackTemp)
            << QString::fromStdString("--conflict-resolution") <<  QString::fromStdString(resolution_file);

        const QString& command = QString("Phosphoros UDP ") + s3.join(" ");
        get_resolution_process->start(command);
      }
   } else {
     loadAuxData();
     QMessageBox::information(this, "Data Package Imported ...",
                        QString::fromStdString("The new version of the data package has been successfully imported."),
                        QMessageBox::Close);
   }
}


void MainWindow::getResolutionFinished(int, QProcess::ExitStatus){
  loadAuxData();
  QMessageBox::information(this, "Data Package Imported ...",
                          QString::fromStdString("The new version of the data package has been successfully imported."),
                          QMessageBox::Close);
}

//Todo (?) confirmation on quit
void MainWindow::quit(bool) {
  this->close();
}

void MainWindow::navigateToConfig() {
  on_btn_HomeToOption_clicked();
}


 void MainWindow::navigateToNewCatalog(std::string new_name) {
   changeMainStackedWidgetIndex(2);
   ui->widget_Catalog->loadMappingPage(m_survey_model_ptr, m_filter_repository, new_name);
 }

 //--------------------------------------------------
 // Option Page
 //  - Slots opening the option tab
 void MainWindow::on_btn_HomeToOption_clicked() {
   changeMainStackedWidgetIndex(4);
 }

//------------------------------------------------
// Model Set Page
//  - Slots landing on this page
void MainWindow::on_btn_HomeToModel_clicked() {
    changeMainStackedWidgetIndex(1);
    ui->widget_ModelSet->updateSelection();
}


void MainWindow::on_btn_HomeToCatalog_clicked() {
  changeMainStackedWidgetIndex(2);
  ui->widget_Catalog->updateSelection();
}


//------------------------------------------------
// Analysis page  Page
//  - Slots landing on this page
void MainWindow::on_btn_HomeToAnalysis_clicked() {
  changeMainStackedWidgetIndex(3);
  ui->widget_Analysis->updateSelection();
}


void MainWindow::on_btn_HomeToPP_clicked(){
  changeMainStackedWidgetIndex(5);
  ui->widget_postprocessing->updateSelection(true);
}

}
}
