#include "PhzQtUI/MainWindow.h"
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include "PreferencesUtils.h"
#include "XYDataset/AsciiParser.h"
#include "ui_MainWindow.h"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QMessageBox>
#include <fstream>
#include <stdio.h>
#include <chrono>
#include <QApplication>

#include "PhzQtUI/DataPackHandler.h"

#include "ThisProject.h"  // for the name and version of this very project

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging main_logger = Elements::Logging::getLogger("MainWindow");

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  main_logger.debug()<<"start loading";
  QString title = QString::fromStdString(THIS_PROJECT_NAME_STRING + " " + THIS_PROJECT_VERSION_STRING);
  setWindowTitle(title);

  ui->setupUi(this);
  this->layout()->setSpacing(0);
  this->layout()->setContentsMargins(0, 0, 0, 0);
  ui->mainStackedWidget->setContentsMargins(0, 0, 0, 0);
  //  ui->page_layout->setSpacing(0);
  //  ui->page_layout->setContentsMargins(0,0,0,0);
  //  this->setStyleSheet("background-color: green");
  ui->Lb_warning_time->setText("");
  QPixmap pixmap(":/logoPhUI.png");
  ui->image_label->setTopMargin(20);
  ui->image_label->setPixmap(pixmap);
  main_logger.debug()<<"UI setup done";

  // DEBUG: List the resources
  // main_logger.info()<<"Resources in ':' ";
  // QDirIterator it(":", QDirIterator::Subdirectories);
  // while (it.hasNext()) {
  //	  main_logger.info()<< it.next().toStdString();
  // }


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


  main_logger.debug()<<"Button wiring done";
  auto      root_path = QString::fromStdString(FileUtils::getRootPath(false));
  QFileInfo info(root_path);
  if (!info.exists()) {
    if (QMessageBox::Apply ==
        QMessageBox::question(
            this, "Missing Phosphoros directory...",
            "The Phosphoros directory (specified by the environment variable 'PHOSPHOROS_ROOT') is missing.\n "
            "Click 'Apply' to create the directory '" +
                root_path + "' or 'Abort' to close Phosphoros.",
            QMessageBox::Apply | QMessageBox::Abort)) {
      QDir().mkpath(root_path);
    } else {
      exit(0);
    }
  }

  FileUtils::buildDirectories();

  /*  DataPack handling */
  m_dataPackHandler.reset(new DataPackHandler(this));
  connect(m_dataPackHandler.get(), SIGNAL(completed()), this, SLOT(loadAuxData()));
  main_logger.debug()<<"Ready to check data pack";
  m_dataPackHandler->check(false);



  std::string sun_sed = PreferencesUtils::getUserPreference("AuxData", "SUN_SED");
  if (sun_sed == "") {
    QMessageBox::warning(this, "Missing Solar SED...",
                         "Phosphoros needs a reference solar spectrum to normalize the models it uses.\n "
                         "Please navigate to Configuration/Aux. Data/SEDs and select it.",
                         QMessageBox::Ok);
  }


  main_logger.debug()<<"loading done";
}

MainWindow::~MainWindow() {}

