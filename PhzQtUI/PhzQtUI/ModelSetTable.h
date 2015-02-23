#ifndef MODELSETTABLE_H
#define MODELSETTABLE_H


#include <QTableView>
#include "ModelSetModel.h"
#include "ParameterRule.h"


/**
 * @brief The ModelSetTable class
 * A tableView to display the ModelSetModel.
 */
class ModelSetTable : public QTableView
{

public:
    ModelSetTable(QWidget*& parent);

    /**
     * @brief Create a ModelSetModel (by loading the ModelSet persisted
     * into the 'path' directory) and use it as the TableView Model
     * @param path
     */
    void loadFromPath(std::string path);

    /**
     * @brief Check if a row is currently selected.
     * @return true if a row is selected
     */
    bool hasSelectedSet() const;


    /**
     * @brief Get the name of the currently selected ModelSet
     * @return the Model Set Name
     */
    QString getSelectedName();

    /**
     * @brief Set the name of the currently selected ModelSet. If the name is already used,
     * the operation is canceled and the call returns false
     * @param new_name
     * @return true if the name is not conflicting with another name and can be used.
     */
    bool setSelectedName(QString new_name);

    /**
     * @brief Get the map of ParameterRules of the selected ModelSet.
     * @return the map of ParameterRules
     */
    std::map<int,Euclid::PhosphorosUiDm::ParameterRule> getSelectedParameterRules();

    /**
     * @brief  Set the map of ParameterRules to the selected ModelSet.
     * @param new_value
     */
    void setSelectedRules(const std::map<int,Euclid::PhosphorosUiDm::ParameterRule>& new_value);

    /**
     * @brief Recompute the number of model inside the selected ModelSet and refresh the view.
     */
    void updateModelNumberForSelected();

    /**
     * @brief Delete the selected ModelSet. Also delete the corresponding file if 'deletFile' is true.
     * @param deletFile
     */
    void deleteSelectedSet(bool deletFile);

    /**
     * @brief Write the selected ModelSet on the disk. The old name has to be provided to remove the old file.
     * @param oldName
     */
    void saveSelectedSet(std::string oldName);

    /**
     * @brief Create a new ModelSet and add it to the view. If the 'duplicate_selected' is true,
     * the new ModelSet is pre-populated with the value of the selected one.
     * @param duplicate_selected
     */
    void newSet(bool duplicate_selected);


    ModelSetModel* getModel();

private:



};


#endif // MODELSETTABLE_H
