#include "PhzQtUI/ModelSetTable.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItemModel>
#include <dirent.h>
#include <iostream>

namespace Euclid {
namespace PhzQtUI {

ModelSetTable::ModelSetTable(QWidget*& parent) : QTableView(parent) {}

void ModelSetTable::load(std::shared_ptr<ModelSetModel> model_set_model_ptr) {
  setModel(model_set_model_ptr.get());
  this->setColumnHidden(2, true);
  this->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->setSelectionMode(QAbstractItemView::SingleSelection);
  this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  this->update(QModelIndex());
}

}  // namespace PhzQtUI
}  // namespace Euclid
