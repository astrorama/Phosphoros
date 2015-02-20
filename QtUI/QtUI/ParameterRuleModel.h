#ifndef PARAMETERRULEMODEL_H
#define PARAMETERRULEMODEL_H

#include <QStandardItemModel>
#include "ParameterRule.h"

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
    ParameterRuleModel(std::map<int,PhosphorosUiDm::ParameterRule> init_parameter_rules, std::string sedRootPath, std::string redRootPath);

    /**
     * @brief Returns the version of the ParameterRules handled by the ParameterRuleModel.
     * @return ParameterRules map
     */
    const std::map<int,PhosphorosUiDm::ParameterRule>& getParameterRules() const;

    /**
     * @brief get the value of the item at a given row and column
     * @param row
     * @param column
     * @return the item value
     */
    const QString getValue(int row,int column) const;

    /**
     * @brief Push the E(B-V) and Redshift range to the ParameterRule represented by the row 'row'.
     * @param ebvRange
     * @param zRange
     * @param row
     */
    void setRanges(PhosphorosUiDm::Range ebvRange,PhosphorosUiDm::Range zRange,int row);

    /**
     * @brief Push the SED root object and SED excluded path to the ParameterRule represented by the row 'row'.
     * @param root
     * @param exceptions
     * @param row
     */
    void setSeds(std::string root, std::list<std::string> exceptions,int row);

    /**
     * @brief Push the Reddenig Curve root object and Reddening Curve excluded path to the ParameterRule represented by the row 'row'.
     * @param root
     * @param exceptions
     * @param row
     */
    void setRedCurves(std::string root, std::list<std::string> exceptions,int row);

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
    const PhosphorosUiDm::ParameterRule& getRule(int row) const;

private:
    std::list<QString> getItemsRepresentation(const PhosphorosUiDm::ParameterRule& rule,int id) const;
    std::map<int,PhosphorosUiDm::ParameterRule> m_parameter_rules;
    std::string m_sed_root_path;
    std::string m_red_root_path;
};

#endif // PARAMETERRULEMODEL_H
