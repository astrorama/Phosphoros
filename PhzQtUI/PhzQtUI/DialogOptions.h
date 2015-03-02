#ifndef DIALOGOPTIONS_H
#define DIALOGOPTIONS_H

#include <QDialog>

namespace Euclid {
namespace PhzQtUI {



namespace Ui {
class DialogOptions;
}

/**
 * @brief The DialogOptions class
 * This popup allows the user to Navigate through the options
 * and change the root path.
 */
class DialogOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOptions(QWidget *parent = 0);
    ~DialogOptions();

signals:
    /**
     * @brief SIGNAL goToFilterManagement: rised when the poupu is closing and the next
     * page has to be the Survey Filter Mapping page
     */
    void goToFilterManagement();

    /**
     * @brief SIGNAL goToAuxDataManagement: rised when the popup is closed and the next
     * page has to be the Aux. Data Management page
     */
    void goToAuxDataManagement();

private slots:
    /**
     * @brief SLOT on_btn_ManageFilter_clicked: rise the SIGNAL goToFilterManagement
     * and close the popup
     */
    void on_btn_ManageFilter_clicked();

    /**
     * @brief SLOT on_btn_ManageAuxData_clicked: rise the SIGNAL goToAuxDataManagement
     * and close the popup
     */
    void on_btn_ManageAuxData_clicked();

    /**
     * @brief SLOT on_btn_editGeneral_clicked: turn the root path section in edition.
     */
    void on_btn_editGeneral_clicked();

    /**
     * @brief SLOT on_btn_cancelGeneral_clicked: cancel the root path section edition.
     */
    void on_btn_cancelGeneral_clicked();

    /**
     * @brief SLOT on_btn_saveGeneral_clicked: ends the edition and save the root path into the
     * resources file.
     */
    void on_btn_saveGeneral_clicked();

    /**
     * @brief SLOT on_btn_browseRoot_clicked: Open the native Folder selector dialog.
     */
    void on_btn_browseRoot_clicked();

private:
    Ui::DialogOptions *ui;
};

}
}

#endif // DIALOGOPTIONS_H
