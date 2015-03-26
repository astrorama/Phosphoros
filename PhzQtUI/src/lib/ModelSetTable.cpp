#include <iostream>
#include <dirent.h>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QHeaderView>
#include "PhzQtUI/ModelSetTable.h"

namespace Euclid {
namespace PhzQtUI {


ModelSetTable::ModelSetTable(QWidget*& parent): QTableView(parent){

}

void ModelSetTable::loadFromPath(std::string path){
    ModelSetModel* model = new ModelSetModel();
    model->loadSets(path);
    setModel(model);
    this->setColumnHidden(2, true);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->update(QModelIndex());

}

ModelSetModel* ModelSetTable::getModel(){
    return static_cast<ModelSetModel*>(this->model());
}

void ModelSetTable::deleteSelectedSet(bool deletFile){
    QModelIndexList index = this->selectionModel()->selectedIndexes();
    getModel()->deleteSet(index[0].row(),deletFile);
}

void ModelSetTable::saveSelectedSet(std::string oldName){
    QModelIndexList index = this->selectionModel()->selectedIndexes();
    getModel()->saveSet(index[0].row(),oldName);
}

void ModelSetTable::newSet(bool duplicate_selected){
    int index_to_duplicate =-1;
    if (duplicate_selected){
        QModelIndexList index = this->selectionModel()->selectedIndexes();
        index_to_duplicate=index[0].row();
    }

    int row_to_select = getModel()->newSet(index_to_duplicate);
    this->selectRow(row_to_select);
}

bool ModelSetTable::hasSelectedSet() const{
    return this->selectionModel()->currentIndex().isValid();
}

QString ModelSetTable::getSelectedName(){
    QModelIndexList index = this->selectionModel()->selectedRows();
    return getModel()->getName(index[0].row());
}

bool ModelSetTable::setSelectedName(QString new_name){
     QModelIndexList index = this->selectionModel()->selectedIndexes();
     return getModel()->setName(index[0].row(),new_name);
}


std::map<int,ParameterRule> ModelSetTable::getSelectedParameterRules(){
    QModelIndexList index = this->selectionModel()->selectedRows();
    return getModel()->getParameterRules(index[0].row());
}


void ModelSetTable::setSelectedRules(const std::map<int,ParameterRule>& new_value){
    QModelIndexList index = this->selectionModel()->selectedIndexes();
    getModel()->setParameterRules(index[0].row(),new_value);
}




void ModelSetTable::updateModelNumberForSelected(){
    QModelIndexList index = this->selectionModel()->selectedIndexes();
    getModel()->updateModelCount(index[0].row());
}

}
}
