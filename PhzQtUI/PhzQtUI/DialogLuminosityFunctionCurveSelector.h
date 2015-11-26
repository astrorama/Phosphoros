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
 * @brief The DialogLuminosityFunctionCurveSelector class
 * This popup allows the user to select a curve.
 */
class DialogLuminosityFunctionCurveSelector : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLuminosityFunctionCurveSelector(QWidget *parent = 0);
    ~DialogLuminosityFunctionCurveSelector();

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
