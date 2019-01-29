#include "PhzQtUI/SurveyModel.h"
#include "PreferencesUtils.h"
#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("SurveyModel");

SurveyModel::SurveyModel(){ }




void SurveyModel::newSurvey(bool duplicate_from_selected) {
  // Get the new ref
  int max_ref = 0;
  for(auto it = m_survey_filter_mappings.begin(); it != m_survey_filter_mappings.end(); ++it) {
      if (it->first > max_ref) {
          max_ref = it->first;
      }
  }
  ++max_ref;

  if (duplicate_from_selected && getSelectedRow() >= 0) {
      m_survey_filter_mappings[max_ref] = SurveyFilterMapping{};

      auto& ref_survey = getSelectedSurvey();

      m_survey_filter_mappings[max_ref].setName(getDuplicateName(ref_survey.getName()));
      m_survey_filter_mappings[max_ref].setNonDetection(ref_survey.getNonDetection());
      m_survey_filter_mappings[max_ref].setSourceIdColumn(ref_survey.getSourceIdColumn());
      m_survey_filter_mappings[max_ref].setColumnList(ref_survey.getColumnList());
      m_survey_filter_mappings[max_ref].setFilters(ref_survey.getFilters());
  } else {
       m_survey_filter_mappings[max_ref] = SurveyFilterMapping {};
       m_survey_filter_mappings.at(max_ref).setName("New_Catalog_Filter_Mapping");
  }

  QList<QStandardItem*> items;
  items.push_back(new QStandardItem(QString::fromStdString(m_survey_filter_mappings.at(max_ref).getName())));
  items.push_back(new QStandardItem(QString::number(m_survey_filter_mappings.at(max_ref).getFilterNumber())));
  items.push_back(new QStandardItem(QString::number(max_ref)));
  this->appendRow(items);
  selectSurvey(items[0]->row());
}

const QString SurveyModel::getValue(int row,int column) const {
    return this->item(row, column)->text();
}

