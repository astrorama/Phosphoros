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

typedef std::shared_ptr<
    PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>>DatasetRepo;

/**
 * @class ParameterRuleTable
 * @brief A tableView to display the ParameterRuleModel.
 */
class ParameterRuleTable: public QTableView {
public:
  ParameterRuleTable(QWidget*& parent);

  ParameterRuleTable(QDialog*&);

  /**
   * @brief Create a ParameterRuleModel by copying the 'init_parameter_rules'
   * and set it as the Table View model.
   * @param init_parameter_rules
   * @param sedRootPath
   * @param redRootPath
   */
  void loadParameterRules(std::map<int, ParameterRule> init_parameter_rules,
      DatasetRepo sed_repo, DatasetRepo red_curve_repo);

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
   * @brief push the redshift range to the selected ParameterRule
   * @param ranges The new vector of ranges
   */
  void setRedshiftRangesToSelectedRule(std::vector<Range> ranges);

  /**
   * @brief push the E(B-V) range to the selected ParameterRule
   * @param ranges The new vector of ranges
   */
  void setEbvRangesToSelectedRule(std::vector<Range> ranges);

  /**
   * @brief push the redshift values to the selected ParameterRule
   * @param values The new set of values
   */
  void setRedshiftValuesToSelectedRule(std::set<double> values);

  /**
   * @brief push the E(B_V) values to the selected ParameterRule
   * @param values The new set of values
   */
  void setEbvValuesToSelectedRule(std::set<double> values);

  /**
   * @brief push the SED list to the selected ParameterRule
   * @param state_selection The new selection of SED
   */
  void setSedsToSelectedRule(DatasetSelection state_selection);

  /**
   * @brief push the Reddening curves list to the selected ParameterRule
   * @param state_selection The new selection of Reddening curves
   */
  void setRedCurvesToSelectedRule(DatasetSelection state_selection);

  /**
   * @brief Get the Id of the selected ParameterRule
   * @return the rule Id
   */
  int getSelectedRuleId() const;

  /**
   * @brief Delete the currently selected ParameterRule.
   */
  void deletSelectedRule();

  /**
   * @brief Create a new ParameterRule and add it to the ParameterRuleModel.
   * If 'duplicate_selected' is true the new  object is prepopulated with
   * the value of the selected ParameterRule.
   * @param duplicate_selected
   */
  void newRule(bool duplicate_selected);

  /**
   * @brief Expose a ptr the Model of the TableView casted as a ParameterRuleModel
   * @return the ParameterRuleModel
   */
  ParameterRuleModel *getModel();

  /**
   * @brief Expose a constant ptr to the Model of the TableView casted as a
   * ParameterRuleModel
   * @return the ParameterRuleModel
   */
  const ParameterRuleModel *cGetModel() const;
};

}
}
#endif // PARAMETERRULETABLE_H
