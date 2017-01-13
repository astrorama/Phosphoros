#ifndef PARAMETERRULE_H
#define PARAMETERRULE_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "Range.h"
#include "DatasetSelection.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

namespace boost {
namespace program_options {
class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<
    PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>>DatasetRepo;

/**
 * @class ParameterRule
 *
 * @brief
 *  Object storing the description of a ParameterRule.
 *
 */
class ParameterRule {
public:
  /**
   * @brief ParameterRule
   */
  ParameterRule();

  explicit ParameterRule(long long model_number);

  /**
   * @brief Get the name of the ParameterRule
   * @return the name of the ParameterRule
   */
  std::string getName() const;

  /**
   * @brief Set the name of the ParameterRule
   * @param new_name the new name of the ParameterRule
   */
  void setName(std::string new_name);
  /**
   * @brief get Reddening Curves Number
   * @return the cardinality of selected Reddening Curves in the the ParameterRule
   */
  std::pair<long, long> getRedCurveNumber(
      DatasetRepo redenig_curves_repository) const;

  std::string getRedCurveGroupName() const;

  /**
   * @brief get SED Number
   * @return the cardinality of selected SED in the the ParameterRule
   */
  std::pair<long, long> getSedNumber(DatasetRepo sed_repository) const;

  std::string getSedGroupName() const;

  /**
   * @brief get Model Number
   * @return the cardinality of models which can be build from the ParameterRule
   */
  long long getModelNumber(bool recompute = false);

  /**
   * @brief Get a string representing the E(B-V) range
   * @return The range as a string
   */
  const std::string getEbvRangeString() const;

  /**
   * @brief Get a string representing the Redshift range
   * @return The range as a string
   */
  const std::string getRedshiftRangeString() const;

  /**
   * @brief Get a string representing the E(B-V) values as a list
   * @return The list as a string
   */
  std::string getEbvStringValueList() const;

  /**
   * @brief Get a string representing the redshift values as a list
   * @return The list as a string
   */
  std::string getRedshiftStringValueList() const;

  /**
   * @brief Get the E(B-V) values
   */
  const std::set<double>& getEbvValues() const;

  void setEbvValues(std::set<double> values);

  const std::set<double>& getRedshiftValues() const;

  void setRedshiftValues(std::set<double> values);

  /**
   * @brief get the E(B-V) Ranges.
   * @return the E(B-V) Ranges.
   */
  const std::vector<Range>& getEbvRanges() const;

  /**
   * @brief set the E(B-V) Ranges by moving in the provided ranges
   * @param ebv_ranges
   */
  void setEbvRanges(std::vector<Range> ebv_ranges);

  /**
   * @brief get the redshift Ranges.
   * @return the redshift Ranges.
   */
  const std::vector<Range>& getZRanges() const;

  /**
   * @brief set the redshift Ranges by moving in the provided ranges
   * @param z_ranges
   */
  void setZRanges(std::vector<Range> z_ranges);

  std::map<std::string, boost::program_options::variable_value> getConfigOptions(
      std::string region) const;

  void setRedCurveSelection(DatasetSelection red_curve_selection);
  void setSedSelection(DatasetSelection sed_selection);

  const DatasetSelection& getRedCurveSelection() const;
  const DatasetSelection& getSedSelection() const;

  static std::set<double> parseValueList(const std::string& list);

private:
  std::pair<long, long> getSelectionNumbers(DatasetSelection selection,
      DatasetRepo repository) const;
  std::string getAxisStringValue(std::vector<double> axis) const;
  std::string getStringValueList(const std::set<double>& list) const;

  std::string m_name;

  std::set<double> m_ebv_values { };
  std::set<double> m_redshift_values { };

  std::vector<Range> m_ebv_ranges { };
  std::vector<Range> m_redshift_ranges { };

  long long m_model_number = -1;

  DatasetSelection m_red_curve_selection;
  DatasetSelection m_sed_selection;
};

}
}
#endif // PARAMETERRULE_H