bool SurveyModel::checkUniqueName(QString new_name, int row) const {
        int ref = -1;
        if (row >= 0) {
          ref = getValue(row, 2).toInt();
        }
    for (auto it = m_survey_filter_mappings.begin(); it != m_survey_filter_mappings.end(); ++it) {
        if (it->first == ref) {
            continue;
        }

        if (new_name.toStdString().compare(it->second.getName()) == 0) {
            return false;
        }
    }

    return true;
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

/////////////////////////////////////////////////
void SurveyModel::loadSurvey() {
    m_survey_filter_mappings = SurveyFilterMapping::loadCatalogMappings();
    this->setColumnCount(3);
    this->setRowCount(m_survey_filter_mappings.size());
    QStringList  setHeaders;
    setHeaders << "Catalog Type" << "Number of Filters" << "Hidden_Id";
    this->setHorizontalHeaderLabels(setHeaders);

    int i = 0;
    for (auto& it : m_survey_filter_mappings) {
        this->setItem(i, 0, new QStandardItem(QString::fromStdString(it.second.getName())));
        this->setItem(i, 1, new QStandardItem(QString::number(it.second.getFilterNumber())));
        this->setItem(i, 2, new QStandardItem(QString::number(it.first)));
        ++i;
    }
    auto saved_catalog = PreferencesUtils::getUserPreference("_global_selection_", "catalog");
    logger.info()<< "use the saved catalog "<<saved_catalog<<".";
    selectSurvey(QString::fromStdString(saved_catalog));
}

 void SurveyModel::selectSurvey(int row) {
   if (row >= 0 && row < static_cast<int>(m_survey_filter_mappings.size())) {
     m_selected_row = row;
     m_selected_index = getValue(row, 2).toInt();
     m_edited_survey = m_survey_filter_mappings[m_selected_index];
     PreferencesUtils::setUserPreference(
           "_global_selection_",
           "catalog",
           m_edited_survey.getName());
   } else {
     m_selected_row = -1;
     m_selected_index = -1;
     m_edited_survey = SurveyFilterMapping();
   }
   m_in_edition = false;
 }

 void SurveyModel::selectSurvey(QString name) {
     int selected_row = -1;
     for (int row = 0; row < static_cast<int>(m_survey_filter_mappings.size()); ++row) {
       if (getValue(row, 0).compare(name) == 0) {
         selected_row = row;
         break;
       }
     }
     selectSurvey(selected_row);
 }

 int SurveyModel::getSelectedRow(){
   return m_selected_row;
 }



 const SurveyFilterMapping& SurveyModel::getSelectedSurvey() {
   return m_edited_survey;
 }

 bool SurveyModel::isInEdition() {
   return m_in_edition;
 }

 bool SurveyModel::saveSelected() {
   logger.info() << "Saving the selected catalog '" << m_edited_survey.getName() <<"'.";
   bool pre_tests = checkUniqueName(QString::fromStdString(m_edited_survey.getName()), m_selected_row);

   if (pre_tests) {
     m_survey_filter_mappings[m_selected_index] = m_edited_survey;
     m_survey_filter_mappings[m_selected_index].saveSurvey(m_survey_filter_mappings[m_selected_index].getName());
     this->setItem(m_selected_row, 0, new QStandardItem(QString::fromStdString(m_edited_survey.getName())));
     this->setItem(m_selected_row, 1, new QStandardItem(QString::number(m_edited_survey.getFilterNumber())));
     m_in_edition = false;
     return true;
   } else {
     return false;
   }
 }

 void SurveyModel::deletSelected() {
   if (m_selected_row >= 0) {
     logger.info() << "Deleting the selected catalog '" << m_edited_survey.getName() <<"'.";
     m_survey_filter_mappings.at(m_selected_index).deleteSurvey();
     m_survey_filter_mappings.erase(m_selected_index);
     this->removeRow(m_selected_row);
     selectSurvey(-1);
     m_in_edition = false;
   }
 }

 void SurveyModel::cancelSelected() {

   logger.info() << "Cancel edition on the selected catalog '" << m_edited_survey.getName() <<"'.";
   m_edited_survey = m_survey_filter_mappings[m_selected_index];
   m_in_edition = false;
 }

 void SurveyModel::setNameToSelected(QString new_name) {
   m_edited_survey.setName(new_name.toStdString());
   m_in_edition = true;
 }

 void SurveyModel::setIdColumnToSelected(QString new_name) {
   m_edited_survey.setSourceIdColumn(new_name.toStdString());
   m_in_edition = true;
 }

 void SurveyModel::setRaColumnToSelected(QString new_name) {
   m_edited_survey.setRaColumn(new_name.toStdString());
   m_in_edition = true;
 }

 void SurveyModel::setDecColumnToSelected(QString new_name) {
   m_edited_survey.setDecColumn(new_name.toStdString());
   m_in_edition = true;
 }

 void SurveyModel::setGalEbvColumnToSelected(QString new_name) {
   m_edited_survey.setGalEbvColumn(new_name.toStdString());
   m_in_edition = true;
 }

 void SurveyModel::setFiltersToSelected(std::vector<FilterMapping> new_filters) {
   m_edited_survey.setFilters(new_filters);
   m_in_edition = true;
 }

 void SurveyModel::setColumnListToSelected(std::set<std::string> new_list) {
   m_edited_survey.setColumnList(new_list);
   m_in_edition = true;
 }

 void SurveyModel::setDefaultCatalogToSelected(QString new_name) {
   m_edited_survey.setDefaultCatalogFile(new_name.toStdString());
   m_in_edition = true;
 }

 void SurveyModel::setNonDetectionToSelected(QString new_value) {
   m_edited_survey.setNonDetection(new_value.toDouble());
   m_in_edition = true;
 }

 void SurveyModel::setHasUpperLimitToSelected(bool has_upper_limit) {
   m_edited_survey.setHasUpperLimit(has_upper_limit);
   m_in_edition = true;
 }

 void SurveyModel::setHasMissingPhotToSelected(bool has_missing_phot) {
   m_edited_survey.setHasMissingPhotometry(has_missing_phot);
   m_in_edition = true;
 }


 void SurveyModel::setCopiedColumnsToSelected(std::map<std::string, std::string> copied_columns) {
   m_edited_survey.setCopiedColumns(copied_columns);
   m_in_edition = true;
 }


 const std::vector<QString> SurveyModel::getSurveyList() const {
   std::vector<QString> result = {};
   for (auto it = m_survey_filter_mappings.begin(); it != m_survey_filter_mappings.end(); ++it) {
     result.push_back(QString::fromStdString(it->second.getName()));
     }
   return result;
 }


}
}
