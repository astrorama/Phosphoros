

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

    connect( this, SIGNAL(changeMainStackedWidgetIndex(int)), ui->mainStackedWidget, SLOT(setCurrentIndex(int)) );

    connect(ui->widget_ModelSet,SIGNAL(navigateToHome()),SLOT(navigateToHome()));


    connect(ui->widget_Analysis,SIGNAL(navigateToHome()),SLOT(navigateToHome()));
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
