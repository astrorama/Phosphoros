#ifndef FORMMODELSET_H
#define FORMMODELSET_H

#include <memory>
#include <QWidget>
#include <QModelIndex>
#include "ParameterRule.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class FormModelSet;
}

/**
 * @brief The FormModelSet class
 * This widget allows the user to create and manage the Astronomical Model Sets.
 */
class FormModelSet : public QWidget
{
    Q_OBJECT

public:
    explicit FormModelSet(QWidget *parent = 0);
    ~FormModelSet();

     void loadSetPage();

signals:
    void navigateToHome();

private slots:
    void setSelectionChanged(QModelIndex, QModelIndex);

    void setGridDoubleClicked(QModelIndex);

    void parameterGridDoubleClicked(QModelIndex);

    void setEditionPopupClosing(std::map<int,ParameterRule>);

    void on_btn_SetEdit_clicked();

    void on_btn_SetCancel_clicked();

    void on_btn_SetSave_clicked();

    void on_btn_SetToHome_clicked();

    void on_btn_backHome_clicked();

    void on_btn_SetNew_clicked();

    void on_btn_SetDuplicate_clicked();

    void on_btn_SetDelete_clicked();

    void on_btn_SetToRules_clicked();
    void on_btn_viewSet_clicked();



private:
    std::unique_ptr<Ui::FormModelSet> ui;
    void setModelInEdition();
    void setModelInView();

    bool m_setInsert;
};

}
}

#endif // FORMMODELSET_H
