#include <QDir>
#include "PhzQtUI/DialogPhotCorrectionEdition.h"
#include "ui_DialogPhotCorrectionEdition.h"
#include "PhzQtUI/FileUtils.h"

DialogPhotCorrectionEdition::DialogPhotCorrectionEdition(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPhotCorrectionEdition)
{
    ui->setupUi(this);
}

DialogPhotCorrectionEdition::~DialogPhotCorrectionEdition()
{
    delete ui;
}


void DialogPhotCorrectionEdition::setCorrectionsFile(std::string filePath){
    m_file_path=Euclid::PhosphorosUiDm::FileUtils::getPhotCorrectionsRootPath(false) + QString(QDir::separator()).toStdString() + filePath;
    ui->txt_FileName->setText(QString::fromStdString(filePath));

    //TODO Open file and load grid
}
