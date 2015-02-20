#ifndef FORMAUXDATAMANAGEMENT_H
#define FORMAUXDATAMANAGEMENT_H

#include <QWidget>

namespace Ui {
class FormAuxDataManagement;
}

/**
 * @brief The FormAuxDataManagement class
 * This widget allows the user to manage the Aux. Data, by uploading/deleting
 * files in the pre-defined folders.
 */
class FormAuxDataManagement : public QWidget
{
    Q_OBJECT

public:
    explicit FormAuxDataManagement(QWidget *parent = 0);
    ~FormAuxDataManagement();
    void loadManagementPage(int index=0);

signals:

    void navigateToHome();

private slots:

    void on_btn_ManageToHome_clicked();

    void on_btn_RedImport_clicked();

    void on_btn_RedSubGroup_clicked();

    void on_btn_RedDelete_clicked();

    void on_btn_SedImport_clicked();

    void on_btn_SedSubGroup_clicked();

    void on_btn_SedDelete_clicked();

    void on_btn_FilterImport_clicked();

    void on_btn_FilterSubGroup_clicked();

    void on_btn_FilterDelete_clicked();


private:
    Ui::FormAuxDataManagement *ui;
};

#endif // FORMAUXDATAMANAGEMENT_H
