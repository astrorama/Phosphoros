#ifndef DIALOGZEROPOINTNAME_H
#define DIALOGZEROPOINTNAME_H

#include <QDialog>
#include <memory>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogZeroPointName;
}

/**
 * @class DialogZeroPointName
 * @brief This popup allows the user to provide a name.
 */
class DialogZeroPointName : public QDialog {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   */
  explicit DialogZeroPointName(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogZeroPointName();

  void setFolder(std::string base_folder);

  /**
   * @brief Get the name chosen by the user.
   * @return The new Catalog Type name.
   */
  std::string getName() const;

private slots:
  /**
   * @brief SLOT on_btn_create_clicked
   */
  void on_btn_create_clicked();

  /**
   * @brief SLOT on_btn_cancel_clicked
   */
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogZeroPointName> ui;
  std::string                              m_name;
  std::string                              m_folder;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DIALOGZEROPOINTNAME_H
