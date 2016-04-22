#ifndef SURVEYMODEL_H
#define SURVEYMODEL_H

#include <QStandardItemModel>
#include "SurveyFilterMapping.h"
#include <map>

namespace Euclid {
namespace PhzQtUI {


/**
 * @brief The SurveyModel class
 * This class provide a Model to be used in TableView.
 * It handle the SurveyFilterMappings.
 */
class SurveyModel: public QStandardItemModel {
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
  int newSurvey(int duplicate_from_row);

  /**
   * @brief Write the survey represented by the row 'row' of the Model to a xml file.
   * If the name has changed, also delete the old file
   * @oldName previous name of the survey, giving the name of the file to be
   * removed if the survey name has changed.
   */
  void saveSurvey(int row, std::string oldName);

  /**
   * @brief Remove the row of the model and delete the corresponding survey and its
   * persistence file.
   *
   * @param row the row to be removed.
   */
  void deleteSurvey(int row);

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
   * @brief Change the name of the Survey represented by the given row
   *
   * @param newName the new name to be assigned to the survey
   *
   * @param row the model row number representing the survey
   */
  void setName(std::string newName, int row);

  /**
   * @brief Get the name of the survey represented by the row 'row'
   *
   * @param row the model row number representing the survey.
   */
  std::string getName(int row);

  void setNonDetection(double newNonDetection, int row);

  double getNonDetection(int row);

  /**
   * @brief Change the Source column Id of the Survey represented by the given
   * row.
   *
   * @param newSourceIdColumn the new Source column Id name to be assigned to
   * the survey
   *
   * @param row the model row number representing the survey.
   */
  void setSourceIdColumn(std::string newSourceIdColumn, int row);

  /**
   * @brief Get the Source column Id of the survey represented by the row 'row'
   *
   * @param row the model row number representing the survey.
   */
  std::string getSourceIdColumn(int row);

  /**
     * @brief get the list of column stored into the Survey represented by the row 'row'.
     * @param row the model row number representing the survey.
     */
  const std::set<std::string>& getColumnList(int row) const;

    /**
     * @brief Move the list of column into the survey represented by the row 'row'.
     * @param row the model row number representing the survey.
     */
  void setColumnList(std::set<std::string> new_list, int row);


  /**
   * @brief set the default catalog path into the survey represented by the row 'row'.
   * @param row the model row number representing the survey.
   * @param new_default_catalog
   */
  void setDefaultCatalog(std::string new_default_catalog, int row);

  /**
   * @brief get the default catalog path from the survey represented by the row 'row'
   * @return the current default catalog path
   */
  std::string getDefaultCatalog(int row) const;

  /**
   * @brief Change the Filters Mappings the Survey represented by the given
   * row.
   *
   * @param newFilters the list of FilterMApping to be assigned to
   * the survey
   *
   * @param row the model row number representing the survey.
   */
  void setFilters(std::vector<FilterMapping> newFilters,
      int row);

  /**
   * @brief Get the Filters Mappings of the survey represented by the row 'row'
   *
   * @param row the model row number representing the survey.
   */
  const std::vector<FilterMapping>& getFilters(int row);

private:
  std::map<int, SurveyFilterMapping> m_survey_filter_mappings;
  const QString getValue(int row, int column) const;


  std::string getDuplicateName(std::string name) const;
};

}
}

#endif // SURVEYMODEL_H
