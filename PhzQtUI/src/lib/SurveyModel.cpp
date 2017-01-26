#include "PhzQtUI/SurveyModel.h"

namespace Euclid {
namespace PhzQtUI {

SurveyModel::SurveyModel(){ }

void SurveyModel::loadSurvey(){

    m_survey_filter_mappings=SurveyFilterMapping::loadCatalogMappings();

    this->setColumnCount(3);
    this->setRowCount(m_survey_filter_mappings.size());
    QStringList  setHeaders;
    setHeaders<<"Catalog Type"<<"Number of Filters"<<"Hidden_Id";
    this->setHorizontalHeaderLabels(setHeaders);

    int i=0;
    for(auto it = m_survey_filter_mappings.begin(); it != m_survey_filter_mappings.end(); ++it ) {
        this->setItem(i,0,new QStandardItem(QString::fromStdString(it->second.getName())));
        this->setItem(i,1,new QStandardItem(QString::number(it->second.getFilterNumber())));
        this->setItem(i,2,new QStandardItem(QString::number(it->first)));
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
        m_survey_filter_mappings[max_ref]=SurveyFilterMapping{};

        int ref = getValue(duplicate_from_row,2).toInt();
        SurveyFilterMapping ref_survey=m_survey_filter_mappings.at(ref);

        m_survey_filter_mappings[max_ref].setName(getDuplicateName(ref_survey.getName()));
        m_survey_filter_mappings[max_ref].setNonDetection(ref_survey.getNonDetection());
        m_survey_filter_mappings[max_ref].setSourceIdColumn(ref_survey.getSourceIdColumn());
        m_survey_filter_mappings[max_ref].setColumnList(ref_survey.getColumnList());
        m_survey_filter_mappings[max_ref].setFilters(ref_survey.getFilters());
    }
    else{
         m_survey_filter_mappings[max_ref]=SurveyFilterMapping{};
         m_survey_filter_mappings.at(max_ref).setName("New_Catalog_Filter_Mapping");
    }

    QList<QStandardItem*> items;

    items.push_back(new QStandardItem(QString::fromStdString(m_survey_filter_mappings.at(max_ref).getName())));
    items.push_back(new QStandardItem(QString::number(m_survey_filter_mappings.at(max_ref).getFilterNumber())));
    items.push_back(new QStandardItem(QString::number(max_ref)));

    this->appendRow(items);
    return items[0]->row();
}

void SurveyModel::deleteSurvey(int row){
    int ref = getValue(row,2).toInt();
    m_survey_filter_mappings.at(ref).deleteSurvey();
    m_survey_filter_mappings.erase(ref);
    this->removeRow(row);
}

void SurveyModel::saveSurvey(int row,std::string oldName){
    int ref = getValue(row,2).toInt();
    m_survey_filter_mappings.at(ref).saveSurvey(oldName);
}

bool SurveyModel::checkUniqueName(QString new_name, int row) const{
        int ref = -1;
        if (row>=0){
          ref =getValue(row,2).toInt();
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
    int ref = getValue(row,2).toInt();
    this->item(row,0)->setText(QString::fromStdString(newName));
    m_survey_filter_mappings.at(ref).setName(newName);
}

void SurveyModel::setSourceIdColumn(std::string newSourceIdColumn, int row){
    int ref = getValue(row,2).toInt();
    m_survey_filter_mappings.at(ref).setSourceIdColumn(newSourceIdColumn);
}

void SurveyModel::setFilters(std::vector<FilterMapping> newFilters, int row){
    int ref = getValue(row,2).toInt();
    m_survey_filter_mappings.at(ref).setFilters(std::move(newFilters));

    this->item(row,1)->setText(QString::number(m_survey_filter_mappings.at(ref).getFilterNumber()));
}

const QString SurveyModel::getValue(int row,int column) const{
    return this->item(row,column)->text();
}

std::string SurveyModel::getName( int row){
    return getValue(row,0).toStdString();
}

std::string SurveyModel::getSourceIdColumn( int row){
    int ref = getValue(row,2).toInt();
    return m_survey_filter_mappings.at(ref).getSourceIdColumn();
}

const std::set<std::string>& SurveyModel::getColumnList(int row) const{
  int ref = getValue(row,2).toInt();
  return m_survey_filter_mappings.at(ref).getColumnList();
}

void SurveyModel::setColumnList(std::set<std::string> new_list, int row){
  int ref = getValue(row,2).toInt();
     m_survey_filter_mappings.at(ref).setColumnList(std::move(new_list));
}

const std::vector<FilterMapping>&  SurveyModel::getFilters(int row){
    int ref = getValue(row,2).toInt();
    return m_survey_filter_mappings.at(ref).getFilters();
}

void SurveyModel::setDefaultCatalog(std::string new_default_catalog, int row){
  int ref = getValue(row,2).toInt();
     m_survey_filter_mappings.at(ref).setDefaultCatalogFile(new_default_catalog);
}

void SurveyModel::setNonDetection(double newNonDetection, int row){
  int ref = getValue(row,2).toInt();
  m_survey_filter_mappings.at(ref).setNonDetection(newNonDetection);
}

 double SurveyModel::getNonDetection(int row){
   int ref = getValue(row,2).toInt();
   return m_survey_filter_mappings.at(ref).getNonDetection();
 }


 void SurveyModel::setHasUpperLimit(bool has_upper_limit, int row){
   int ref = getValue(row,2).toInt();
   m_survey_filter_mappings.at(ref).setHasUpperLimit(has_upper_limit);
  }

  bool SurveyModel::getHasUpperLimit(int row){
    int ref = getValue(row,2).toInt();
      return m_survey_filter_mappings.at(ref).getHasUpperLimit();
  }

  void SurveyModel::setHasMissingPhot(bool has_missing_phot, int row){
    int ref = getValue(row,2).toInt();
    m_survey_filter_mappings.at(ref).setHasMissingPhotometry(has_missing_phot);
  }

  bool SurveyModel::getHasMissingPhot(int row){
    int ref = getValue(row,2).toInt();
          return m_survey_filter_mappings.at(ref).getHasMissingPhotometry();
  }

std::string SurveyModel::getDefaultCatalog(int row) const{
  int ref = getValue(row,2).toInt();
  return m_survey_filter_mappings.at(ref).getDefaultCatalogFile();
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
