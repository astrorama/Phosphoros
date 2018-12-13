#ifndef SURVEYMODEL_H
#define SURVEYMODEL_H

#include <QString>
#include <QStandardItemModel>
#include "SurveyFilterMapping.h"
#include <map>
#include <vector>
#include <set>
#include <string>

namespace Euclid {
namespace PhzQtUI {


/**
 * @brief The SurveyModel class
 * This class provide a Model to be used in TableView.
 * It handle the SurveyFilterMappings.
 */
class SurveyModel: public QStandardItemModel {
 Q_OBJECT
public:

  SurveyModel();

  /**
   * @brief Initialise the SurveyModel by setting its internal data
   * @param path is the folder into which the model has to look for persisted
   * SurveyFilterMappings
   */
  void loadSurvey();

  /**
   * @brief Create a new survey and add it to the model.
   *
   * @param duplicate_from_row if >0 pre-fill the new survey with the values
   * found into the row with the provided number.
   */
  void newSurvey(bool duplicate_from_selected = false);

  /**
   * @brief Check that no other survey have the same name as the one provided. This
   * has to be done has the name is use for tnaming the persistance file.
   *
   * @param new_name the proposed name to be checked to be unique.
   *
   * @param row the current Survey reference (allowing for unchanged name).
   */
  bool checkUniqueName(QString new_name, int row) const;

  void selectSurvey(int row);
  void selectSurvey(QString name);

  const SurveyFilterMapping& getSelectedSurvey();
  bool isInEdition();
  bool saveSelected();
  void cancelSelected();
  void deletSelected();
  int getSelectedRow();
  const std::vector<QString> getSurveyList() const;

public slots:
  void setNameToSelected(QString new_name);
  void setIdColumnToSelected(QString new_name);
  void setRaColumnToSelected(QString new_name);
  void setDecColumnToSelected(QString new_name);
  void setGalEbvColumnToSelected(QString new_name);
  void setFiltersToSelected(std::vector<FilterMapping> new_filters);
  void setColumnListToSelected(std::set<std::string> new_list);
  void setDefaultCatalogToSelected(QString new_name);
  void setNonDetectionToSelected(QString new_name);
  void setHasUpperLimitToSelected(bool has_upper_limit);
  void setHasMissingPhotToSelected(bool has_missing_phot);
  void setCopiedColumnsToSelected(std::map<std::string, std::string> copied_columns);

private:
  bool m_in_edition = false;
  int m_selected_row = -1;
  int m_selected_index = -1;
  SurveyFilterMapping m_edited_survey;
  std::map<int, SurveyFilterMapping> m_survey_filter_mappings;
  const QString getValue(int row, int column) const;


  std::string getDuplicateName(std::string name) const;
};

}
}

#endif //  SURVEYMODEL_H
