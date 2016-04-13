#ifndef PARAMETERRULEMODEL_H
#define PARAMETERRULEMODEL_H

#include <QStandardItemModel>
#include <vector>
#include <set>
#include "ParameterRule.h"

namespace Euclid {
namespace PhzQtUI {

/**
 * @brief The ParameterRuleModel class
 * This class provide a Model to be used in TableView.
 * It handle the ParameterRule.
 */
class ParameterRuleModel : public QStandardItemModel
{
public:
    /**
     * @brief Initialise the ParameterRuleModel by copying the 'init_parameter_rules'.
     * Root paths are used display relative path for the selected object rather than full absolute path
     * @param init_parameter_rules
     * @param sedRootPath
     * @param redRootPath
     */
    ParameterRuleModel(std::map<int,ParameterRule> init_parameter_rules, std::string sedRootPath, std::string redRootPath);

    /**
      * @briefcheck if the provided new name is already used for
      * another ParameterRule than the one in the row 'row'.
      * @param new_name
      * @row
      */
    bool checkNameAlreadyUsed(std::string new_name,int row) const;

    /**
     * @brief Returns the version of the ParameterRules handled by the ParameterRuleModel.
     * @return ParameterRules map
     */
    const std::map<int,ParameterRule>& getParameterRules() const;

    /**
     * @brief get the value of the item at a given row and column
     * @param row
     * @param column
     * @return the item value
     */
    const QString getValue(int row,int column) const;

    void setRedshiftRanges(std::vector<Range> z_ranges, int row);

    void setEbvRanges(std::vector<Range> ebv_ranges, int row);

    void setEbvValues(std::set<double> values,int row);

    void setRedshiftValues(std::set<double> values,int row);

    /**
     * @brief Push the name to the ParameterRule represented by the row 'row'.
     * @param new_name
     * @param row
     */
    void setName(std::string new_name,int row);

    /**
     * @brief Push the SED root object and SED excluded path to the ParameterRule represented by the row 'row'.
     * @param root
     * @param exceptions
     * @param row
     */
    void setSeds(std::string root, std::vector<std::string> exceptions,int row);

    /**
     * @brief Push the Reddenig Curve root object and Reddening Curve excluded path to the ParameterRule represented by the row 'row'.
     * @param root
     * @param exceptions
     * @param row
     */
    void setRedCurves(std::string root, std::vector<std::string> exceptions,int row);

    /**
     * @brief Create a new ParameterRule and add it to the ParameterRuleModel.
     * If 'duplicate_from_row' is true the new  object is prepopulated with the value of the one stored on row 'row'.
     * @param duplicate_from_row
     * @return the row number storing the new object
     */
    int newParameterRule(int duplicate_from_row );

    /**
     * @brief Delete the ParameterRule represented by the row 'row'.
     * @param row
     */
    void deletRule(int row);

    /**
     * @brief Get the ParameterRule represented by the row 'row'.
     * @param row
     * @return the ParameterRule.
     */
    const ParameterRule& getRule(int row) const;



private:

    std::string getParamName(const ParameterRule& rule) const;
    std::string getSedStatus(const ParameterRule& rule) const;
    std::string getSedGroupName(const ParameterRule& rule) const;
    std::string getRedStatus(const ParameterRule& rule) const;
    std::string getRedGroupName(const ParameterRule& rule) const;

    std::list<QString> getItemsRepresentation(ParameterRule& rule,int id) const;
    std::map<int,ParameterRule> m_parameter_rules;
    std::string m_sed_root_path;
    std::string m_red_root_path;
};

}
}

#endif // PARAMETERRULEMODEL_H
