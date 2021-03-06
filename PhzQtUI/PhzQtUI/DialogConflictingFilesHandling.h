#ifndef DIALOGCONFLICTINGFILESHANDLING_H
#define DIALOGCONFLICTINGFILESHANDLING_H

#include <QDialog>
#include <memory>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogConflictingFilesHandling;
}

/**
 * @class DialogConflictingFilesHandling
 */
class DialogConflictingFilesHandling : public QDialog {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   */
  explicit DialogConflictingFilesHandling(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogConflictingFilesHandling();

  void setFilesPath(std::string temp_folder, std::string conflicting_file, std::string resolution_file);

  void loadConflicts();

  void saveResolution();

private slots:

  void on_btn_apply_clicked();
  void on_cb_all_clicked();

private:
  std::unique_ptr<Ui::DialogConflictingFilesHandling> ui;
  std::string                                         m_temp_folder;
  std::string                                         m_conflicting_file;
  std::string                                         m_resolution_file;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DIALOGCONFLICTINGFILESHANDLING_H
