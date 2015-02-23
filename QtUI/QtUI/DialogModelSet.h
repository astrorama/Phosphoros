#ifndef DIALOGMODELSET_H
#define DIALOGMODELSET_H

#include <map>
#include <QDialog>
#include <QItemSelection>

#include "ParameterRule.h"

namespace Ui {
class DialogModelSet;
}

/**
 * @brief The DialogModelSet class
 * This popup allows the user to view/edit the Parameter Rules of a Model Set.
 */
class DialogModelSet : public QDialog
{
    Q_OBJECT

public:
    explicit DialogModelSet(QWidget *parent = 0);
    ~DialogModelSet();

    /**
     * @brief Initialise the popup by setting its internal data
     * @param init_parameter_rules
     * the map of ParameterRules to be displayed/modified
     */
    void loadData(const std::map<int,Euclid::PhosphorosUiDm::ParameterRule>& init_parameter_rules);

    /**
     * @brief When called, set the popup in read-only.
     */
    void setViewMode();

    /**
     * @brief When called, do not allow the user to enter more than one single ParameterRule.
     */
    void setSingleLine();

signals:
    /**
      * @brief SIGNAL popupClosing: rised when the the popup close.
      * The argument is the modifed map of ParameterRules.
      */
     void popupClosing(std::map<int,Euclid::PhosphorosUiDm::ParameterRule>);

private slots:
     /**
      * @brief SLOT selectionChanged: the user has change the master grid line selection,
      * update the detail pannels and action buttons accordingly.
      */
     void selectionChanged(QModelIndex, QModelIndex);

     /**
      * @brief SLOT on_buttonBox_rejected: The user close the popup, build the new ParameterRules map
      * and rise the SIGNAL popupClosing
      */
     void on_buttonBox_rejected();

     /**
      * @brief SLOT on_btn_new_clicked: create a new ParameterRule
      */
     void on_btn_new_clicked();

     /**
      * @brief SLOT on_btn_delete_clicked: delete the selected ParameterRule
      */
     void on_btn_delete_clicked();

     /**
      * @brief SLOT on_btn_duplicate_clicked: duplicate the selected ParameterRule
      */
     void on_btn_duplicate_clicked();

     /**
      * @brief SLOT on_btn_edit_clicked: switch the detail pannels in edition.
      */
     void on_btn_edit_clicked();

     /**
      * @brief SLOT on_btn_cancel_clicked: refresh the detail pannels whith old value and switch them in read-only.
      */
     void on_btn_cancel_clicked();

     /**
      * @brief SLOT on_btn_save_clicked: update the selected ParameterRule with the values of the detail pannels.
      */
     void on_btn_save_clicked();

private:
    Ui::DialogModelSet *ui;
    bool m_insert=false;
    bool m_view_popup=false;
    bool m_singe_line=false;

    void turnControlsInEdition();
    void turnControlsInView();
};

#endif // DIALOGMODELSET_H
