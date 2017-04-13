
#include <QDir>
#include <QMessageBox>
#include <QFileInfo>
#include "FileUtils.h"
#include "PhzQtUI/MainWindow.h"
#include "ui_MainWindow.h"
#include "XYDataset/AsciiParser.h"

#include "ThisProject.h"             // for the name and version of this very project

namespace Euclid {
namespace PhzQtUI {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  QString title = QString::fromStdString(THIS_PROJECT_NAME_STRING+ " " + THIS_PROJECT_VERSION_STRING);

  setWindowTitle(title);

  ui->setupUi(this);
  this->layout()->setSpacing(0);
  this->layout()->setContentsMargins(0,0,0,0);
  ui->mainStackedWidget->setContentsMargins(0,0,0,0);
//  ui->page_layout->setSpacing(0);
//  ui->page_layout->setContentsMargins(0,0,0,0);
//  this->setStyleSheet("background-color: green");

  QPixmap pixmap( ":/logoPhUI.png" );
  ui->image_label->setTopMargin(20);
  ui->image_label->setPixmap(pixmap);

  connect( this, SIGNAL(changeMainStackedWidgetIndex(int)), ui->mainStackedWidget, SLOT(setCurrentIndex(int)) );


  connect(ui->widget_ModelSet,SIGNAL(navigateToConfig()),SLOT(navigateToConfig()));
  connect(ui->widget_ModelSet,SIGNAL(navigateToCatalog(bool)),SLOT(navigateToCatalog(bool)));
  connect(ui->widget_ModelSet,SIGNAL(navigateToComputeRedshift(bool)),SLOT(navigateToComputeRedshift(bool)));
  connect(ui->widget_ModelSet,SIGNAL(quit(bool)),SLOT(quit(bool)));

  connect(ui->widget_Catalog,SIGNAL(navigateToConfig()),SLOT(navigateToConfig()));
  connect(ui->widget_Catalog,SIGNAL(navigateToParameter(bool)),SLOT(navigateToParameter(bool)));
  connect(ui->widget_Catalog,SIGNAL(navigateToComputeRedshift(bool)),SLOT(navigateToComputeRedshift(bool)));
  connect(ui->widget_Catalog,SIGNAL(quit(bool)),SLOT(quit(bool)));

  connect(ui->widget_configuration,SIGNAL(navigateToCatalog(bool)),SLOT(navigateToCatalog(bool)));
  connect(ui->widget_configuration,SIGNAL(navigateToParameter(bool)),SLOT(navigateToParameter(bool)));
  connect(ui->widget_configuration,SIGNAL(navigateToComputeRedshift(bool)),SLOT(navigateToComputeRedshift(bool)));
  connect(ui->widget_configuration,SIGNAL(quit(bool)),SLOT(quit(bool)));

  connect(ui->widget_Analysis,SIGNAL(navigateToCatalog(bool)),SLOT(navigateToCatalog(bool)));
  connect(ui->widget_Analysis,SIGNAL(navigateToParameter(bool)),SLOT(navigateToParameter(bool)));
  connect(ui->widget_Analysis,SIGNAL(navigateToConfig()),SLOT(navigateToConfig()));
  connect(ui->widget_Analysis,SIGNAL(quit(bool)),SLOT(quit(bool)));

  connect(ui->widget_Analysis,SIGNAL(navigateToNewCatalog(std::string)),SLOT(navigateToNewCatalog(std::string)));




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



  std::unique_ptr <XYDataset::FileParser > filter_file_parser {new XYDataset::AsciiParser { } };
  std::unique_ptr<XYDataset::FileSystemProvider> filter_provider(new XYDataset::FileSystemProvider{FileUtils::getFilterRootPath(true), std::move(filter_file_parser) });
  m_filter_repository.reset(new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>(std::move(filter_provider)));
  m_filter_repository->reload();

