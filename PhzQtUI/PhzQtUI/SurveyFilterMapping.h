#ifndef SURVEYFILTERMAPPING_H
#define SURVEYFILTERMAPPING_H
#include <list>
#include <map>
#include "FilterMapping.h"

namespace Euclid {
namespace PhosphorosUiDm {

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
      *  Constructor to be called when one want to persist the object.
      *
      * @param root_path
      * The folder into which the storing file will be created.
      */
    SurveyFilterMapping(std::string root_path);

    /**
      * @brief
      *  Get a concatenated list of the filters.
      *
      * @param separator
      * String to be inserted between the different filters names.
      *
      * @return a string listing the filters.
      */
    std::string getFilterNames(std::string separator) const;

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
    void setFilters(std::list<FilterMapping> filters);

    /**
     * @brief get the filters Filters
     * @return a const ref on the filter list
     */
    const std::list<FilterMapping>& getFilters() const;

    /**
     * @brief Read from the disk all the SurveyFilterMapping stored in a given folder.
     *
     * @param root_path
     * The folder containing the persisted SurveyFilterMapping;
     *
     * @return a map of SurveyFilterMapping indexed by a int key.
     */
    static std::map<int,SurveyFilterMapping> loadSurveysFromFolder(std::string root_path);

    /**
     * @brief load a SurveyFilterMapping from a xml file
     * @param fileName
     * @param root_path
     * @return the SurveyFilterMapping.
     */
    static SurveyFilterMapping loadSurveyFromFile(std::string fileName,std::string root_path);

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
    std::list<FilterMapping> m_filters;
    std::string m_root_path;

};

}
}

#endif // SURVEYFILTERMAPPING_H
