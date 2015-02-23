#ifndef MODELSET_H
#define MODELSET_H

#include <string>
#include <map>
#include "ParameterRule.h"

namespace Euclid {
namespace PhosphorosUiDm {
/**
 * @brief The ModelSet class
 */
class ModelSet
{
public:
    /**
      * @brief
      *  The default constructor is only here to allow the usage of ModelSet in lists.
      */
    ModelSet();

    /**
      * @brief
      *  Constructor to be called when one want to persist the object.
      *
      * @param root_path
      * The folder into which the storing file will be created.
      */
    ModelSet(std::string root_path);

    /**
     * @brief get Model Number
     * @return the cardinality of numbers which can be build from the ParameterRule(s)
     * of the ModelSet.
     */
    long long getModelNumber() const;

    /**
     * @brief get the Name.
     * @return the current name.
     */
    std::string getName() const;

    /**
     * @brief set the Name (which is also the file name).
     * @param name
     */
    void setName(std::string name);

    /**
     * @brief get the ParameterRules list
     * @return  a const ref on the ParameterRules list
     */
    std::map<int,ParameterRule> getParameterRules() const;

    /**
     * @brief set the ParameterRules by moving the provided object into the ModelSet
     * @param parameter_rules
     */
    void setParameterRules( std::map<int,ParameterRule> parameter_rules);

    /**
     * @brief Read from the disk all the ModelSet stored in a given folder.
     * @param root_path
     * The folder containing the persisted SurveyFilterMapping;
     * @return
     */
    static std::map<int,ModelSet> loadModelSetsFromFolder(std::string root_path);

    /**
     * @brief load a ModelSet from a xml file
     * @param fileName
     * @param root_path
     * @return the ModelSet.
     */
    static ModelSet loadModelSetFromFile(std::string file_name,std::string root_path);

    /**
     * @brief Delete the current ModelSet by erasing the file it was persisted in.
     */
    void deleteModelSet();

    /**
     * @brief Write the ModelSet in a xml file which path is <root_path>/<name>.xml.
     * If the name has change (old_name is different from the current name),
     * a new file is created and the old one is wiped out.
     * @param oldName
     */
    void saveModelSet(std::string old_name);

private:
    std::string m_name;
    std::map<int,ParameterRule> m_parameter_rules {};
    std::string m_root_path;
};

}
}
#endif // MODELSET_H
