#ifndef PHZQTUI_SEDTREEMODEL_H
#define PHZQTUI_SEDTREEMODEL_H

#include "PhzQtUI/DatasetRepository.h"
#include "PhzQtUI/DatasetSelection.h"
#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/QualifiedName.h"
#include <QStandardItemModel>
#include <QString>

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

/**
 * @class DataSetTreeModel
 * @brief This class provide a Model to associate the data structure returned
 * by a DatasetRepository<YDataset::FileSystemProvider> to a TreeView.
 * It display a group hierarchy with no root items and allow multiple items
 * selection. The selection state is set and retrieve using a DatasetSelection
 * object.
 */
class SedTreeModel : public QStandardItemModel {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   * @param repository A pointer on a
   * DatasetRepository<YDataset::FileSystemProvider> containing the data
   * structure to be displayed
   * @param parent the parent QObject
   */
  explicit SedTreeModel(DatasetRepo repository, QObject* parent = 0);

  /**
   * @brief Read the provided repository and create the items needed to
   * represent it. To be called once.
   */
  void load(bool selectable = true, bool onlyLeaves = false);

  /**
   * @brief Turn the model Enabled (In edition: the user can check/uncheck
   * elements)/Disabled(read-only)
   * @param enable
   */
  void setEnabled(bool enable);

  /**
   * @brief Set the selection state
   * @param selection A DatasetSelection object encoding the selection state.
   */
  void setState(const DatasetSelection& selection);

  void setState(const std::vector<std::string>& selected_leaves);

  /**
   * @brief Get the selection state
   * @return A DatasetSelection object encoding the selection state.
   */
  DatasetSelection getState() const;

  std::vector<std::string> getSelectedLeaves() const;

  bool hasLeave() const;

  bool    canOpenInfo(QStandardItem* item) const;
  bool    canAddEmissionLineToGroup(QStandardItem* item) const;
  bool    canAddLpEmissionLineToGroup(QStandardItem* item) const;
  QString getFullGroupName(QStandardItem* item) const;

public slots:
  /**
   * @brief SLOT onItemChanged To be connected to the ItemChanged SIGNAL.
   */
  void onItemChanged(QStandardItem*);
  void onItemChangedSingleLeaf(QStandardItem*);

private:
  /**
   * @brief Get the name of the enclosing group
   * @param qualified_name The name of the item.
   */
  static std::string getGroupName(XYDataset::QualifiedName qualified_name);

  /**
   * @brief Uncheck all items.
   */
  void clearState();

  /**
   * @brief Check a group and its children
   * @param name the name of the group.
   */
  void checkGroup(XYDataset::QualifiedName name);

  /**
   * @brief look for a group which is the direct parent of the provided child,
   * mark it as partially checked
   * @param child The item/group name of wich the parent has to be checked
   */
  void partialCheckParentGroups(XYDataset::QualifiedName child);

  /**
   * @brief Set the edition flag of the class
   * @param inEdition the new edition status.
   */
  void setEditionStatus(bool inEdition);

  bool                                  m_in_edition = false;
  bool                                  m_bypass     = false;
  std::map<std::string, QStandardItem*> m_map_dir;
  DatasetRepo                           m_repository;
};

}  // namespace PhzQtUI
}  // namespace Euclid
#endif  // PHZQTUI_SEDTREEMODEL_H
