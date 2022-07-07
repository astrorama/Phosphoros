#ifndef PHZQTUI_DATASETSELECTION
#define PHZQTUI_DATASETSELECTION

#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {
/**
 * @class DatasetSelection
 * @brief This class store the selection state on a QualifiedName data structure.
 * It list the Groups containing selected items, the item belonging to selected
 * groups but not selected itself and the selected items which do not belong
 * to any group.
 */
class DatasetSelection {
public:
  /**
   * @brief Set the selected groups
   * @param groups A vector of string containing the (full) name of the groups.
   */
  void setGroupes(std::vector<std::string> groups);

  /**
   * @brief Get the selected groups
   * @return A vector of string containing the (full) name of the groups.
   */
  const std::vector<std::string>& getGroupes() const;

  /**
   * @brief Set the selected items not belonging to any group
   * @param isolated A vector of string containing the (full) name of the items.
   */
  void setIsolated(std::vector<std::string> isolated);

  /**
   * @brief Get the selected items not belonging to any group
   * @return A vector of string containing the (full) name of the items.
   */
  const std::vector<std::string>& getIsolated() const;

  /**
   * @brief Set the item not selected but belonging to a selected group
   * @param exclusions A vector of string containing the (full) name of the items.
   */
  void setExclusions(std::vector<std::string> exclusions);

  /**
   * @brief Get the item not selected but belonging to a selected group
   * @return A vector of string containing the (full) name of the items.
   */
  const std::vector<std::string>& getExclusions() const;

  /**
   * @brief Check if the selection contains at least one item.
   * @return true is the selection is empty.
   */
  bool isEmpty() const;

  /**
   * @brief Check if the selection belong to multiple first level groups or
   * contains groups and isolated item or multiple isolated items.
   * @return true if the selection is not restricted to a single group or
   * contains more than a single item out of the groups
   */
  bool hasMultipleGroups() const;

  std::string getBaseGroupName() const;

private:
  std::vector<std::string> m_groupes;
  std::vector<std::string> m_isolated;
  std::vector<std::string> m_exclusions;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // PHZQTUI_DATASETSELECTION
