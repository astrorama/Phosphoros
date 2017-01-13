#ifndef DIALOGCATALOGNAME_H
#define DIALOGCATALOGNAME_H

#include <memory>
#include <QDialog>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogCatalogName;
}

/**
 * @class DialogCatalogName
 * @brief This popup allows the user to provide a name for the newly
 * created catalog type. The provided name is checked to not conflict
 *  with the existing catalog types.
 */
class DialogCatalogName: public QDialog {
  Q_OBJECT
public:

  /**
   * @brief Constructor
   */
  explicit DialogCatalogName(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogCatalogName();

  /**
   * @brief Set a proposed name at the opening of the popup.
   * @param default_name The name to be proposed to the user.
   */
  void setDefaultName(std::string default_name);

  /**
   * @brief Set the existing names the new name has to be different from.
   * @param existing_names A vector of string containing the name of
   * the existing catalogs.
   */
  void setExistingNames(std::vector<std::string> existing_names);

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
  std::unique_ptr<Ui::DialogCatalogName> ui;
  std::string m_name;
  std::vector<std::string> m_existing_names;

};

}
}

#endif // DIALOGCATALOGNAME_H
