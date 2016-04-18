
#include <QDir>
#include <QMessageBox>
#include <QFileInfo>
#include "FileUtils.h"
#include "PhzQtUI/MainWindow.h"
#include "ui_MainWindow.h"
#include "PhzQtUI/DialogOptions.h"
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

  QPixmap pixmap( ":/logoPhUI.png" );
  ui->image_label->setTopMargin(20);
  ui->image_label->setPixmap(pixmap);

  connect( this, SIGNAL(changeMainStackedWidgetIndex(int)), ui->mainStackedWidget, SLOT(setCurrentIndex(int)) );

  connect(ui->widget_ModelSet,SIGNAL(navigateToHome()),SLOT(navigateToHome()));

  connect(ui->widget_Catalog,SIGNAL(navigateToHome()),SLOT(navigateToHome()));

  connect(ui->widget_Analysis,SIGNAL(navigateToHome()),SLOT(navigateToHome()));
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



  std::unique_ptr <XYDataset::FileParser > sed_file_parser {new XYDataset::AsciiParser { } };
  std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(false), std::move(sed_file_parser) });
  m_seds_repository.reset(new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>(std::move(sed_provider)));
  m_seds_repository->reload();

  std::unique_ptr <XYDataset::FileParser > reddening_file_parser {new XYDataset::AsciiParser { } };
  std::unique_ptr<XYDataset::FileSystemProvider> red_curve_provider(new XYDataset::FileSystemProvider{  FileUtils::getRedCurveRootPath(false), std::move(reddening_file_parser) });
  m_redenig_curves_repository.reset(new DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>{std::move(red_curve_provider)});
  m_redenig_curves_repository->reload();
}

MainWindow::~MainWindow()
{
}

//- Home Page
//  - Slots landing on this page
 void MainWindow::navigateToHome(){
     changeMainStackedWidgetIndex(0);
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
   std::unique_ptr<DialogOptions> popUp(new DialogOptions(m_seds_repository, m_redenig_curves_repository));

     popUp->exec();
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
}


void MainWindow::on_btn_HomeToCatalog_clicked(){
  changeMainStackedWidgetIndex(2);

  if (!m_mapping_loaded){
    ui->widget_Catalog->loadMappingPage("");
    m_mapping_loaded=true;
  }
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
