#ifndef PHZQTUI_SEDGROUPMODEL_H
#define PHZQTUI_SEDGROUPMODEL_H

#include <QStandardItemModel>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

/**
 * @class SedGroupModel
 * @brief A Model to display items like SED in a TreeView keeping in an hidden
 * column the full name and allowing drag and drop.
 */
class SedGroupModel : public QStandardItemModel {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   */
  explicit SedGroupModel(QObject* parent = 0);

  /**
   * @brief Load the model with alist of items
   * @param items A vector of string representing the full name of the items
   */
  void load(std::vector<std::string> items);

  /**
   * @brief Get the list of SED in the model (drag and drop may have change it)
   * @return A vector of string representing the full name of the items
   */
  std::vector<std::string> getSeds() const;

  /**
   * @brief Drag and drop handling
   */
  Qt::DropActions supportedDropActions() const override;

  /**
   * @brief Drag and drop handling
   */
  QStringList mimeTypes() const override;

  /**
   * @brief Drag and drop handling
   */
  QMimeData* mimeData(const QModelIndexList& indexes) const override;

  /**
   * @brief Drag and drop handling
   */
  bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                    const QModelIndex& parent) override;

private:
  void                     addItems(std::string item);
  std::vector<QModelIndex> getAllChildren(QModelIndex parent) const;
};

}  // namespace PhzQtUI
}  // namespace Euclid
#endif  // PHZQTUI_SEDGROUPMODEL_H
