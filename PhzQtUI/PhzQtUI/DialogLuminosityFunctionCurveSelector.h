#ifndef DialogLuminosityFunctionCurveSelector_H
#define DialogLuminosityFunctionCurveSelector_H

#include <set>
#include <memory>
#include <QDialog>
#include "FilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogLuminosityFunctionCurveSelector;
}

/**
 * @class DialogLuminosityFunctionCurveSelector
 * @brief This popup allows the user to select a luminosity function curve.
 */
class DialogLuminosityFunctionCurveSelector: public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogLuminosityFunctionCurveSelector(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogLuminosityFunctionCurveSelector();

  /**
   * @brief set the curve name at the opening of the popup.
   * @parame curve_name The name of the curve displayed when the popup opens.
   */
  void setCurve(std::string curve_name);

  signals:
  /**
   * @brief SIGNAL popupClosing: rised when the popup is closed.
   * The argument is the curve Name.
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
  std::unique_ptr<Ui::DialogLuminosityFunctionCurveSelector> ui;
};

}
}

#endif // DialogLuminosityFunctionCurveSelector_H
