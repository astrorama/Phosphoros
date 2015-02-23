

#include "PhzQtUI/MainWindow.h"
#include "ui_MainWindow.h"
#include "PhzQtUI/DialogOptions.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( this, SIGNAL(changeMainStackedWidgetIndex(int)), ui->mainStackedWidget, SLOT(setCurrentIndex(int)) );

    connect(ui->widget_ModelSet,SIGNAL(navigateToHome()),SLOT(navigateToHome()));

    connect(ui->widget_SurveyMapping,SIGNAL(navigateToHome()),SLOT(navigateToHome()));
    connect(ui->widget_SurveyMapping,SIGNAL(navigateToFilterManagement()),SLOT(navigateToAuxDataManagement()));

    connect(ui->widget_AuxDataManagement,SIGNAL(navigateToHome()),SLOT(navigateToHome()));


    connect(ui->widget_Analysis,SIGNAL(navigateToHome()),SLOT(navigateToHome()));
}

MainWindow::~MainWindow()
{
    delete ui;
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
     DialogOptions* popUp= new DialogOptions();

     connect( popUp, SIGNAL(goToFilterManagement()),
       SLOT(navigateToFilterManagement())
      );

     connect( popUp, SIGNAL(goToAuxDataManagement()),
               SLOT(navigateToAuxDataManagement())
              );

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
// Filter Mapping  Page
//  - Slots landing on this page
void MainWindow::navigateToFilterManagement(){
     changeMainStackedWidgetIndex(2);
     ui->widget_SurveyMapping->loadMappingPage();
}

//------------------------------------------------
// Aux Data Management  Page
//  - Slots landing on this page
void MainWindow::navigateToAuxDataManagement(){

    changeMainStackedWidgetIndex(3);
    ui->widget_AuxDataManagement->loadManagementPage();
}

//------------------------------------------------
// Analysis page  Page
//  - Slots landing on this page
void MainWindow::on_btn_HomeToAnalysis_clicked()
{
  changeMainStackedWidgetIndex(4);
  ui->widget_Analysis->loadAnalysisPage();
}


