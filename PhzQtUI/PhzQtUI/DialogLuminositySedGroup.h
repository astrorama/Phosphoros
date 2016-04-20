/*
 * DialogLuminositySedGroup.h
 *
 *  Created on: September 4, 2015
 *      Author: fdubath
 */

#ifndef DIALOGLUMINOSITYSEDGROUP_H_
#define DIALOGLUMINOSITYSEDGROUP_H_

#include <vector>
#include <memory>
#include <QDialog>
#include <QVBoxLayout>
#include <map>
#include "ElementsKernel/Exception.h"
#include "PhzQtUI/LuminosityPriorConfig.h"

namespace boost{
namespace program_options{
 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogLuminositySedGroup;
}

/**
 * @brief The DialogLuminositySedGroup class.
 * This popup show the progress-bar during analysis run.
 */
class DialogLuminositySedGroup : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLuminositySedGroup(QWidget *parent = 0);
    ~DialogLuminositySedGroup();

    void setGroups(std::vector<LuminosityPriorConfig::SedGroup> groups);
    /**
     * @param missing_seds
     * SEDs not present in the parameter space to be removed
     *
     * @param new_seds
     * SEDs not present in the config to be added
     */
    void setDiff(std::vector<std::string> missing_seds, std::vector<std::string> new_seds);

signals:
  void popupClosing(std::vector<LuminosityPriorConfig::SedGroup> groups);


private slots:
void on_btn_cancel_clicked();
void on_btn_save_clicked();

void on_btn_add_clicked();
void onDeleteGroupClicked(size_t sed_group_id,size_t);



private:
void readNewGroups();
void addGroup(LuminosityPriorConfig::SedGroup group,size_t i, size_t i_max);
std::unique_ptr<Ui::DialogLuminositySedGroup> ui;
std::vector<LuminosityPriorConfig::SedGroup> m_groups;

};

}
}



#endif /* DIALOGLUMINOSITYSEDGROUP_H_*/
