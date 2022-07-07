#ifndef PARAMETERRULEMODEL_H
#define PARAMETERRULEMODEL_H

#include "ParameterRule.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include <QStandardItemModel>
#include <set>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

/**
 * @class ParameterRuleModel
 * @brief This class provide a Model to be used in TableView.
 * It handle the ParameterRule.
 */
class ParameterRuleModel : public QStandardItemModel {
public:
  /**
   * @brief Initialise the ParameterRuleModel by copying the 'init_parameter_rules'.
   * Root paths are used display relative path for the selected object rather than full absolute path
   * @param init_parameter_rules
   * @param sedRootPath
   * @param redRootPath
   */
  ParameterRuleModel(std::map<int, ParameterRule> init_parameter_rules, DatasetRepo sed_repo,
                     DatasetRepo red_curve_repo);

  /**
   * @briefcheck if the provided new name is already used for
   * another ParameterRule than the one in the row 'row'.
   * @param new_name
   * @row
   */
  bool checkNameAlreadyUsed(std::string new_name, int row) const;

  /**
   * @brief Returns the version of the ParameterRules handled by the ParameterRuleModel.
   * @return ParameterRules map
   */
  const std::map<int, ParameterRule>& getParameterRules() const;

  /**
   * @brief get the value of the item at a given row and column
   * @param row
   * @param column
   * @return the item value
   */
  const QString getValue(int row, int column) const;

  /**
   * @brief Set the redshift range to the rules stored in the provided row
   * @param z_ranges The vector of redshift ranges
   * @param row The index of the row storing the rule to be updated
   */
  void setRedshiftRanges(std::vector<Range> z_ranges, int row);

  /**
   * @brief Set the E(B_V) range to the rules stored in the provided row
   * @param ebv_ranges The vector of redshift ranges
   * @param row The index of the row storing the rule to be updated
   */
  void setEbvRanges(std::vector<Range> ebv_ranges, int row);

  /**
   * @brief Set the Redshift set of values to the rules stored in the provided row
   * @param values The set of value
   * @param row The index of the row storing the rule to be updated
   */
  void setRedshiftValues(std::set<double> values, int row);

  /**
   * @brief Set the E(B-V) set of values to the rules stored in the provided row
   * @param values The set of value
   * @param row The index of the row storing the rule to be updated
   */
  void setEbvValues(std::set<double> values, int row);

  /**
   * @brief Push the name to the ParameterRule represented by the row 'row'.
   * @param new_name
   * @param row The index of the row storing the rule to be updated
   */
  void setName(std::string new_name, int row);

  /**
   * @brief Set the SED selection to the rules stored in the provided row
   * @param state_selection The SED selection state
   * @param row The index of the row storing the rule to be updated
   */
  void setSeds(DatasetSelection state_selection, int row);

  /**
   * @brief Set the Reddening curve selection to the rules stored in the provided row
   * @param state_selection The reddening curve selection state
   * @param row The index of the row storing the rule to be updated
   */
  void setRedCurves(DatasetSelection state_selection, int row);

  /**
   * @brief Create a new ParameterRule and add it to the ParameterRuleModel.
   * If 'duplicate_from_row' is true the new  object is prepopulated with the
   * value of the one stored on row 'row'.
   * @param duplicate_from_row
   * @return the row number storing the new object
   */
  int newParameterRule(int duplicate_from_row);

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

  std::list<QString>           getItemsRepresentation(ParameterRule& rule, int id) const;
  std::map<int, ParameterRule> m_parameter_rules;
  DatasetRepo                  m_sed_repo;
  DatasetRepo                  m_red_curve_repo;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // PARAMETERRULEMODEL_H
