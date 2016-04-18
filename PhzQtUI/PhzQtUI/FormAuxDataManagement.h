#ifndef FORMAUXDATAMANAGEMENT_H
#define FORMAUXDATAMANAGEMENT_H
#include <memory>
#include <QWidget>
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

namespace Euclid {
namespace PhzQtUI {




namespace Ui {
class FormAuxDataManagement;
}


typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

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

    void setRepositories(DatasetRepo seds_repository,
        DatasetRepo redenig_curves_repository);
    void loadManagementPage(int index=0);



private slots:

    void on_btn_RedImport_clicked();

    void on_btn_RedSubGroup_clicked();

    void on_btn_RedDelete_clicked();

    void on_btn_SedImport_clicked();

    void on_btn_SedSubGroup_clicked();

    void on_btn_SedDelete_clicked();

    void on_btn_FilterImport_clicked();

    void on_btn_FilterSubGroup_clicked();

    void on_btn_FilterDelete_clicked();

    void on_btn_LuminosityImport_clicked();

    void on_btn_LuminositySubGroup_clicked();

    void on_btn_LuminosityDelete_clicked();


private:
    std::unique_ptr<Ui::FormAuxDataManagement> ui;
    DatasetRepo m_seds_repository;
    DatasetRepo m_redenig_curves_repository;
};

}
}

#endif // FORMAUXDATAMANAGEMENT_H
