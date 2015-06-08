#ifndef PARAMETERRULETABLE_H
#define PARAMETERRULETABLE_H

#include <QTableView>
#include "ParameterRule.h"
#include "ParameterRuleModel.h"

namespace Euclid {
namespace PhzQtUI {


/**
 * @brief The ParameterRuleTable class
 *  A tableView to display the ParameterRuleModel.
 */
class ParameterRuleTable : public QTableView
{
public:
       ParameterRuleTable(QWidget*& parent);

       ParameterRuleTable(QDialog*& );

       /**
        * @brief Create a ParameterRuleModel by copying the 'init_parameter_rules' and set it as the Table View model.
        * @param init_parameter_rules
        * @param sedRootPath
        * @param redRootPath
        */
       void loadParameterRules(std::map<int,ParameterRule> init_parameter_rules, std::string sedRootPath ,std::string redRootPath);

       /**
        * @brief Check if there is a row currently selected.
        * @return true if a row is selected.
        */
       bool hasSelectedPArameterRule();

       /**
        * @brief Get the currently selected ParameterRule
        * @return the selected ParameterRule.
        */
       const ParameterRule& getSelectedRule() const;

       /**
        * @briefcheck if the provided new name is already used for
        * another ParameterRule than the selected one.
        * @param new_name
        */
       bool checkNameAlreadyUsed(std::string new_name) const;
       /**
        * @brief push the name to the selected ParameterRule
        * @param new_name the new name
        */
       void setNameToSelectedRule(std::string new_name);
       /**
        * @brief Push the E(B-V) and Redshift range to the selected ParameterRule.
        * @param ebvRange
        * @param zRange
        */
       void setRangesToSelectedRule(Range ebvRange, Range zRange);

       /**
        * @brief Push the SED root object and SED excluded path to the selected ParameterRule.
        * @param root
        * @param exceptions
        */
       void setSedsToSelectedRule(std::string root, std::vector<std::string> exceptions);

       /**
        * @brief Push the Reddenig Curve root object and Reddening Curve excluded path to the selected ParameterRule.
        * @param root
        * @param exceptions
        */
       void setRedCurvesToSelectedRule(std::string root, std::vector<std::string> exceptions);

       /**
        * @brief Delete the currently selected ParameterRule.
        */
       void deletSelectedRule();

       /**
        * @brief Create a new ParameterRule and add it to the ParameterRuleModel.
        * If 'duplicate_selected' is true the new  object is prepopulated with the value of the selected ParameterRule.
        * @param duplicate_selected
        */
       void newRule(bool duplicate_selected);

       /**
        * @brief Expose a ptr the Model of the TableView casted as a ParameterRuleModel
        * @return the ParameterRuleModel
        */
       ParameterRuleModel *getModel();

       /**
        * @brief Expose a constant ptr to the Model of the TableView casted as a ParameterRuleModel
        * @return the ParameterRuleModel
        */
       const ParameterRuleModel *cGetModel() const;
};

}
}
#endif // PARAMETERRULETABLE_H
