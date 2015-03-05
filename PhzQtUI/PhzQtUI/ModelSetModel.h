#ifndef MODELSETMODEL_H
#define MODELSETMODEL_H

#include <QStandardItemModel>

#include "ModelSet.h"

namespace Euclid {
namespace PhzQtUI {

/**
 * @brief The ModelSetModel class
 * This class provide a Model to be used in TableView.
 * It handle the ModelSet.
 */
class ModelSetModel : public QStandardItemModel
{

public:
    ModelSetModel();
    virtual ~ModelSetModel()=default;

    /**
     * @brief Initialise the ModelSetModel by loading the ModelSet out of the files
     * present in the 'path' directory.
     * @param path
     */
    void loadSets(const std::string& path);

    /**
     * @brief Create a new ModelSet and add it to the ModelSetModel. If the 'duplicate_from_row'
     * is a valid row, the data of the new set are copied from the one at this row.
     * @param duplicate_from_row
     * @return the row of the model which contains the new ModelSet.
     */
    int newSet(int duplicate_from_row );

    /**
     * @brief Delete the ModelSet represented by the row 'row' of the model.
     * If 'deleteFile' is true also delete the file into which the ModelSet was persisted.
     * @param row
     * @param deletFile
     */
    void deleteSet(int row, bool deletFile);

    /**
     * @brief Write the ModelSet represented by the row 'row' into its file.
     * If the name of the ModelSet has changed also remove the old file.
     * @param row
     * @param oldName
     */
    void saveSet(int row,std::string oldName);

    /**
     * @brief Check that the 'new_name' do not conflict with other existing ModelSet name.
     * @param new_name
     * @param id
     * @return true if the name can be used
     */
    bool checkUniqueName(QString new_name, int id) const;



    /**
     * @brief Get the Name of the ModelSet represented by the row 'row'
     * @param row
     * @return the name
     */
    const QString getName(int row) const;

    /**
     * @brief Set the Name of the ModelSet represented by the row 'row'.
     * The new name ('value') is checked to not conflict with other names.
     * @param row
     * @param value
     * the new name
     * @return true if the name has been set, false if the name conflict and cannot be used.
     */
    bool setName(int row, const QString& value);

    /**
     * @brief Recompute the number of model inside the set at row 'row' and refresh the displayed value.
     * @param row
     */
    void updateModelCount(int row);

    /**
     * @brief Get the map of ParameterRules of the ModelSet represented by the row 'row'.
     * @param row
     * @return the map of ParameterRules
     */
    const std::map<int,ParameterRule> getParameterRules(int row) const;

    /**
     * @brief Set the map of ParameterRules to the ModelSet represented by the row 'row'.
     * @param row
     * @param value
     */
    void setParameterRules(int row, const std::map<int,ParameterRule>& value);

    /**
     * @brief get the RootPath
     * @return the Root path
     */
    const std::string getRootPath() const;

private:
    const QString getValue(int row, int column) const;
    const QString getNumber(int row) const;
    const QString getRef(int row) const;
    void setValue(int row, int column,const QString& value);
    std::map<int,ModelSet> m_set_list;
    std::string m_root_path;


    std::string getDuplicateName(std::string name) const;


};

}
}
#endif // MODELSETMODEL_H
