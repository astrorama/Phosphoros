#ifndef SURVEYMODEL_H
#define SURVEYMODEL_H

#include <QStandardItemModel>
#include "SurveyFilterMapping.h"
#include <map>


class SurveyModel : public QStandardItemModel
{
public:

    SurveyModel();
    void loadSurvey(std::string path);
    int newSurvey(int duplicate_from_row );
    void saveSurvey(int row,std::string oldName);
    void deleteSurvey(int row);
    bool checkUniqueName(QString new_name, int row) const;

    void setName(std::string newName, int row);
    void setSourceIdColumn(std::string newSourceIdColumn, int row);
    void setFilters(std::list<Euclid::PhosphorosUiDm::FilterMapping> newFilters, int row);


    std::string getName( int row);
    std::string getSourceIdColumn( int row);
    const std::list<Euclid::PhosphorosUiDm::FilterMapping>&  getFilters(int row);

private:
    std::map<int,Euclid::PhosphorosUiDm::SurveyFilterMapping> m_survey_filter_mappings;
    const QString getValue(int row,int column) const;
    std::string m_root_path;
};

#endif // SURVEYMODEL_H
