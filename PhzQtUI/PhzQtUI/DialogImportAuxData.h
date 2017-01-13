#ifndef DIALOGIMPORTAUXDATA_H
#define DIALOGIMPORTAUXDATA_H

#include <memory>
#include <QDialog>
#include <string>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogImportAuxData;
}

/**
 * @class DialogImportAuxData
 * @brie This popup allows the user to copy file/folder into the provided path.
 */
class DialogImportAuxData: public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogImportAuxData(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogImportAuxData();

  /**
   * @brief Initialise the popup by setting its internal data
   * @param title
   * String to be set as popup title.
   * @param parentFolderFull
   * The full path of the parent folder into which the data will be copied
   * @param parentFolderDisplay
   * An aliases for displaying the parent folder to the user.
   */
  void setData(std::string title, std::string parentFolderFull,
      std::string parentFolderDisplay);

private slots:
  /**
   * @brief SLOT on_rb_file_clicked: switch the popup from the folder import to the
   * single file import mode.
   */
  void on_rb_file_clicked();

  /**
   * @brief SLOT on_rb_folder_clicked: switch the popup from the single file import to the
   * folder import mode.
   */
  void on_rb_folder_clicked();

  /**
   * @brief SLOT on_btn_browseFile_clicked: Open the native File selector dialog.
   */
  void on_btn_browseFile_clicked();

  /**
   * @brief SLOT on_btn_browseFolder_clicked: Open the native Folder selector dialog.
   */
  void on_btn_browseFolder_clicked();

  /**
   * @brief SLOT on_btn_import_clicked
   */
  void on_btn_import_clicked();

private:
  std::unique_ptr<Ui::DialogImportAuxData> ui;
  std::string m_parent_folder;
};

}
}

#endif // DIALOGIMPORTAUXDATA_H
