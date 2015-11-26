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


class DialogLuminosityFunctions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLuminosityFunctions(QWidget *parent = 0);
    ~DialogLuminosityFunctions();

    void setInfos(std::vector<std::vector<LuminosityFunctionInfo>> infos,
        std::vector<LuminosityPriorConfig::SedGroup> groups,
        std::vector<double> zs);

signals:
    /**
     * @brief SIGNAL popupClosing: rised when the dialog is saved and the popup close.
     * The argument is the new set of functions.
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
