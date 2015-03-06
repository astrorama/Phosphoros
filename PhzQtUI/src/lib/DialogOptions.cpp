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
    ui->txt_rootDir->setText(QString::fromStdString(FileUtils::getRootPath()));

    ui->tabWidget->setTabEnabled(0,true);
    ui->tabWidget->setTabEnabled(1,true);
    ui->buttonBox->setEnabled(true);
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


}
}
