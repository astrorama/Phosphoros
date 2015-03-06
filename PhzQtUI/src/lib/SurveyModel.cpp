#include "PhzQtUI/SurveyModel.h"

namespace Euclid {
namespace PhzQtUI {

SurveyModel::SurveyModel(){ }

void SurveyModel::loadSurvey(std::string path){
    m_root_path=path;
    m_survey_filter_mappings=SurveyFilterMapping::loadSurveysFromFolder(path);

    this->setColumnCount(4);
    this->setRowCount(m_survey_filter_mappings.size());
    QStringList  setHeaders;
    setHeaders<<"Survey Name"<<"Number of Filters"<<"Filters"<<"Hidden_Id";
    this->setHorizontalHeaderLabels(setHeaders);

    int i=0;
    for(auto it = m_survey_filter_mappings.begin(); it != m_survey_filter_mappings.end(); ++it ) {
        this->setItem(i,0,new QStandardItem(QString::fromStdString(it->second.getName())));
        this->setItem(i,1,new QStandardItem(QString::number(it->second.getFilterNumber())));
        this->setItem(i,2,new QStandardItem(QString::fromStdString(it->second.getFilterNames(", "))));
        this->setItem(i,3,new QStandardItem(QString::number(it->first)));
        ++i;
    }
}

int SurveyModel::newSurvey(int duplicate_from_row ){
    int max_ref = 0;

    for(auto it = m_survey_filter_mappings.begin(); it != m_survey_filter_mappings.end(); ++it ) {
        if (it->first > max_ref) {
            max_ref = it->first;
        }
    }

    ++max_ref;

    if (duplicate_from_row>=0){
        int ref = getValue(duplicate_from_row,3).toInt();
        SurveyFilterMapping ref_survey=m_survey_filter_mappings.at(ref);

        m_survey_filter_mappings[max_ref]=ref_survey;

        std::string new_name = getDuplicateName(m_survey_filter_mappings[max_ref].getName());
        m_survey_filter_mappings[max_ref].setName(new_name);

    }
    else{
         m_survey_filter_mappings[max_ref]=SurveyFilterMapping(m_root_path);
         m_survey_filter_mappings.at(max_ref).setName("New_Survey_Filter_Mapping");
    }

    QList<QStandardItem*> items;

    items.push_back(new QStandardItem(QString::fromStdString(m_survey_filter_mappings.at(max_ref).getName())));
    items.push_back(new QStandardItem(QString::number(m_survey_filter_mappings.at(max_ref).getFilterNumber())));
    items.push_back(new QStandardItem(QString::fromStdString(m_survey_filter_mappings.at(max_ref).getFilterNames(", "))));
    items.push_back(new QStandardItem(QString::number(max_ref)));

    this->appendRow(items);
    return items[0]->row();
}

void SurveyModel::deleteSurvey(int row){
    int ref = getValue(row,3).toInt();
    m_survey_filter_mappings.at(ref).deleteSurvey();
    m_survey_filter_mappings.erase(ref);
    this->removeRow(row);
}

void SurveyModel::saveSurvey(int row,std::string oldName){
    int ref = getValue(row,3).toInt();
    m_survey_filter_mappings.at(ref).saveSurvey(oldName);
}

bool SurveyModel::checkUniqueName(QString new_name, int row) const{
        int ref = -1;
        if (row>=0){
          ref =getValue(row,3).toInt();
        }
    for(auto it = m_survey_filter_mappings.begin(); it != m_survey_filter_mappings.end(); ++it ) {
        if (it->first==ref){
            continue;
        }

        if(new_name.toStdString().compare(it->second.getName())==0){
            return false;
        }
    }

    return true;
}


void SurveyModel::setName(std::string newName, int row){
    int ref = getValue(row,3).toInt();
    this->item(row,0)->setText(QString::fromStdString(newName));
    m_survey_filter_mappings.at(ref).setName(newName);
}

void SurveyModel::setSourceIdColumn(std::string newSourceIdColumn, int row){
    int ref = getValue(row,3).toInt();
    m_survey_filter_mappings.at(ref).setSourceIdColumn(newSourceIdColumn);
}

void SurveyModel::setFilters(std::list<FilterMapping> newFilters, int row){
    int ref = getValue(row,3).toInt();
    m_survey_filter_mappings.at(ref).setFilters(std::move(newFilters));

    this->item(row,1)->setText(QString::number(m_survey_filter_mappings.at(ref).getFilterNumber()));
    this->item(row,2)->setText(QString::fromStdString(m_survey_filter_mappings.at(ref).getFilterNames(", ")));
}

const QString SurveyModel::getValue(int row,int column) const{
    return this->item(row,column)->text();
}

std::string SurveyModel::getName( int row){
    return getValue(row,0).toStdString();
}

std::string SurveyModel::getSourceIdColumn( int row){
    int ref = getValue(row,3).toInt();
    return m_survey_filter_mappings.at(ref).getSourceIdColumn();
}

const std::list<FilterMapping>&  SurveyModel::getFilters(int row){
    int ref = getValue(row,3).toInt();
    return m_survey_filter_mappings.at(ref).getFilters();
}

std::string SurveyModel::getDuplicateName(std::string name) const{

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

}
}
