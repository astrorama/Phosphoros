
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include "PhzQtUI/DialogOutputColumnSelection.h"
#include "ui_DialogOutputColumnSelection.h"
#include "FileUtils.h"
#include <QStandardItemModel>
#include <QStandardItem>

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogOutputColumnSelection::DialogOutputColumnSelection(
      std::list<std::string> all_columns,
      std::string id_column,
      std::map<std::string,std::string> copied_columns,
      QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOutputColumnSelection),
    m_all_columns{std::move(all_columns)},
    m_id_column{std::move(id_column)},
    m_copied_columns{std::move(copied_columns)}{
  ui->setupUi(this);

  //todo add aliases


  QStandardItemModel *model = new QStandardItemModel();
  model->setColumnCount(2);
  QStringList  setHeaders;
     setHeaders<<"Input Catalog Column Name"<<"renaming";
  model->setHorizontalHeaderLabels (setHeaders );
  for(std::string column :m_all_columns){
    QList<QStandardItem*> items;
    items.push_back(new QStandardItem(QString::fromStdString(column)));
    items[0]->setEditable(false);
    items[0]->setCheckable(true);
    items.push_back(new QStandardItem(""));

    if (column==m_id_column){
      items[0]->setCheckState(Qt::CheckState::Checked);
      items[0]->setEnabled(false);
      items[1]->setEnabled(false);
    }else if  (m_copied_columns.find(column) != m_copied_columns.end()){
      items[0]->setCheckState(Qt::CheckState::Checked);
      std::string alias = m_copied_columns[column];
      if (alias != column && alias!=""){
        items[1]->setText(QString::fromStdString(alias));
      }
    }

    model->appendRow(items);
  }

  ui->treeView->setModel(model);
  ui->treeView->expandAll();
  ui->treeView->header()->setResizeMode(0, QHeaderView::Stretch);

  ui->treeView->header()->setResizeMode(1, QHeaderView::Stretch);
 }

DialogOutputColumnSelection::~DialogOutputColumnSelection(){}


void DialogOutputColumnSelection::on_btn_cancel_clicked(){
  reject();
}

void DialogOutputColumnSelection::on_btn_save_clicked(){
  std::map<std::string,std::string> result_list{};

  QStandardItemModel *model=dynamic_cast<QStandardItemModel*>(ui->treeView->model());
  for(int row=0; row < model->rowCount(); ++row){
    QStandardItem* child = model->item(row,0);
    if (child->checkState()==Qt::CheckState::Checked){
      std::string name =  child->text().toStdString();
      std::string alias = model->item(row,1)->text().toStdString();
      if (name!=m_id_column){
        result_list[name]="";
        if (alias!="" && alias!=name){
          result_list[name]=alias;
        }
      }
    }
  }

  selectedColumns(result_list);
  accept();
}



}
}
