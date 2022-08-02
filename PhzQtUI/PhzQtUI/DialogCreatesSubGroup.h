#ifndef DIALOGCREATESUBGROUP_H
#define DIALOGCREATESUBGROUP_H

#include <QDialog>
#include <memory>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogCreateSubGroup;
}

/**
 * @class DialogCreateSubGroup
 * @brief This popup allows the user to provide the name of a folder to be
 * created within the provided parent folder.
 */
class DialogCreateSubGroup : public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogCreateSubGroup(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
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
  std::unique_ptr<Ui::DialogCreateSubGroup> ui;
  std::string                               m_parent_folder_full;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DIALOGCREATESUBGROUP_H
