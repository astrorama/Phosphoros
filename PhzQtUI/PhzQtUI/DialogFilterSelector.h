#ifndef DialogFilterSelector_H
#define DialogFilterSelector_H

#include <set>
#include <memory>
#include <QDialog>
#include "FilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogFilterSelector;
}

/**
 * @class DialogFilterSelector
 * @brief This popup allows the user to select a single filter. It is used
 * by the DialogLuminosityPrior.
 */
class DialogFilterSelector: public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogFilterSelector(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogFilterSelector();

  /**
   * @brief Set the initial filter value
   * @param filter_name Name of the initially selected filter.
   */
  void setFilter(std::string filter_name);

  signals:
  /**
   * @brief SIGNAL popupClosing: rised when the popup is closed.
   * The argument is the selected Filter Name.
   */
  void popupClosing(std::string);

private slots:
  /**
   * @brief SLOT on_btn_cancel_clicked: close the popup
   */
  void on_btn_cancel_clicked();

  /**
   * @brief SLOT on_btn_save_clicked: rises the
   *  SIGNAL popupClosing and close the popup.
   */
  void on_btn_save_clicked();

private:
  std::unique_ptr<Ui::DialogFilterSelector> ui;
};

}
}

#endif // DialogFilterSelector_H
