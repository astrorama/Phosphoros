
#include "PhzQtUI/DatasetSelection.h"
#include "XYDataset/QualifiedName.h"
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

void DatasetSelection::setGroupes(std::vector<std::string> groupes) {
  m_groupes = std::move(groupes);
}

const std::vector<std::string>& DatasetSelection::getGroupes() const {
  return m_groupes;
}

void DatasetSelection::setIsolated(std::vector<std::string> isolated) {
  m_isolated = std::move(isolated);
}

const std::vector<std::string>& DatasetSelection::getIsolated() const {
  return m_isolated;
}

void DatasetSelection::setExclusions(std::vector<std::string> exclusions) {
  m_exclusions = std::move(exclusions);
}

const std::vector<std::string>& DatasetSelection::getExclusions() const {
  return m_exclusions;
}

bool DatasetSelection::isEmpty() const {
  return m_isolated.size() == 0 && m_groupes.size() == 0;
}

bool DatasetSelection::hasMultipleGroups() const {
  if (getIsolated().size() > 1) {
    return true;
  }

  if (getIsolated().size() == 1 && getGroupes().size() > 0) {
    return true;
  }

  std::vector<std::string> top_level_group{};
  for (auto& group : getGroupes()) {
    XYDataset::QualifiedName group_name(group);
    if (group_name.groups().size() == 0) {
      top_level_group.push_back(group);
    }
  }

  return top_level_group.size() > 1;
}

std::string DatasetSelection::getBaseGroupName() const {
  if (hasMultipleGroups()) {
    return "-Multiple-";
  }

  if (getIsolated().size() == 1) {
    return this->getIsolated()[0];
  }

  if (getGroupes().size() > 0) {
    return getGroupes()[0];
  }

  return "";
}

}  // namespace PhzQtUI
}  // namespace Euclid
