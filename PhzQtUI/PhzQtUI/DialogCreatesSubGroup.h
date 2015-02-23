#ifndef DIALOGCREATESUBGROUP_H
#define DIALOGCREATESUBGROUP_H

#include <QDialog>

namespace Ui {
class DialogCreateSubGroup;
}

/**
 * @brief The DialogCreateSubGroup class.
 * This popup allows the user to create a folder
 * in the provided parent folder.
 */
class DialogCreateSubGroup : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCreateSubGroup(QWidget *parent = 0);
    ~DialogCreateSubGroup();

    /**
     * @brief Initialise the popup by setting its internal data
     * @param folderFull
     * the path of the parent folder into which the new folder has to be created
     * @param folderDisplay
     * an alias for displaying the parent folder to the user.
     */
    void setParentFolder(std::string folderFull, std::string folderDisplay);

private slots:
    /**
     * @brief SLOT on_btn_cancel_clicked: close the popup.
     */
    void on_btn_cancel_clicked();

    /**
     * @brief SLOT on_pushButton_clicked: try to create the sub-folder,
     * if the operation succeed close the popup.
     */
    void on_pushButton_clicked();

private:
    Ui::DialogCreateSubGroup *ui;
    std::string m_parent_folder_full;
};

#endif // DIALOGCREATESUBGROUP_H
