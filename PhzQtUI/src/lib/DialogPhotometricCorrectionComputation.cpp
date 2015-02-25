#include <QStandardItem>
#include "PhzQtUI/DialogPhotometricCorrectionComputation.h"
#include "ui_DialogPhotometricCorrectionComputation.h"

DialogPhotometricCorrectionComputation::DialogPhotometricCorrectionComputation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPhotometricCorrectionComputation)
{
    ui->setupUi(this);
}

DialogPhotometricCorrectionComputation::~DialogPhotometricCorrectionComputation()
{
    delete ui;
}


void DialogPhotometricCorrectionComputation::setData(std::string survey, std::string model, std::string grid, std::list<std::string> selectedFilters ){
    ui->txt_survey->setText(QString::fromStdString(survey));
    ui->txt_Model->setText(QString::fromStdString(model));
    ui->txt_grid->setText(QString::fromStdString(grid));

    QStandardItemModel* grid_model = new QStandardItemModel();
    grid_model->setColumnCount(1);
    for (auto filter : selectedFilters){
        QList<QStandardItem*> items{{new QStandardItem(QString::fromStdString(filter))}};
        grid_model->appendRow(items);
    }

    ui->listView_Filter->setModel(grid_model);
}

void DialogPhotometricCorrectionComputation::on_btn_TrainingCatalogFolder_clicked()
{

}
