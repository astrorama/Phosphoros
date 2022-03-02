#ifndef MODELSET_H
#define MODELSET_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <QDomDocument>
#include "PhzDataModel/PhzModel.h"
#include "ParameterRule.h"
#include "PhzQtUI/Range.h"


namespace boost{
namespace program_options{
 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {
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
    explicit ModelSet(std::string root_path);

    /**
     * @brief get Model Number
     * @return the cardinality of numbers which can be build from the ParameterRule(s)
     * of the ModelSet.
     */
    long long getModelNumber(bool recompute = false);


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
     * @brief set the Z ranges.
     * @param the Z ranges.
     */
    void setZRange(std::vector<Range> z_ranges);

    /**
     * @brief get the Z ranges.
     * @return the current Z ranges.
     */
    const std::vector<Range>& getZRanges() const;

    /**
     * @brief set the Z values.
     * @param the Z values.
     */
    void setZValues(std::set<double> z_values);

    /**
     * @brief get the Z values.
     * @return the current Z values.
     */
    const std::set<double>& getZValues() const;

    /**
     * @brief set the EBV ranges.
     * @param the EBV ranges.
     */
    void setEbvRange(std::vector<Range> ebv_ranges);

    /**
     * @brief get the EBV ranges.
     * @return the current  EBV ranges.
     */
    const std::vector<Range>& getEbvRanges() const;

    /**
     * @brief set the EBV values.
     * @param the EBV values.
     */
    void setEbvValues(std::set<double> ebv_values);

    /**
     * @brief get the EBV values.
     * @return the current EBV values.
     */
    const std::set<double>& getEbvValues() const;


    /**
     * @brief get the ParameterRules list
     * @return  a const ref on the ParameterRules list
     */
    std::map<int, ParameterRule> getParameterRules() const;

    /**
     * @brief set the ParameterRules by moving the provided object into the ModelSet
     * @param parameter_rules
     */
    void setParameterRules(std::map<int, ParameterRule> parameter_rules);

    /**
     * @brief Read from the disk all the ModelSet stored in a given folder.
     * @param root_path
     * The folder containing the persisted SurveyFilterMapping;
     * @return
     */
    static std::map<int, ModelSet> loadModelSetsFromFolder(std::string root_path);

    /**
     * @brief load a ModelSet from a xml file
     * @param fileName
     * @param root_path
     * @return the ModelSet.
     */
    static ModelSet loadModelSetFromFile(std::string file_name, std::string root_path);



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

    /**
     * @brief create a ModelSet out of its xml serialized version
     */
    static ModelSet deserialize(QDomDocument& doc, ModelSet& model);

    /**
     * create a xml serialized version of the ModelSet
     */
    QDomDocument serialize();

    /**
     * @brief compute the configuration (as a map<string,variable_values> ) corresponding to
     * this ModelSet
     *
     * @return the configuration map
     */
    std::map<std::string, boost::program_options::variable_value> getConfigOptions() const;


    std::map<std::string, boost::program_options::variable_value> getModelNameConfigOptions() const;

    /**
     * @brief compute the ModelAxeTuple corresponding to this ModelSet
     */
    std::map<std::string, PhzDataModel::ModelAxesTuple> getAxesTuple() const;

    std::vector<std::string> getSeds() const;

private:
    std::string m_name;
    std::map<int, ParameterRule> m_parameter_rules {};
    std::string m_root_path;
    std::vector<Range> m_z_ranges {};
    std::set<double> m_z_values {};
    std::vector<Range> m_ebv_ranges {};
    std::set<double> m_ebv_values {};
};

}
}
#endif // MODELSET_H
