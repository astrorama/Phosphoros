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
#include <QModelIndex>

#include "ElementsKernel/Exception.h"
#include "PhzQtUI/LuminosityFunctionInfo.h"

#include "PhzQtUI/LuminosityPriorConfig.h"
#include "PhzLuminosity/SedGroup.h"
#include "PhzQtUI/GridButton.h"
#include "ModelSet.h"

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

    void loadData(
        ModelSet model,
        std::string survey_name,
        double z_min,
        double z_max);

signals:
    void popupClosing();


private slots:

void priorSelectionChanged(QModelIndex new_index, QModelIndex);

void on_btn_new_clicked();
void on_btn_cancel_clicked();
void on_btn_save_clicked();
void on_btn_delete_clicked();
void on_btn_edit_clicked();
void on_btn_close_clicked();

void on_btn_filter_clicked();
void filterPopupClosing(std::string filter);


void on_btn_z_clicked();
void zPopupClosing(std::vector<double> zs);

void on_cb_unit_currentIndexChanged(const QString &);


void onGridButtonClicked(size_t x,size_t y);
void luminosityFunctionPopupClosing(LuminosityFunctionInfo info, size_t x, size_t y);
void on_btn_group_clicked();
void groupPopupClosing(std::vector<PhzLuminosity::SedGroup> groups);

private:

void manageBtnEnability(bool in_edition);


void loadMainGrid();

void clearGrid();
void loadGrid();
std::unique_ptr<Ui::DialogLuminosityPrior> ui;


std::map<std::string, LuminosityPriorConfig> m_prior_configs;
QString m_config_folder;
QString m_grid_folder;

bool m_new=false;

ModelSet m_model;
std::string m_survey_name;
double m_z_min;
double m_z_max;

std::vector<std::vector<LuminosityFunctionInfo>> m_luminosityInfos{};
std::vector<PhzLuminosity::SedGroup> m_groups{};
std::vector<double> m_zs{};

std::vector<GridButton*> m_grid_buttons{};
};
}
}



#endif /* DIALOGLUMINOSITYPRIOR_H_*/
