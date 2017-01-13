#include <qfiledialog.h>

#include "PhzQtUI/DialogOptions.h"
#include "ui_DialogOptions.h"
#include "PhzQtUI/FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI{

DialogOptions::DialogOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);

    ui->txt_rootDir->setText(QString::fromStdString(FileUtils::getRootPath()));

    ui->widget_survey_mapping->loadMappingPage();

    connect(ui->widget_survey_mapping, SIGNAL(startEdition(int)),SLOT(startEdition(int)));
    connect(ui->widget_survey_mapping, SIGNAL(endEdition()),SLOT(endEdition()));

    ui->widget_aux_Data->loadManagementPage(0);
}

DialogOptions::~DialogOptions()
{
}

void DialogOptions::EditRootPath(){
  m_is_root_edition=true;
  ui->label_2->setText("Welcome to Phosphoros!");
  ui->label->setText("Please select the Root Path into which application data will be stored:" );
  ui->tabWidget->setCurrentIndex(2);
  on_btn_editGeneral_clicked();
}


void DialogOptions::on_btn_editGeneral_clicked()
{
    ui->tabWidget->setTabEnabled(0,false);
    ui->tabWidget->setTabEnabled(1,false);
    ui->buttonBox->setEnabled(false);
    ui->btn_editGeneral->setEnabled(false);
    ui->btn_cancelGeneral->setEnabled(!m_is_root_edition);
    ui->btn_saveGeneral->setEnabled(true);
    ui->btn_browseRoot->setEnabled(true);
}

void DialogOptions::on_btn_cancelGeneral_clicked()
{
    ui->txt_rootDir->setText(QString::fromStdString(FileUtils::getRootPath()));

    ui->tabWidget->setTabEnabled(0,true);
    ui->tabWidget->setTabEnabled(1,true);
    ui->buttonBox->setEnabled(true);
    ui->btn_editGeneral->setEnabled(true);
    ui->btn_cancelGeneral->setEnabled(false);
    ui->btn_saveGeneral->setEnabled(false);
    ui->btn_browseRoot->setEnabled(false);
}

void DialogOptions::on_btn_saveGeneral_clicked()
{
    FileUtils::setRootPath(ui->txt_rootDir->text().toStdString());

    ui->tabWidget->setTabEnabled(0,true);
    ui->tabWidget->setTabEnabled(1,true);
    ui->buttonBox->setEnabled(true);
    ui->btn_cancelGeneral->setEnabled(false);
    ui->btn_saveGeneral->setEnabled(false);
    ui->btn_browseRoot->setEnabled(false);

    if (m_is_root_edition){
      this->accept();
    }
}

void DialogOptions::on_btn_browseRoot_clicked()
{
    QFileDialog dialog(this);

    dialog.selectFile(QString::fromStdString(FileUtils::getRootPath()));
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    if (dialog.exec()){

        QStringList fileNames=dialog.selectedFiles();
        ui->txt_rootDir->setText(fileNames[0]);
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
