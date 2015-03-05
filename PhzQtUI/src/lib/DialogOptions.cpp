#include <qfiledialog.h>

#include "PhzQtUI/DialogOptions.h"
#include "ui_DialogOptions.h"
#include <QSettings>

using namespace std;

namespace Euclid {
namespace PhzQtUI{

DialogOptions::DialogOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOptions)
{
    ui->setupUi(this);

    QSettings settings("SDC-CH", "PhosphorosUI");
    ui->txt_rootDir->setText(settings.value(QString::fromStdString("Gerneral/root-path")).toString());
}

DialogOptions::~DialogOptions()
{
}

void DialogOptions::on_btn_ManageFilter_clicked()
{
    goToFilterManagement();
    this->close();
}

void DialogOptions::on_btn_ManageAuxData_clicked()
{
    goToAuxDataManagement();
    this->close();
}


void DialogOptions::on_btn_editGeneral_clicked()
{
    ui->tabWidget->setTabEnabled(0,false);
    ui->tabWidget->setTabEnabled(1,false);
    ui->buttonBox->setEnabled(false);
    ui->btn_cancelGeneral->setEnabled(true);
    ui->btn_saveGeneral->setEnabled(true);
    ui->btn_browseRoot->setEnabled(true);
}

void DialogOptions::on_btn_cancelGeneral_clicked()
{
    QSettings settings("SDC-CH", "PhosphorosUI");
    ui->txt_rootDir->setText(settings.value(QString::fromStdString("Gerneral/root-path")).toString());

    ui->tabWidget->setTabEnabled(0,true);
    ui->tabWidget->setTabEnabled(1,true);
    ui->buttonBox->setEnabled(true);
    ui->btn_cancelGeneral->setEnabled(false);
    ui->btn_saveGeneral->setEnabled(false);
    ui->btn_browseRoot->setEnabled(false);
}

void DialogOptions::on_btn_saveGeneral_clicked()
{

     QSettings settings("SDC-CH", "PhosphorosUI");
     settings.setValue(QString::fromStdString("Gerneral/root-path"),ui->txt_rootDir->text());

    ui->tabWidget->setTabEnabled(0,true);
    ui->tabWidget->setTabEnabled(1,true);
    ui->buttonBox->setEnabled(true);
    ui->btn_cancelGeneral->setEnabled(false);
    ui->btn_saveGeneral->setEnabled(false);
    ui->btn_browseRoot->setEnabled(false);
}

void DialogOptions::on_btn_browseRoot_clicked()
{
    QFileDialog dialog(this);


    QSettings settings("SDC-CH", "PhosphorosUI");
    dialog.selectFile(settings.value(QString::fromStdString("Gerneral/root-path")).toString());
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    if (dialog.exec()){

        QStringList fileNames=dialog.selectedFiles();
        ui->txt_rootDir->setText(fileNames[0]);

    }
}


}
}
