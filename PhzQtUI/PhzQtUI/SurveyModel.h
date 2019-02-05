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
 * This class provide a Model to be used in TableView. It store a survey on
 * each of its row and handle the loading, selection and edition of the survey.
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

  /**
   * @brief Select the survey located at the row "row", and set the selected
   * survey as preferred one.
   * If the previously selected survey was edited, these modification are discarded.
   * If the provided row is not in the rows range create an empty selection.
   *
   * @param row the row number to be selected
   */
  void selectSurvey(int row);

  /**
   * @brief Select the survey which name match the provided "name", and set the
   * selected survey as preferred one.
   * If the previously selected survey was edited, these modification are discarded.
   * If the provided name do not match any survey create an empty selection.
   *
   *  @param name the name of the survey to select
   */
  void selectSurvey(QString name);

  /**
   *  @brief Returns a reference on the selected survey.
   *  If no survey are selected the reference point to an empty survey.
   *
   *  @return a reference on the selected survey
   */
  const SurveyFilterMapping& getSelectedSurvey();

  /**
   * @brief Returns the number of the row the selected survey is associated with
   */
  int getSelectedRow();

  /**
   * @brief Return the status of the selected survey. If the survey has been
   * modified this function return true, otherwise false.
   *
   * @return  true if the selected survey is modified
   */
  bool isInEdition();

  /**
   * @brief Saves the modification of the selected survey. If the operation
   * succeed returns true however if the name of the survey conflict with
   * another one the saving is canceled and the function returns false.
   *
   * @return  true if the selected survey was successfully saved
   */
  bool saveSelected();

  /**
   * @brief Discards the modifications of the selected survey.
   */
  void cancelSelected();

  /**
   * @brief Delete the selected survey. WARNING This operation will also delete
   * the configurations, intermediary results and results associated with the survey.
   */
  void deletSelected();

  /**
   * @brief Returns the list of survey names contained in this model.
   */
  const std::vector<QString> getSurveyList() const;

  bool doNeedReload() const;

  void reloaded();

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
  bool m_need_reload = true;
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
