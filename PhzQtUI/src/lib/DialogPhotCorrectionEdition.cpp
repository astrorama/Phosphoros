#include <QDir>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QAbstractItemView>

#include "PhzQtUI/DialogPhotCorrectionEdition.h"
#include "ui_DialogPhotCorrectionEdition.h"
#include "PhzQtUI/FileUtils.h"
#include "PhzQtUI/PhotometricCorrectionHandler.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {

DialogPhotCorrectionEdition::DialogPhotCorrectionEdition(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPhotCorrectionEdition)
{
    ui->setupUi(this);
}

DialogPhotCorrectionEdition::~DialogPhotCorrectionEdition()
{
}


void DialogPhotCorrectionEdition::setCorrectionsFile(string filePath, std::list<FilterMapping> filters){
    m_filters=std::move(filters);
    m_file_path=filePath;
    ui->txt_FileName->setText(QString::fromStdString(filePath));
    m_map = PhotometricCorrectionHandler::getCorrections(filePath);

    QStandardItemModel* model = new QStandardItemModel();
    model->setColumnCount(3);

    QStringList  setHeaders;
    setHeaders<<"Filter"<<"Transmission"<<"Correction";
    model->setHorizontalHeaderLabels(setHeaders);

    for (auto& correction_pair : m_map){
      QList<QStandardItem*> items;
      std::string alias ="";
      for (auto filter:m_filters){
        if (filter.getFilterFile().compare(correction_pair.first.qualifiedName())==0){
          alias=filter.getName();
          break;
        }
      }
      items.push_back(new QStandardItem(QString::fromStdString( alias)));
      items[0]->setEditable(false);
      items.push_back(new QStandardItem(QString::fromStdString( correction_pair.first.datasetName())));
      items[1]->setEditable(false);
      items.push_back(new QStandardItem(QString::number( correction_pair.second)));
      model->appendRow(items);
    }




    ui->tableView->setModel(model);

    ui->tableView->resizeColumnsToContents();

    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);

}

void DialogPhotCorrectionEdition::on_btn_Edit_clicked()
{
    ui->btn_Edit->setEnabled(false);
    ui->btn_Cancel->setEnabled(true);
    ui->btn_Save->setEnabled(true);
    ui->buttonBox->setEnabled(false);
    ui->tableView->setEnabled(true);
    ui->tableView->setEnabled(true);

}


void DialogPhotCorrectionEdition::on_tableView_doubleClicked ( const QModelIndex & ){
     ui->btn_Edit->setEnabled(false);
     ui->btn_Cancel->setEnabled(true);
     ui->btn_Save->setEnabled(true);
     ui->buttonBox->setEnabled(false);
     ui->tableView->setEnabled(true);
     ui->tableView->setEnabled(true);
}

void DialogPhotCorrectionEdition::on_btn_Cancel_clicked()
{
    ui->btn_Edit->setEnabled(true);
    ui->btn_Cancel->setEnabled(false);
    ui->btn_Save->setEnabled(false);
    ui->buttonBox->setEnabled(true);

    setCorrectionsFile(m_file_path,std::move(m_filters));
}


void DialogPhotCorrectionEdition::on_btn_Save_clicked()
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->tableView->model());
    bool valid=true;
    for (int i = 0; i< model->columnCount();++i){
         bool ok;
         model->item(0,i)->text().toDouble(&ok);
        valid= valid && ok;
    }
    if (!valid){
        QMessageBox::warning( this, "Incorrect Data...",
                                          "The Photometric corrections that you have entered cannot be converted in number, please correct them.",
                                          QMessageBox::Ok );
    }
    else{

      int i=0;
      for (auto& correction_pair : m_map){
        correction_pair.second = model->item(0,i)->text().toDouble();
        ++i;
      }

      PhotometricCorrectionHandler::writeCorrections(m_map,m_file_path);
        ui->btn_Edit->setEnabled(true);
        ui->btn_Cancel->setEnabled(false);
        ui->btn_Save->setEnabled(false);
        ui->tableView->setEnabled(false);
    }
}

}
}