  std::unique_ptr <XYDataset::FileParser > sed_file_parser {new XYDataset::AsciiParser { } };
  std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(true), std::move(sed_file_parser) });
  m_seds_repository.reset(new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>(std::move(sed_provider)));
  m_seds_repository->reload();

  std::unique_ptr <XYDataset::FileParser > reddening_file_parser {new XYDataset::AsciiParser { } };
  std::unique_ptr<XYDataset::FileSystemProvider> red_curve_provider(new XYDataset::FileSystemProvider{  FileUtils::getRedCurveRootPath(true), std::move(reddening_file_parser) });
  m_redenig_curves_repository.reset(new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>{std::move(red_curve_provider)});
  m_redenig_curves_repository->reload();

  std::unique_ptr <XYDataset::FileParser > luminosity_file_parser {new XYDataset::AsciiParser { } };
  std::unique_ptr<XYDataset::FileSystemProvider> luminosity_curve_provider(new XYDataset::FileSystemProvider{  FileUtils::getLuminosityFunctionCurveRootPath(true), std::move(luminosity_file_parser) });
  m_luminosity_repository.reset(new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>{std::move(luminosity_curve_provider)});
  m_luminosity_repository->reload();
}

MainWindow::~MainWindow()
{
}

//- Home Page
//  - Slots landing on this page
 void MainWindow::navigateToHome(){
     changeMainStackedWidgetIndex(0);
 }

 void MainWindow::navigateToHomeWithReset(bool reset){
     changeMainStackedWidgetIndex(0);
     if (reset){
       m_model_loaded=false;
       m_mapping_loaded=false;
     }
 }

void MainWindow::navigateToParameter(bool reset){
  if (reset){
        m_model_loaded=false;
        m_mapping_loaded=false;
      }
  on_btn_HomeToModel_clicked();
}

void MainWindow::navigateToCatalog(bool reset){
  if (reset){
        m_model_loaded=false;
        m_mapping_loaded=false;
      }
  on_btn_HomeToCatalog_clicked();
}

void MainWindow::navigateToComputeRedshift(bool reset){
  if (reset){
        m_model_loaded=false;
        m_mapping_loaded=false;
      }
  on_btn_HomeToAnalysis_clicked();
}

//Todo confirmation

void MainWindow::quit(bool){
  this->close();
}

void MainWindow::navigateToConfig(){
  on_btn_HomeToOption_clicked();
}


 void MainWindow::navigateToNewCatalog(std::string new_name){
   changeMainStackedWidgetIndex(2);
   ui->widget_Catalog->loadMappingPage(new_name);
 }

 //--------------------------------------------------
 // Option Popup
 //  - Slots opening the popup
 void MainWindow::on_btn_HomeToOption_clicked()
 {
   changeMainStackedWidgetIndex(4);
   ui->widget_configuration->loadOptionPage(m_filter_repository, m_seds_repository, m_redenig_curves_repository, m_luminosity_repository);
 }

//------------------------------------------------
// Model Set Page
//  - Slots landing on this page
void MainWindow::on_btn_HomeToModel_clicked(){
    changeMainStackedWidgetIndex(1);

    if (!m_model_loaded){
      ui->widget_ModelSet->loadSetPage(m_seds_repository, m_redenig_curves_repository);
      m_model_loaded=true;
    }

    ui->widget_ModelSet->updateSelection();
}


void MainWindow::on_btn_HomeToCatalog_clicked(){
  changeMainStackedWidgetIndex(2);

  if (!m_mapping_loaded){
    ui->widget_Catalog->loadMappingPage("");
    m_mapping_loaded=true;
  }

  ui->widget_Catalog->updateSelection();
}


//------------------------------------------------
// Analysis page  Page
//  - Slots landing on this page
void MainWindow::on_btn_HomeToAnalysis_clicked()
{
  changeMainStackedWidgetIndex(3);

    ui->widget_Analysis->loadAnalysisPage();


}

}
}
