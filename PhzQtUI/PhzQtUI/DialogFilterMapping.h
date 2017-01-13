#ifndef DIALOGFILTERMAPPING_H
#define DIALOGFILTERMAPPING_H

#include <set>
#include <string>
#include <vector>
#include <memory>
#include <QDialog>
#include "FilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogFilterMapping;
}

/**
 * @class DialogFilterMapping
 * @brief This popup allows the user to select the filters to be mapped
 * in the calling form. It instanciate and access directly the filters provider.
 */
class DialogFilterMapping: public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogFilterMapping(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogFilterMapping();

  /**
   * @brief Initialise the popup by setting its internal data
   * @param filters
   * A vector of string containing the names of the selected filters at the
   * opening of the popup.
   */
  void setFilters(const std::vector<std::string>& filters);

  signals:
  /**
   * @brief SIGNAL popupClosing: rised when the FilterMapping is saved and
   * the popup close.
   * The argument is the vector of selected filters.
   */
  void popupClosing( std::vector<std::string>);

private slots:
  /**
   * @brief SLOT on_btn_cancel_clicked: close the popup
   */
  void on_btn_cancel_clicked();

  /**
   * @brief SLOT on_btn_save_clicked: call the SIGNAL popupClosing
   * and close the popup.
   */
  void on_btn_save_clicked();

private:
  std::unique_ptr<Ui::DialogFilterMapping> ui;
};

}
}

#endif // DIALOGFILTERMAPPING_H
