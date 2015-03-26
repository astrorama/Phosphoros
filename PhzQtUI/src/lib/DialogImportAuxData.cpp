
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "PhzQtUI/DialogImportAuxData.h"
#include "ui_DialogImportAuxData.h"
#include "PhzQtUI/FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogImportAuxData::DialogImportAuxData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImportAuxData)
{
    ui->setupUi(this);
}

DialogImportAuxData::~DialogImportAuxData()
{
}

void DialogImportAuxData::setData(string title,string parentFolderFull,string parentFolderDisplay){
    ui->lbl_title->setText(QString::fromStdString(title));
    ui->txt_targetGroup->setText(QString::fromStdString(parentFolderDisplay));
    m_parent_folder=parentFolderFull;
}

void DialogImportAuxData::on_btn_browseFile_clicked()
{
    QFileDialog dialog(this);
    dialog.selectFile(QString::fromStdString(FileUtils::getLastUsedPath()));
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()){
        QStringList file_name=dialog.selectedFiles();
        ui->txt_file->setText(file_name[0]);
        FileUtils::setLastUsedPath(file_name[0].toStdString());
    }
}

void DialogImportAuxData::on_btn_browseFolder_clicked()
{
    QFileDialog dialog(this);
    dialog.selectFile(QString::fromStdString(FileUtils::getLastUsedPath()));
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    if (dialog.exec()){
        QStringList folder_names=dialog.selectedFiles();
        ui->txt_folder->setText(folder_names[0]);
        FileUtils::setLastUsedPath(folder_names[0].toStdString());
    }
}

void DialogImportAuxData::on_rb_file_clicked()
{
    ui->rb_folder->setChecked(false);
    ui->frame_Folder->setEnabled(false);
    ui->frame_File->setEnabled(true);
    ui->txt_file->setEnabled(false);
}

void DialogImportAuxData::on_rb_folder_clicked()
{
    ui->rb_file->setChecked(false);
    ui->frame_Folder->setEnabled(true);
    ui->frame_File->setEnabled(false);
    ui->txt_folder->setEnabled(false);
}

void DialogImportAuxData::on_btn_import_clicked()
{
    if (ui->rb_file->isChecked()){
        // File case
        QFileInfo info(ui->txt_file->text());
        if (!info.exists() || !QFile::copy(info.absoluteFilePath(), QString::fromStdString(m_parent_folder)+QDir::separator() + info.fileName())){
            QMessageBox::warning( this, "Missing File...",
                                              "It is not possible to find the file you want to import. Please enter the file name again.",
                                              QMessageBox::Ok );
            return;
        }
    }
    else{
        //Folder case

        QFileInfo info(ui->txt_folder->text());
        if (!info.exists() ){
          QMessageBox::warning( this, "Missing Folder...",
                                                              "It is not possible to find the folder you want to import. Please enter the folder name again.",
                                                              QMessageBox::Ok );
           return;
        }

        QString dest_folder=QString::fromStdString(m_parent_folder);
        if (ui->cb_createSubGroup->isChecked()){
           dest_folder=dest_folder+QDir::separator()+info.fileName();
           if (! QDir().mkdir(dest_folder)){
               QMessageBox::warning( this, "Folder Not Created...",
                                                 "It is not possible to create a group with the selected folder name.",
                                                 QMessageBox::Ok );
               return;
           }
        }

        // fill the dest folder
        QFileInfo dest_info(dest_folder);
        FileUtils::copyRecursively(info.absoluteFilePath(),dest_info.absoluteFilePath());
    }
    accept();
}

}
}
