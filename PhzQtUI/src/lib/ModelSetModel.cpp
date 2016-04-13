#include <iostream>
#include <dirent.h>
#include "PhzQtUI/ModelSet.h"
#include "PhzQtUI/ModelSetModel.h"

namespace Euclid {
namespace PhzQtUI {


ModelSetModel::ModelSetModel():QStandardItemModel()
{
}

std::map<std::string,PhzDataModel::ModelAxesTuple> ModelSetModel::getAxesTuple(int row) const{
  int id = getValue(row,2).toInt();
  const std::map<int,ModelSet>& ref=m_set_list;
  return ref.at(id).getAxesTuple();
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

const std::map<int,ParameterRule> ModelSetModel::getParameterRules(int row) const{
    int key = getRef(row).toInt();
    return m_set_list.at(key).getParameterRules();
}


void ModelSetModel::setParameterRules(int row, const std::map<int,ParameterRule>& value){
     int key = getRef(row).toInt();
     m_set_list[key].setParameterRules(std::move(std::map<int,ParameterRule>(value))); // copy to anew object
}

void ModelSetModel::setValue(int row,int column, const QString& value){
    this->setData(this->index(row,column),value);
}

void ModelSetModel::loadSets(const std::string& path){
    m_root_path=path;
     m_set_list=ModelSet::loadModelSetsFromFolder(path);

     this->setColumnCount(3);
     this->setRowCount(m_set_list.size());
     QStringList  setHeaders;
     setHeaders<<"Name"<<"Total Size"<<"Hidden_Id";
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
    this->item(row,1)->setText(QString::number(m_set_list[key].getModelNumber(true)));
}

int ModelSetModel::newSet(int duplicate_from_row ){
    int max_ref = 0;

    for(auto it = m_set_list.begin(); it != m_set_list.end(); ++it ) {
        if (it->first > max_ref) {
            max_ref = it->first;
        }
    }

    ++max_ref;

    QString text_1 = "New_Parameter_Space";
    QString text_2 = "0";




    ModelSet set(m_root_path);


    if (duplicate_from_row>=0){
        text_1=QString::fromStdString(getDuplicateName(getName(duplicate_from_row).toStdString()));
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

bool ModelSetModel::checkUniqueName(QString new_name, int id) const{


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



std::string ModelSetModel::getDuplicateName(std::string name) const{
  auto new_name=name+ "_Copy";
  if (checkUniqueName(QString::fromStdString(new_name),-1)){
    return new_name;
  }

  int i=2;
  while(!checkUniqueName(QString::fromStdString(new_name + "("+std::to_string(i)+")"),-1)){
    ++i;
  }

  return new_name + "("+std::to_string(i)+")";
}


 const std::string ModelSetModel::getRootPath() const{
    return m_root_path;
}

}
}
