/*
 * DialogLuminosityFunction.h
 *
 *  Created on: September 2, 2015
 *      Author: fdubath
 */

#ifndef DialogLUMINOSITYFUNCTION_H_
#define DialogLUMINOSITYFUNCTION_H_

#include <memory>
#include <QDialog>
#include <QTimer>
#include <map>
#include "ElementsKernel/Exception.h"
#include "PhzQtUI/LuminosityFunctionInfo.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"


namespace boost {
namespace program_options {
class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class DialogLuminosityFunction;
}

/**
 * @class DialogLuminosityFunction
 * @brief This popup allow to specify the Luminosity function, providing
 * Schechter parameters or a custom curve.
 */
class DialogLuminosityFunction: public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogLuminosityFunction(DatasetRepo luminosity_repository, QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogLuminosityFunction();

  /**
   * @brief load the luminositiy function into the popup
   * and store the calling coordinates
   * @param info The LuminosityFunctionInfo storing the current values for the
   * Luminosity function
   * @param x The redshift range coordinate in the calling grid, allowing to
   * associate the Luminosity function info with the right luminosity function
   * @param y The SED group  coordinate in the calling grid, allowing to
   * associate the Luminosity function info with the right luminosity function
   */
  void setInfo(LuminosityFunctionInfo info, size_t x, size_t y);

  signals:
  /**
   * @brief SIGNAL Rised when the popup is closing after a save action:
   * return the new luminosity function info and the calling coordinates.
   * @param info The LuminosityFunctionInfo storing the modified values for the
   * Luminosity function
   * @param x The redshift range coordinate in the calling grid, allowing to
   * associate the Luminosity function info with the right luminosity function
   * @param y The SED group  coordinate in the calling grid, allowing to
   * associate the Luminosity function info with the right luminosity function
   */
  void popupClosing(LuminosityFunctionInfo info, size_t x, size_t y);

private slots:
  /**
   * @brief SLOT Called when the cancel button is clicked, close the popup
   */
  void on_btn_cancel_clicked();

  /**
   * @brief SLOT Called when the save button is clicked, call the
   * SIGNAL popupClosing and close the popup
   */
  void on_btn_save_clicked();

  /**
   * @brief SLOT Called when the select curve button is clicked, open the
   * Curve selection popup.
   */
  void on_btn_curve_clicked();

  /**
   * @brief SLOT Called when the the user select the Schechter option.
   * Will disable the curve option.
   */
  void on_gb_schechter_clicked();

  /**
   * @brief SLOT Called when the the user select the curve option.
   * Will disable the Schechter option
   */
  void on_gb_custom_clicked();

  /**
   *  @brief SLOT Called when the the Curve selection popup is closed,
   *   Will update the curve name.
   */
  void curvePopupClosing(std::string curve);

private:
  std::unique_ptr<Ui::DialogLuminosityFunction> ui;
  LuminosityFunctionInfo m_FunctionInfo;
  size_t m_x;
  size_t m_y;
  DatasetRepo m_luminosity_repository;

};

}
}

#endif /* DialogLUMINOSITYFUNCTION_H_*/
