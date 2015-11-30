
#include <QDir>
#include <QMessageBox>
#include <QFileInfo>
#include "FileUtils.h"
#include "PhzQtUI/MainWindow.h"
#include "ui_MainWindow.h"
#include "PhzQtUI/DialogOptions.h"

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

  connect(ui->widget_Analysis,SIGNAL(navigateToHome()),SLOT(navigateToHome()));

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
}

MainWindow::~MainWindow()
{
}

//- Home Page
//  - Slots landing on this page
 void MainWindow::navigateToHome(){
     changeMainStackedWidgetIndex(0);
 }

 //--------------------------------------------------
 // Option Popup
 //  - Slots opening the popup
 void MainWindow::on_btn_HomeToOption_clicked()
 {
   std::unique_ptr<DialogOptions> popUp(new DialogOptions());

     popUp->exec();
 }

//------------------------------------------------
// Model Set Page
//  - Slots landing on this page
void MainWindow::on_btn_HomeToModel_clicked(){
    changeMainStackedWidgetIndex(1);
    ui->widget_ModelSet->loadSetPage();
}

//------------------------------------------------
// Analysis page  Page
//  - Slots landing on this page
void MainWindow::on_btn_HomeToAnalysis_clicked()
{
  changeMainStackedWidgetIndex(2);
  ui->widget_Analysis->loadAnalysisPage();
}

}
}
