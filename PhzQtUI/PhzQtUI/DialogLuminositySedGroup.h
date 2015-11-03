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

signals:
  void popupClosing(std::vector<LuminosityPriorConfig::SedGroup> groups);


private slots:
void on_btn_cancel_clicked();
void on_btn_save_clicked();

void on_btn_add_clicked();
void onMoveRightClicked(size_t sed_group_id,size_t sed_id);
void onMoveLeftClicked(size_t sed_group_id,size_t sed_id);
void onDeleteGroupClicked(size_t sed_group_id,size_t);



private:
std::vector<std::string> getNewGroupNames();
void addGroup(LuminosityPriorConfig::SedGroup group,size_t i, size_t i_max);
void clearSeds(size_t group_id);


void fillSedList(std::vector<std::string> seds ,size_t group_id, size_t max_group_id, QVBoxLayout* layout);
std::unique_ptr<Ui::DialogLuminositySedGroup> ui;
std::vector<LuminosityPriorConfig::SedGroup> m_groups;

};

}
}



#endif /* DIALOGLUMINOSITYSEDGROUP_H_*/
