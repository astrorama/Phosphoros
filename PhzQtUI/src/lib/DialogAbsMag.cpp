/*
 * DialogAbsMag.cpp
 *
 *  Created on: 2026/08/07
 *      Author: fdubath
 */

#include <QStandardItemModel>
#include "PhzQtUI/DialogAbsMag.h"
#include "PhzQtUI/DialogFilterSelector.h"
#include "ui_DialogAbsMag.h"
#include "PhzQtUI/MessageButton.h"
#include "AlexandriaKernel/memory_tools.h"
#include <set>
#include <QMessageBox>

namespace Euclid {
namespace PhzQtUI {

class MyCustomModel : public QStandardItemModel {
public:
    Qt::ItemFlags flags(const QModelIndex &index) const override {
        Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);
        if (index.column() == 0) {
            // Bitwise clear the ItemIsEditable flag
            defaultFlags &= ~Qt::ItemIsEditable; 
        }
        return defaultFlags;
    }
};

DialogAbsMag::DialogAbsMag(QWidget* parent) : QDialog(parent), ui(new Ui::DialogAbsMag) {
  ui->setupUi(this);
}

  DialogAbsMag::~DialogAbsMag() {}  
  
  void DialogAbsMag::setData(DatasetRepo filter_repository, std::list<std::pair<QString, QString>> abs_mag_config) {
      m_filter_repository = filter_repository;
      m_abs_mag_config = abs_mag_config;
      
      auto* grid_model = new MyCustomModel();
      grid_model->setColumnCount(3);
      grid_model->setHeaderData(0, Qt::Horizontal, tr("Filter"));
      grid_model->setHeaderData(1, Qt::Horizontal, tr("Output ABS MAG Column"));
      grid_model->setHeaderData(2, Qt::Horizontal, tr("Action"));
      ui->tbl_AbsMag->setModel(grid_model);

      for (auto pair_item : m_abs_mag_config) {
          QStandardItem* item_filter = new QStandardItem(pair_item.first);
          QStandardItem* item_col = new QStandardItem(pair_item.second);
          QStandardItem* item_btn = new QStandardItem("");

          QList<QStandardItem*> items;
          items.push_back(item_filter);
          items.push_back(item_col);
          items.push_back(item_btn);
          grid_model->appendRow(items);
          
      }
      
      for( int i=0; i<grid_model->rowCount(); i++ ) {
          auto item_filter = grid_model->index(i, 0); 
          auto item_button = grid_model->index(i, 2); 
          auto* cartButton = new MessageButton(QString{item_filter.data().toString()}, "Delete");
          ui->tbl_AbsMag->setIndexWidget(item_button, cartButton);
          connect(cartButton, SIGNAL(MessageButtonClicked(const QString&)), this, SLOT(deletButtonClicked(const QString&)));
      }
      
      QFont font = ui->tbl_AbsMag->horizontalHeader()->font();
      font.setPointSize(11);
      ui->tbl_AbsMag->horizontalHeader()->setFont(font);
      ui->tbl_AbsMag->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); 
  }
  
  
  void DialogAbsMag::filterPopupClosing(std::string filter) {
     auto column = "ABS_MAG_"+filter;
     std::replace(column.begin(), column.end(), '/', '_');
     QStandardItem* item_filter = new QStandardItem(QString::fromStdString(filter));

     QStandardItem* item_col = new QStandardItem(QString::fromStdString(column));
     QStandardItem* item_btn = new QStandardItem("");
     
     QList<QStandardItem*> items;
     items.push_back(item_filter);
     items.push_back(item_col);
     items.push_back(item_btn);
     

     qobject_cast<QStandardItemModel*>(ui->tbl_AbsMag->model())->appendRow(items);
     
     auto* cartButton = new MessageButton(QString::fromStdString(filter), "Delete");
     ui->tbl_AbsMag->setIndexWidget(item_btn->index(), cartButton);
     connect(cartButton, SIGNAL(MessageButtonClicked(const QString&)), this, SLOT(deletButtonClicked(const QString&)));


  }

  std::list<std::pair<QString, QString>> DialogAbsMag::getAbsMagConfig() {
      return m_abs_mag_config;
  }
  
  void DialogAbsMag::on_btn_SelectFilter_clicked() {
      std::unique_ptr<DialogFilterSelector> dialog(new DialogFilterSelector(m_filter_repository));
      dialog->setFilter("");
      connect(dialog.get(), SIGNAL(popupClosing(std::string)), this, SLOT(filterPopupClosing(std::string)));
      dialog->exec();
  }
  
  
  
  void DialogAbsMag::deletButtonClicked(const QString& filter) {
       auto* model =qobject_cast<QStandardItemModel*>(ui->tbl_AbsMag->model());
       int row_index;
       for(row_index = 0; row_index<model->rowCount();++row_index){
          if (model->item(row_index,0)->text()==filter) {
             break;
          }
      }
      
      if (row_index<model->rowCount()) {
          model->removeRows(row_index,1);
      } 
  }
  
  void DialogAbsMag::on_btn_cancel_clicked() {
      reject();
  }
  
  void DialogAbsMag::on_btn_save_clicked() {
      std::list<std::pair<QString, QString>> new_abs_mag_config = {};
      auto* model =qobject_cast<QStandardItemModel*>(ui->tbl_AbsMag->model());

      std::set<QString> filters{};
      std::set<QString> columns{};
      for(int row_index = 0; row_index<model->rowCount();++row_index){
         new_abs_mag_config.push_back(std::make_pair(model->item(row_index,0)->text(), model->item(row_index,1)->text()));
         filters.insert(model->item(row_index,0)->text());
         columns.insert(model->item(row_index,1)->text());
      }
      
      if (filters.size()<model->rowCount()) {
          QMessageBox msgBox(this);
          msgBox.setText("You have selected duplicate filters. Please check your selection.");
          msgBox.exec();
      } else if (columns.size()<model->rowCount()) {
          QMessageBox msgBox(this);
          msgBox.setText("You have selected duplicate column names. Please check your selection.");
          msgBox.exec();
      } else {
          m_abs_mag_config = new_abs_mag_config;
          accept();
      }
  }
}  // namespace PhzQtUI
}  // namespace Euclid
