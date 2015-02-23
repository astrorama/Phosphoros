#include <iostream>
#include <dirent.h>
#include "QtUI/ModelSet.h"
#include "QtUI/ModelSetModel.h"

ModelSetModel::ModelSetModel():QStandardItemModel()
{
}

const QString ModelSetModel::getName(int row) const{
    return getValue(row,0);
}

bool ModelSetModel::setName(int row, const QString& value){
    int id = getValue(row,2).toInt();

    if (!checkUniqueName(value,id)){
        return false;
    }
    m_set_list[id].setName(value.toStdString());
    setValue( row,0, value);
    return true;
}

const QString ModelSetModel::getNumber(int row) const{
    return getValue(row,1);
}

const QString ModelSetModel::getRef(int row) const{
    return getValue(row,2);
}

const QString ModelSetModel::getValue(int row,int column) const{
    return this->item(row,column)->text();
}

const std::map<int,Euclid::PhosphorosUiDm::ParameterRule> ModelSetModel::getParameterRules(int row) const{
    int key = getRef(row).toInt();
    return m_set_list.at(key).getParameterRules();
}


void ModelSetModel::setParameterRules(int row, const std::map<int,Euclid::PhosphorosUiDm::ParameterRule>& value){
     int key = getRef(row).toInt();
     m_set_list[key].setParameterRules(std::move(std::map<int,Euclid::PhosphorosUiDm::ParameterRule>(value))); // copy to anew object
}

void ModelSetModel::setValue(int row,int column, const QString& value){
    this->setData(this->index(row,column),value);
}

void ModelSetModel::loadSets(const std::string& path){
    m_root_path=path;
     m_set_list=Euclid::PhosphorosUiDm::ModelSet::loadModelSetsFromFolder(path);

     this->setColumnCount(3);
     this->setRowCount(m_set_list.size());
     QStringList  setHeaders;
     setHeaders<<"Model Set Name"<<"Number of Models"<<"Hidden_Id";
     this->setHorizontalHeaderLabels(setHeaders);

     int i=0;
     for(auto it = m_set_list.begin(); it != m_set_list.end(); ++it ) {
         this->setItem(i,0,new QStandardItem(QString::fromStdString(it->second.getName())));
         this->setItem(i,1,new QStandardItem(QString::number(it->second.getModelNumber())));
         this->setItem(i,2,new QStandardItem(QString::number(it->first)));
         ++i;
     }
}


void ModelSetModel::updateModelCount(int row){
    int key = getRef(row).toInt();
    this->item(row,1)->setText(QString::number(m_set_list[key].getModelNumber()));
}

int ModelSetModel::newSet(int duplicate_from_row ){
    int max_ref = 0;

    for(auto it = m_set_list.begin(); it != m_set_list.end(); ++it ) {
        if (it->first > max_ref) {
            max_ref = it->first;
        }
    }

    ++max_ref;

    QString text_1 = "New Model Set";
    QString text_2 = "0";




    Euclid::PhosphorosUiDm::ModelSet set(m_root_path);


    if (duplicate_from_row>=0){
        text_1=getName(duplicate_from_row)+ "_Copy";
        text_2=getNumber(duplicate_from_row);

        set.setParameterRules(getParameterRules(duplicate_from_row));
    }

     set.setName(text_1.toStdString());

    m_set_list[max_ref]=set;


    QList<QStandardItem*> items;
    items.push_back(new QStandardItem(text_1));
    items.push_back(new QStandardItem(text_2));
    items.push_back(new QStandardItem(QString::number(max_ref)));
    this->appendRow(items);
    return items[0]->row();
}

void ModelSetModel::deleteSet(int row,bool deletFile){
    int ref = getRef(row).toInt();
    if (deletFile){
         m_set_list.at(ref).deleteModelSet();
    }
    m_set_list.erase(ref);
    this->removeRow(row);
}

void ModelSetModel::saveSet(int row,std::string oldName){
    int ref = getRef(row).toInt();
    m_set_list.at(ref).saveModelSet(oldName);
}

bool ModelSetModel::checkUniqueName(QString new_name, int id){


    // TODO Complete by a check that the name can be a name of a file (no reserved char)
    for(auto it = m_set_list.begin(); it != m_set_list.end(); ++it ) {
        if (it->first==id){
            continue;
        }

        if(new_name.toStdString().compare(it->second.getName())==0){
            return false;
        }
    }

    return true;
}


 const std::string ModelSetModel::getRootPath() const{
    return m_root_path;
}


