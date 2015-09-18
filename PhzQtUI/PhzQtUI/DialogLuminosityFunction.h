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

namespace boost{
namespace program_options{
 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogLuminosityFunction;
}

/**
 * @brief The DialogLuminosityFunction class.
 * This popup show the progress-bar during analysis run.
 */
class DialogLuminosityFunction : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLuminosityFunction(QWidget *parent = 0);
    ~DialogLuminosityFunction();
    /**
     * @brief load the luminositiy function into the popup
     * and store the calling coordinates
     */
    void setInfo(LuminosityFunctionInfo info, size_t x, size_t y);

signals:
  /**
   * @brief Rised when the popup is closing after a save action: return the
   * new luminosity function info and the calling coordinates.
   */
  void popupClosing(LuminosityFunctionInfo info, size_t x, size_t y);


private slots:
void on_btn_cancel_clicked();
void on_btn_save_clicked();
void on_btn_curve_clicked();
void on_gb_schechter_clicked();
void on_gb_custom_clicked();

void curvePopupClosing(std::string curve);


private:
std::unique_ptr<Ui::DialogLuminosityFunction> ui;
LuminosityFunctionInfo m_FunctionInfo;
size_t m_x;
size_t m_y;


};

}
}



#endif /* DialogLUMINOSITYFUNCTION_H_*/
