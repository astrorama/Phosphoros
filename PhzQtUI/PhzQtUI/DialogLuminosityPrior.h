/*
 * DialogLuminosityPrior.h
 *
 *  Created on: September 1, 2015
 *      Author: fdubath
 */

#ifndef DIALOGLUMINOSITYPRIOR_H_
#define DIALOGLUMINOSITYPRIOR_H_

#include <memory>
#include <QDialog>
#include <QTimer>
#include <map>
#include "ElementsKernel/Exception.h"
#include "PhzQtUI/LuminosityFunctionInfo.h"


#include "PhzLuminosity/SedGroup.h"

namespace boost{
namespace program_options{
 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogLuminosityPrior;
}

/**
 * @brief The DialogLuminosityPrior class.
 * This popup show the progress-bar during analysis run.
 */
class DialogLuminosityPrior : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLuminosityPrior(QWidget *parent = 0);
    ~DialogLuminosityPrior();



private slots:
void onGridButtonClicked(int x,int y);
void luminosityFunctionPopupClosing(LuminosityFunctionInfo info, int x, int y);
void on_btn_group_clicked();
void groupPopupClosing(std::vector<PhzLuminosity::SedGroup> groups);

private:
void ClearGrid();
void loadGrid(const std::vector<PhzLuminosity::SedGroup>& groups, const std::vector<double>& zs);
std::unique_ptr<Ui::DialogLuminosityPrior> ui;
std::vector<std::vector<LuminosityFunctionInfo>> m_luminosityInfos{};
std::vector<PhzLuminosity::SedGroup> m_groups{};
};

}
}



#endif /* DIALOGLUMINOSITYPRIOR_H_*/
