#ifndef DIALOGSELECTPARAM_H
#define DIALOGSELECTPARAM_H

#include <set>
#include <vector>
#include <string>
#include <memory>
#include <QDialog>
#include "FilterMapping.h"
#include "PhzQtUI/ModelSet.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogSelectParam;
}

/**
 * @class DialogSelectParam
 */
class DialogSelectParam: public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogSelectParam(const std::set<std::string>& param_list, QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogSelectParam();

  /**
   * @brief Set the initial sed value
   * @param sed_name Name of the initially selected sed.
   */
  void setParams(std::set<std::string> selected_param);

  signals:
  /**
   * @brief SIGNAL popupClosing: rised when the popup is closed.
   * The argument is the selected param Name.
   */
  void popupClosing(std::vector<std::string>);

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
  std::unique_ptr<Ui::DialogSelectParam> ui;
  const std::set<std::string>& m_param_list;
};

}
}

#endif // DIALOGSELECTPARAM_H
