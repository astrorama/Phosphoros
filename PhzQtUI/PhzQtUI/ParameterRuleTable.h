#ifndef PARAMETERRULETABLE_H
#define PARAMETERRULETABLE_H

#include <QTableView>
#include <set>
#include <vector>
#include "ParameterRule.h"
#include "ParameterRuleModel.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

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
       void loadParameterRules(std::map<int,ParameterRule> init_parameter_rules, DatasetRepo sed_repo, DatasetRepo red_curve_repo);

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

       void setRedshiftRangesToSelectedRule(std::vector<Range> ranges);

       void setEbvRangesToSelectedRule(std::vector<Range> ranges);

       void setEbvValuesToSelectedRule(std::set<double> values);

       void setRedshiftValuesToSelectedRule(std::set<double> values);



       void setSedsToSelectedRule(DatasetSelection state_selection);

       void setRedCurvesToSelectedRule(DatasetSelection state_selection);

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
