
#include <QFileDialog>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include "PhzQtUI/DialogImportAuxData.h"
#include "ui_DialogImportAuxData.h"
#include "PhzQtUI/FileUtils.h"

DialogImportAuxData::DialogImportAuxData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImportAuxData)
{
    ui->setupUi(this);
}

DialogImportAuxData::~DialogImportAuxData()
{
    delete ui;
}


void DialogImportAuxData::setData(std::string title,std::string parentFolderFull,std::string parentFolderDisplay){
    ui->lbl_title->setText(QString::fromStdString(title));
    ui->txt_targetGroup->setText(QString::fromStdString(parentFolderDisplay));
    m_parent_folder=parentFolderFull;
}

void DialogImportAuxData::on_btn_browseFile_clicked()
{
    QFileDialog dialog(this);
    QSettings settings("SDC-CH", "PhosphorosUI");
    dialog.selectFile(settings.value(QString::fromStdString("Gerneral/root-path")).toString());
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()){
        QStringList file_name=dialog.selectedFiles();
        ui->txt_file->setText(file_name[0]);
    }
}

void DialogImportAuxData::on_btn_browseFolder_clicked()
{
    QFileDialog dialog(this);
    QSettings settings("SDC-CH", "PhosphorosUI");
    dialog.selectFile(settings.value(QString::fromStdString("Gerneral/root-path")).toString());
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    if (dialog.exec()){
        QStringList folder_names=dialog.selectedFiles();
        ui->txt_folder->setText(folder_names[0]);
    }
}

void DialogImportAuxData::on_rb_file_clicked()
{
    ui->rb_folder->setChecked(false);
    ui->frame_Folder->setEnabled(false);
    ui->frame_File->setEnabled(true);
}

void DialogImportAuxData::on_rb_folder_clicked()
{
    ui->rb_file->setChecked(false);
    ui->frame_Folder->setEnabled(true);
    ui->frame_File->setEnabled(false);
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
        Euclid::PhosphorosUiDm::FileUtils::copyRecursively(info.absoluteFilePath(),dest_info.absoluteFilePath());
    }
    accept();
}
