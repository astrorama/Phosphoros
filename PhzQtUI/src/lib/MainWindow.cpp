

#include <QSettings>
#include <QDir>


#include "PhzQtUI/MainWindow.h"
#include "ui_MainWindow.h"
#include "PhzQtUI/DialogOptions.h"

namespace Euclid {
namespace PhzQtUI {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pixmap( ":/logoPhUI.png" );
    ui->image_label->setTopMargin(20);
    ui->image_label->setPixmap(pixmap);

    connect( this, SIGNAL(changeMainStackedWidgetIndex(int)), ui->mainStackedWidget, SLOT(setCurrentIndex(int)) );

    connect(ui->widget_ModelSet,SIGNAL(navigateToHome()),SLOT(navigateToHome()));


    connect(ui->widget_Analysis,SIGNAL(navigateToHome()),SLOT(navigateToHome()));

  // default value for the root-path
  QSettings settings("SDC-CH", "PhosphorosUI");

  std::string test_value = "default";
  if (test_value.compare(
      settings.value(QString::fromStdString("General/root-path"),
          QString::fromStdString(test_value)).toString().toStdString()) == 0) {

    settings.beginGroup("General");
    settings.setValue(QString::fromStdString("root-path"), QDir::currentPath());
    settings.endGroup();

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
