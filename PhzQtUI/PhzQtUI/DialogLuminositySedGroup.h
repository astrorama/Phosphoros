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
#include "PhzLuminosity/SedGroup.h"

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

    void setGroups(std::vector<PhzLuminosity::SedGroup> groups);

signals:
  void popupClosing(std::vector<PhzLuminosity::SedGroup> groups);


private slots:
void on_btn_cancel_clicked();
void on_btn_save_clicked();

void on_btn_add_clicked();
void onMoveRightClicked(int sed_group_id,int sed_id);
void onMoveLeftClicked(int sed_group_id,int sed_id);
void onDeleteGroupClicked(int sed_group_id,int);



private:
std::vector<std::string> getNewGroupNames();
void addGroup(PhzLuminosity::SedGroup group,int i, int i_max);
void clearSeds(int group_id);


void fillSedList(std::vector<std::string> seds ,int group_id, int max_group_id, QVBoxLayout* layout);
std::unique_ptr<Ui::DialogLuminositySedGroup> ui;
std::vector<PhzLuminosity::SedGroup> m_groups;

};

}
}



#endif /* DIALOGLUMINOSITYSEDGROUP_H_*/
