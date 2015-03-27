#ifndef DIRECTORYTREEMODEL_H
#define DIRECTORYTREEMODEL_H

#include <QStandardItemModel>

using namespace std;

namespace Euclid {
namespace PhzQtUI {


/**
 * @brief The DirectoryTreeModel class
 * This class provide a Model to be used in TreeView. It display a folder hierarchy
 * below a provided path and provide a set of possible behavior on user interaction.
 * Each item (or only file item) present a check box allowing the user to select it.
 */
class DirectoryTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit DirectoryTreeModel(QObject *parent = 0);

    /**
     * @brief Initialise the DirectoryTreeModel by setting its internal data
     * @param rootPath
     * The path of the top folder to be displayed by the model
     * @param singleLeafSelection
     * If true allows only selection of leaf (file) otherwise allows also node (folder) selection.
     * @param rootDisplayName
     * Alias for the root folder to be used as displayed value.
     */
    void loadDirectory(string rootPath, bool singleLeafSelection, string rootDisplayName=".");

    /**
     * @brief Turn the model Enabled(In edition: the user can check/uncheck elements)/Disabled(read-only)
     * @param enable
     */
    void setEnabled(bool enable);

    /**
     * @brief Set the checkboxes states according to the provided arguments.
     * @param root
     * check the 'root' element and all its children (if any)
     * @param exclusions
     * prevent the listed element to be checked, this allow for selecting a folder but some of its sub-elements.
     */
    void setState(string root, const vector<string>& exclusions);

    /**
     * @brief Programatically check the root element of the model.
     */
    void selectRoot();

    /**
     * @brief Get the first element to be checked starting from the the given path 'from'
     * and cascading to the children if none at this level.
     *
     * @param from (by default 'root')
     * @return
     */
    pair<bool,string> getRootSelection(string from=".") const;

    /**
     * @brief get all the leaves which are not checked under a given 'root'
     * @param root
     * @return
     */
    vector<string> getExclusions(string root) const;

    /**
     * @brief get the current element if it represent a folder or the parent element if the current is a file.
     * @return
     */
    string getGroup() const;

    /**
     * @brief If the 'path' is within the current 'root_dir' return the path relative to the root.
     * @param path
     * @return the relative path
     */
    string getRelPath(string path,string root_alias = "") const;

    /**
     * @brief Complete the relative 'path' with the root dir in order to obtain an absolute path
     * @param path
     * @return the absolute path
     */
    string getFullPath(string path) const;

public slots:
      /**
       * @brief SLOT onItemChanged To be connected to the ItemChanged SIGNAL,
       * Behavior -> Sub-tree check, allow exclusions:
       * When an unchecked folder is checked, check all its children (recursivelly)
       * When a checked folder is unchecked, uncheck all its children (recursivelly)
       * When an item which parent is not checked is checked uncheck all but it (and its children if any)
       */
      void onItemChanged(QStandardItem*);

      /**
       * @brief SLOT onItemChangedSingleLeaf To be connected to the ItemChanged SIGNAL,
       * Behavior -> Only no check or a single item checked:
       * When an item is checked uncheck the currently checked item.
       */
      void onItemChangedSingleLeaf(QStandardItem*);

      /**
       * @brief SLOT onItemChangedUniqueSelection: To be connected to the ItemChanged SIGNAL,
       * Behavior -> Allways a single item checked:
       * When an item is checked uncheck the currently checked item, when an item is unchecked, check the root item.
       */
      void onItemChangedUniqueSelection(QStandardItem*);

private:
      void setEditionStatus(bool inEdition);
      void checkDir(bool checked,string dir, vector<string> exclusions={});
      bool m_in_edition = false;
      bool m_bypass_item_changed =false;
      map<string,QStandardItem*> m_map_dir;
      string m_root_dir;


};

}
}

#endif // DIRECTORYTREEMODEL_H
