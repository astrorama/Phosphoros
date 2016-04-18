#ifndef PHZQTUI_DATASETSELECTION
#define PHZQTUI_DATASETSELECTION

#include <vector>
#include <string>

namespace Euclid {
namespace PhzQtUI {

class DatasetSelection
{
public:
  void setGroupes(std::vector<std::string> groupes);
  const std::vector<std::string>& getGroupes() const;

  void setIsolated(std::vector<std::string> isolated);
  const std::vector<std::string>& getIsolated() const;

  void setExclusions(std::vector<std::string> exclusions);
  const std::vector<std::string>& getExclusions() const;

  bool isEmpty() const;

  bool hasMultipleGroups() const;

  std::string getBaseGroupName() const;
private:
  std::vector<std::string> m_groupes;
  std::vector<std::string> m_isolated;
  std::vector<std::string> m_exclusions;

};

}
}

#endif // PHZQTUI_DATASETSELECTION
