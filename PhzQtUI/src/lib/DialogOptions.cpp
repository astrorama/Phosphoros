#include <qfiledialog.h>

#include "PhzQtUI/DialogOptions.h"
#include "ui_DialogOptions.h"
#include "FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI{

DialogOptions::DialogOptions(QWidget *parent) :DialogOptions("",parent){
}



DialogOptions::DialogOptions(std::string new_catalog_file_path, QWidget *parent):
  QDialog(parent),
  ui(new Ui::DialogOptions)
{
     ui->setupUi(this);

     connect(ui->widget_survey_mapping, SIGNAL(startEdition(int)),SLOT(startEdition(int)));
     connect(ui->widget_survey_mapping, SIGNAL(endEdition()),SLOT(endEdition()));

     ui->widget_aux_Data->loadManagementPage(0);

     auto path_map = FileUtils::readPath();
     ui->txt_rootDir->setText(QString::fromStdString(FileUtils::getRootPath()));
     ui->txt_catDir->setText(QString::fromStdString(path_map["Catalogs"]));
     ui->txt_auxDir->setText(QString::fromStdString(path_map["AuxiliaryData"]));
     ui->txt_interDir->setText(QString::fromStdString(path_map["IntermediateProducts"]));
     ui->txt_resDir->setText(QString::fromStdString(path_map["Results"]));
     checkDirectories();

     ui->widget_survey_mapping->loadMappingPage(new_catalog_file_path);
}

DialogOptions::~DialogOptions()
{
}

void DialogOptions::on_btn_editGeneral_clicked()
{
    ui->tabWidget->setTabEnabled(1,false);
    ui->tabWidget->setTabEnabled(2,false);
    ui->buttonBox->setEnabled(false);
    ui->btn_editGeneral->setEnabled(false);
    ui->btn_cancelGeneral->setEnabled(true);
    ui->btn_saveGeneral->setEnabled(true);




    ui->btn_browseCat->setEnabled(true);
    ui->btn_browseAux->setEnabled(true);
    ui->btn_browseInter->setEnabled(true);
    ui->btn_browseRes->setEnabled(true);

    ui->btn_defCat->setEnabled(true);
    ui->btn_defAux->setEnabled(true);
    ui->btn_defInter->setEnabled(true);
    ui->btn_defRes->setEnabled(true);

}

void DialogOptions::on_btn_cancelGeneral_clicked()
{
    auto path_map = FileUtils::readPath();
    ui->txt_rootDir->setText(QString::fromStdString(FileUtils::getRootPath()));
    ui->txt_catDir->setText(QString::fromStdString(path_map["Catalogs"]));
    ui->txt_auxDir->setText(QString::fromStdString(path_map["AuxiliaryData"]));
    ui->txt_interDir->setText(QString::fromStdString(path_map["IntermediateProducts"]));
    ui->txt_resDir->setText(QString::fromStdString(path_map["Results"]));
    checkDirectories();

    ui->tabWidget->setTabEnabled(1,true);
    ui->tabWidget->setTabEnabled(2,true);
    ui->buttonBox->setEnabled(true);
    ui->btn_editGeneral->setEnabled(true);
    ui->btn_cancelGeneral->setEnabled(false);
    ui->btn_saveGeneral->setEnabled(false);


    ui->btn_browseCat->setEnabled(false);
    ui->btn_browseAux->setEnabled(false);
    ui->btn_browseInter->setEnabled(false);
    ui->btn_browseRes->setEnabled(false);

    ui->btn_defCat->setEnabled(false);
    ui->btn_defAux->setEnabled(false);
    ui->btn_defInter->setEnabled(false);
    ui->btn_defRes->setEnabled(false);

}

void DialogOptions::on_btn_saveGeneral_clicked()
{
    std::map<std::string,std::string> map{};

    map.insert(std::make_pair("LastUsed",FileUtils::getLastUsedPath()));

    map.insert(std::make_pair("Catalogs",ui->txt_catDir->text().toStdString()));

    map.insert(std::make_pair("AuxiliaryData",ui->txt_auxDir->text().toStdString()));

    map.insert(std::make_pair("IntermediateProducts",ui->txt_interDir->text().toStdString()));

    map.insert(std::make_pair("Results",ui->txt_resDir->text().toStdString()));


    FileUtils::savePath(map);

    ui->tabWidget->setTabEnabled(1,true);
    ui->tabWidget->setTabEnabled(2,true);
    ui->buttonBox->setEnabled(true);
    ui->btn_editGeneral->setEnabled(true);
    ui->btn_cancelGeneral->setEnabled(false);
    ui->btn_saveGeneral->setEnabled(false);


    ui->btn_browseCat->setEnabled(false);
    ui->btn_browseAux->setEnabled(false);
    ui->btn_browseInter->setEnabled(false);
    ui->btn_browseRes->setEnabled(false);

    ui->btn_defCat->setEnabled(false);
    ui->btn_defAux->setEnabled(false);
    ui->btn_defInter->setEnabled(false);
    ui->btn_defRes->setEnabled(false);

}

