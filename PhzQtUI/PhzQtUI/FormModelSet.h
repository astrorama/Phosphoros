#ifndef FORMMODELSET_H
#define FORMMODELSET_H

#include <memory>
#include <QWidget>
#include <QModelIndex>
#include "ParameterRule.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"


namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class FormModelSet;
}


typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

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

     void loadSetPage(DatasetRepo seds_repository,
         DatasetRepo redenig_curves_repository);

signals:
    void navigateToHome();

private slots:
    void setSelectionChanged(QModelIndex, QModelIndex);
    void rulesSelectionChanged(QModelIndex, QModelIndex);

    void setGridDoubleClicked(QModelIndex);

    void parameterGridDoubleClicked(QModelIndex);

    void setEditionPopupClosing(int,ParameterRule,bool);

    void on_btn_SetEdit_clicked();

    void on_btn_SetCancel_clicked();

    void on_btn_SetSave_clicked();

    void on_btn_SetToHome_clicked();

    void on_btn_backHome_clicked();

    void on_btn_SetNew_clicked();

    void on_btn_SetDuplicate_clicked();

    void on_btn_SetDelete_clicked();

    void on_btn_viewSet_clicked();

    void on_btn_new_set_clicked();
    void on_btn_duplicate_set_clicked();
    void on_btn_delete_set_clicked();


private:
    std::unique_ptr<Ui::FormModelSet> ui;
    void setModelInEdition();
    void setModelInView();

    bool m_setInsert;
    DatasetRepo m_seds_repository;
    DatasetRepo m_redenig_curves_repository;
};

}
}

#endif // FORMMODELSET_H
