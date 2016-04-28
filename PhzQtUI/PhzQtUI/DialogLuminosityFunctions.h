#ifndef DIALOGLUMINOSITYFUNCTIONS_H
#define DIALOGLUMINOSITYFUNCTIONS_H

#include <memory>
#include <QDialog>
#include <string>
#include "PhzQtUI/LuminosityFunctionInfo.h"
#include "PhzQtUI/LuminosityPriorConfig.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogLuminosityFunctions;
}

/**
 * @class DialogLuminosityFunctions
 * @brief This popup allows the bulk edition of the Schechter parameters.
 * It is called from the Luminosity prior configuration popup.
 */
class DialogLuminosityFunctions: public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogLuminosityFunctions(QWidget *parent = 0);
  /**
   * @brief Destructor
   */
  ~DialogLuminosityFunctions();

  /**
   * @brief Set the values of the Schechters parameters at the opening of
   * the popup.
   * @param infos A vector of LuminosityFunctionInfo containing the
   * Schechter parameters values.
   */
  void setInfos(std::vector<std::vector<LuminosityFunctionInfo>> infos,
      std::vector<LuminosityPriorConfig::SedGroup> groups,
      std::vector<double> zs);

  signals:
  /**
   * @brief SIGNAL popupClosing: raised when the dialog is saved and the
   * popup close. The argument is the new set of functions.
   */
  void popupClosing(std::vector<std::vector<LuminosityFunctionInfo>>);

private slots:

  /**
   * @brief SLOT on_btn_save_clicked
   */
  void on_btn_save_clicked();

  /**
   * @brief SLOT on_btn_cancel_clicked
   */
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogLuminosityFunctions> ui;

  std::vector<std::vector<LuminosityFunctionInfo>> m_infos;
  std::vector<LuminosityPriorConfig::SedGroup> m_groups;
  std::vector<double> m_zs;

};

}
}

#endif // DIALOGLUMINOSITYFUNCTIONS_H