void DialogOptions::on_btn_browseCat_clicked(){
  auto root_path= QString::fromStdString(FileUtils::getRootPath());
  QFileDialog dialog(this);
  dialog.selectFile(root_path);
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  if (dialog.exec()){
      QStringList fileNames=dialog.selectedFiles();



      ui->txt_catDir->setText(fileNames[0]);
  }
  checkDirectories();
}

void DialogOptions::on_btn_browseAux_clicked(){
  auto root_path= QString::fromStdString(FileUtils::getRootPath());
   QFileDialog dialog(this);
   dialog.selectFile(root_path);
   dialog.setFileMode(QFileDialog::DirectoryOnly);
   if (dialog.exec()){
       QStringList fileNames=dialog.selectedFiles();


      ui->txt_auxDir->setText(fileNames[0]);
  }
  checkDirectories();
}

void DialogOptions::on_btn_browseInter_clicked(){
  auto root_path= QString::fromStdString(FileUtils::getRootPath());
   QFileDialog dialog(this);
   dialog.selectFile(root_path);
   dialog.setFileMode(QFileDialog::DirectoryOnly);
   if (dialog.exec()){
       QStringList fileNames=dialog.selectedFiles();


      ui->txt_interDir->setText(fileNames[0]);
  }
  checkDirectories();
}

void DialogOptions::on_btn_browseRes_clicked(){
  auto root_path= QString::fromStdString(FileUtils::getRootPath());
   QFileDialog dialog(this);
   dialog.selectFile(root_path);
   dialog.setFileMode(QFileDialog::DirectoryOnly);
   if (dialog.exec()){
       QStringList fileNames=dialog.selectedFiles();


      ui->txt_resDir->setText(fileNames[0]);
  }
  checkDirectories();
}


void DialogOptions::on_btn_defCat_clicked(){
  auto def = QString::fromStdString(FileUtils::getRootPaths().getCatalogsDir().generic_string());
  ui->txt_catDir->setText(def);
  checkDirectories();
}
void DialogOptions::on_btn_defAux_clicked(){
  auto def = QString::fromStdString(FileUtils::getRootPaths().getAuxDataDir().generic_string());
  ui->txt_auxDir->setText(def);
  checkDirectories();
}

void DialogOptions::on_btn_defInter_clicked(){
  auto def = QString::fromStdString(FileUtils::getRootPaths().getIntermediateDir().generic_string());
  ui->txt_interDir->setText(def);
  checkDirectories();
}

void DialogOptions::on_btn_defRes_clicked(){
  auto def = QString::fromStdString(FileUtils::getRootPaths().getResultsDir().generic_string());
  ui->txt_resDir->setText(def);
  checkDirectories();
}


void DialogOptions::checkDirectories(){
  auto root_paths= FileUtils::getRootPaths();

  if (ui->txt_catDir->text().toStdString()==root_paths.getCatalogsDir().generic_string()){
    ui->txt_catDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
    ui->txt_catDir->setStyleSheet("QLineEdit { color: black }");
  }

  if (ui->txt_auxDir->text().toStdString()==root_paths.getAuxDataDir().generic_string()){
      ui->txt_auxDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
      ui->txt_auxDir->setStyleSheet("QLineEdit { color: black }");
  }

  if (ui->txt_interDir->text().toStdString()==root_paths.getIntermediateDir().generic_string()){
      ui->txt_interDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
      ui->txt_interDir->setStyleSheet("QLineEdit { color: black }");
  }

  if (ui->txt_resDir->text().toStdString()==root_paths.getResultsDir().generic_string()){
      ui->txt_resDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
      ui->txt_resDir->setStyleSheet("QLineEdit { color: black }");
  }
}


void DialogOptions::startEdition(int i){
  for (int j=0;j<3;++j){
      if (i!=j){
        ui->tabWidget->setTabEnabled(j,false);
      }
  }

  ui->buttonBox->setEnabled(false);
}

void DialogOptions::endEdition(){

  ui->tabWidget->setTabEnabled(0,true);
  ui->tabWidget->setTabEnabled(1,true);
  ui->tabWidget->setTabEnabled(2,true);
  ui->tabWidget->setEnabled(true);
  ui->buttonBox->setEnabled(true);
}


}
}
