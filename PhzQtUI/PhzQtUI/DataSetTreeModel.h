#ifndef PHZQTUI_DATASETTREEMODEL_H
#define PHZQTUI_DATASETTREEMODEL_H

#include <QStandardItemModel>
#include "PhzQtUI/DatasetRepository.h"
#include "PhzQtUI/DatasetSelection.h"
#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

/**
 * @brief The DirectoryTreeModel class
 * This class provide a Model to be used in TreeView. It display a folder hierarchy
 * below a provided path and provide a set of possible behavior on user interaction.
 * Each item (or only file item) present a check box allowing the user to select it.
 */
class DataSetTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit DataSetTreeModel(DatasetRepo srepository,
                                QObject *parent = 0);


    void load();

    /**
     * @brief Turn the model Enabled(In edition: the user can check/uncheck elements)/Disabled(read-only)
     * @param enable
     */
    void setEnabled(bool enable);

    void setState(const DatasetSelection& selection);

    DatasetSelection getState() const;


public slots:
      /**
       * @brief SLOT onItemChanged To be connected to the ItemChanged SIGNAL,
       * Behavior -> Sub-tree check, allow exclusions:
       * When an unchecked folder is checked, check all its children (recursivelly)
       * When a checked folder is unchecked, uncheck all its children (recursivelly)
       * When an item which parent is not checked is checked uncheck all but it (and its children if any)
       */
      void onItemChanged(QStandardItem*);



private:

      static std::string getGroupName(XYDataset::QualifiedName qualified_name);

      void clearState();
      void checkGroup(XYDataset::QualifiedName name );
      void partialCheckParentGroups(XYDataset::QualifiedName child );

      void setEditionStatus(bool inEdition);

      bool m_in_edition = false;
      std::map<std::string,QStandardItem*> m_map_dir;
      DatasetRepo m_repository;

};


}
}
#endif // PHZQTUI_DATASETTREEMODEL_H