void MainWindow::loadAuxData() {
  main_logger.info()<<"Loading data";
  try {

	ui->Lb_warning_time->setText("Loading the Filters...");
	qApp->processEvents();
    std::unique_ptr<XYDataset::FileParser>         filter_file_parser{new XYDataset::AsciiParser{}};
    std::unique_ptr<XYDataset::FileSystemProvider> filter_provider(
        new XYDataset::FileSystemProvider{FileUtils::getFilterRootPath(true), std::move(filter_file_parser)});
    m_filter_repository.reset(
        new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>(std::move(filter_provider)));
    m_filter_repository->reload();


	ui->Lb_warning_time->setText("Loading the SEDs...");
	qApp->processEvents();
    std::unique_ptr<XYDataset::FileParser>         sed_file_parser{new XYDataset::AsciiParser{}};
    std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(
        new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(true), std::move(sed_file_parser)});
    m_seds_repository.reset(
        new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>(std::move(sed_provider)));
    m_seds_repository->reload();


	ui->Lb_warning_time->setText("Loading the Reddening Curves...");
	qApp->processEvents();
    std::unique_ptr<XYDataset::FileParser>         reddening_file_parser{new XYDataset::AsciiParser{}};
    std::unique_ptr<XYDataset::FileSystemProvider> red_curve_provider(
        new XYDataset::FileSystemProvider{FileUtils::getRedCurveRootPath(true), std::move(reddening_file_parser)});
    m_redenig_curves_repository.reset(
        new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>{std::move(red_curve_provider)});
    m_redenig_curves_repository->reload();


    m_model_set_model_ptr = std::shared_ptr<ModelSetModel>{new ModelSetModel{m_seds_repository, m_redenig_curves_repository}};


	ui->Lb_warning_time->setText("Loading the Luminosity Functions...");
	qApp->processEvents();
    std::unique_ptr<XYDataset::FileParser>         luminosity_file_parser{new XYDataset::AsciiParser{}};
    std::unique_ptr<XYDataset::FileSystemProvider> luminosity_curve_provider(new XYDataset::FileSystemProvider{
        FileUtils::getLuminosityFunctionCurveRootPath(true), std::move(luminosity_file_parser)});
    m_luminosity_repository.reset(
        new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>{std::move(luminosity_curve_provider)});
    m_luminosity_repository->reload();
    main_logger.info()<<"Loading data done";
	ui->Lb_warning_time->setText("");
	qApp->processEvents();
  } catch (Elements::Exception& e) {
    std::string msg = e.what();
    if (msg.rfind("Qualified name can not be inserted in the map.", 0) == 0) {

      std::string msg_part = msg.replace(0, 61, "");
      std::string name     = msg_part.substr(0, msg_part.find(":") - 5);
      std::string path     = msg_part.substr(msg_part.find(":") + 1);
      path                 = path.substr(0, path.find_last_of("/"));

      QString text = QString::fromStdString(
          std::string("Conflict in the auxiliary data: 2 files contain a dataset with the same name.\n\n"
                      "Dataset name : \n") +
          name + "\n\nPath containing the files : \n" + path + "\n\n" +
          "Please remove one of the files and re-launch Phosphoros.");
      QMessageBox::critical(this, "Error while loading the Auxiliary Data...", text, QMessageBox::Abort);
    } else {
      QString text = QString::fromStdString(std::string("An error occured  while loading the Auxiliary Data.\n "
                                                        "Error : ") +
                                            msg);
      QMessageBox::critical(this, "Error while loading the Auxiliary Data...", text, QMessageBox::Abort);
    }

    exit(0);
  }

  ui->Lb_warning_time->setText("Loading the Pages...");
  qApp->processEvents();
  resetRepo();

  m_option_model_ptr->loadOption(m_filter_repository, m_seds_repository, m_redenig_curves_repository,
                                 m_luminosity_repository);
  ui->widget_configuration->loadOptionPage(m_option_model_ptr);

  ui->btn_HomeToAnalysis->setEnabled(true);
  ui->btn_HomeToCatalog->setEnabled(true);
  ui->btn_HomeToModel->setEnabled(true);
  ui->btn_HomeToOption->setEnabled(true);
  ui->btn_HomeToPP->setEnabled(true);

  ui->Lb_warning_time->setStyleSheet("font-weight: normal; color: #888888");
  ui->Lb_warning_time->setText("TIP: To save disk space, purge grid data you do not use anymore (see 'Catalog Setup')");


}

void MainWindow::resetRepo() {
  auto start = std::chrono::high_resolution_clock::now();
  main_logger.info()<<"Start reset repo";
  m_survey_model_ptr->loadSurvey();

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
  main_logger.info()<<"Survey loaded "<< duration << "[ms]";
  start = stop;

  ui->widget_Catalog->loadMappingPage(m_survey_model_ptr, m_filter_repository, "");
  stop = std::chrono::high_resolution_clock::now();
  duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
  main_logger.info()<<"Catalog Page loaded "<< duration << "[ms]";
  start = stop;

  m_model_set_model_ptr->loadSets();
  ui->widget_ModelSet->loadSetPage(m_model_set_model_ptr, m_seds_repository, m_redenig_curves_repository);
  stop = std::chrono::high_resolution_clock::now();
  duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
  main_logger.info()<<"Model Page loaded "<< duration << "[ms]";
  start = stop;

  ui->widget_Analysis->loadAnalysisPage(m_survey_model_ptr, m_model_set_model_ptr, m_seds_repository, m_redenig_curves_repository,
		                                m_filter_repository,  m_luminosity_repository);
  stop = std::chrono::high_resolution_clock::now();
  duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
  main_logger.info()<<"Analyse Page loaded "<< duration << "[ms]";
  start = stop;

  ui->widget_postprocessing->loadPostProcessingPage(m_survey_model_ptr);
  stop = std::chrono::high_resolution_clock::now();
  duration=(std::chrono::duration_cast<std::chrono::microseconds>(stop - start)).count()/1000;
  main_logger.debug()<<"Post Processing Page loaded "<< duration << "[ms]";
  start = stop;
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

// Todo (?) confirmation on quit
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

void MainWindow::on_btn_HomeToPP_clicked() {
  changeMainStackedWidgetIndex(5);
  ui->widget_postprocessing->updateSelection(true);
}

}  // namespace PhzQtUI
}  // namespace Euclid
