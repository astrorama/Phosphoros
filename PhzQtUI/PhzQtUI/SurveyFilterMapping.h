#ifndef SURVEYFILTERMAPPING_H
#define SURVEYFILTERMAPPING_H
#include <vector>
#include <map>
#include <set>
#include "FilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

/**
 * @class SurveyFilterMapping
 *
 * @brief
 *  Object storing the description of a survey and intended to be persisted.
 *
 */
class SurveyFilterMapping
{
public:
    /**
      * @brief
      *  The default constructor is only here to allow the usage of SurveyFilterMapping in lists.
      */
    SurveyFilterMapping();

    /**
      * @brief
      *  Returns the number of filter in the SurveyFilterMapping.
      */
    int getFilterNumber() const;

    /**
     * @brief set the Name (which is also the file name)
     * @param newSurveyName
     */
    void setName(std::string newSurveyName);

    /**
     * @brief get the Name
     * @return the current name
     */
    std::string getName() const;

    /**
     * @brief set the SourceIdColumn
     * @param newSourceIdColumn
     */
    void setSourceIdColumn(std::string newSourceIdColumn);

    /**
     * @brief get the SourceIdColumn
     * @return the current SourceIdColumn
     */
    std::string getSourceIdColumn() const;

    /**
     * @brief set the Filters by moving the provided object into the SurveyFilterMapping
     * @param filters
     */
    void setFilters(std::vector<FilterMapping> filters);

    /**
     * @brief get the filters Filters
     * @return a const ref on the filter list
     */
    const std::vector<FilterMapping>& getFilters() const;

    /**
     * @brief get the list of column stored into the Survey.
     */
    const std::set<std::string>& getColumnList() const;

    /**
     * @brief Move the list of column inbto the Survey.
     */
    void setColumnList(std::set<std::string> new_list);

    void setNonDetection(double non_detection);

    double getNonDetection() const;


    /**
     * @brief set the default catalog path
     * @param new_default_catalog
     */
    void setDefaultCatalogFile(std::string new_default_catalog);

    /**
     * @brief get the default catalog path
     * @return the current default catalog path
     */
    std::string getDefaultCatalogFile() const;



  /**
   * @brief get the list of available catalogs
   */
  static std::vector<std::string> getAvailableCatalogs();

  /**
   * @brief Read from the disk all the SurveyFilterMapping .
   *
   * @return a map of SurveyFilterMapping indexed by a int key.
   */
  static std::map<int, SurveyFilterMapping> loadCatalogMappings();

  /**
   * @brief load a SurveyFilterMapping from a xml file
   * @param name
   * @return the SurveyFilterMapping.
   */
  static SurveyFilterMapping loadCatalog(std::string name);

  /**
   * @brief Delete the current survey by erasing the file it was persisted in.
   */
  void deleteSurvey();

  /**
   * @brief Write the SurveyFilterMapping in a xml file which path is <root_path>/<name>.xml.
   * If the name has change (old_name is different from the current name),
   * a new file is created and the old one is wiped out.
   * @param oldName
   */
  void saveSurvey(std::string oldName);




private:
    std::string m_survey_name;
    std::string m_source_id_column;
    std::vector<FilterMapping> m_filters;
    std::string m_default_catalog;
    std::set<std::string> m_column_list;
    double m_non_detection=-99.;

    void ReadFilters();

};

}
}

#endif // SURVEYFILTERMAPPING_H
