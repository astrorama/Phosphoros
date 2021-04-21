#ifndef FORMAUXDATAMANAGEMENT_H
#define FORMAUXDATAMANAGEMENT_H
#include <memory>
#include <QWidget>
#include <QProcess>
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzQtUI/SedTreeModel.h"
#include "PhzQtUI/MessageButton.h"
#include "PhzQtUI/DataPackHandler.h"

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

    void setRepositories(DatasetRepo filter_repository,
                         DatasetRepo seds_repository,
                         DatasetRepo redenig_curves_repository,
                         DatasetRepo luminosity_repository);
    void loadManagementPage(int index=0);


private slots:

void sedProcessStarted();
void sedProcessfinished(int, QProcess::ExitStatus);

void addEmissionLineButtonClicked(const QString& group);
void getParameterInfoClicked(const QString& file);


void on_btn_import_filter_clicked();
void on_btn_import_sed_clicked();
void on_btn_sun_sed_clicked();
void on_btn_import_reddening_clicked();
void on_btn_import_luminosity_clicked();

void on_bt_reloadDP_clicked();
void reloadAuxData();

void copyingFilterFinished(bool, QVector<QString>);
void copyingSEDFinished(bool, QVector<QString>);
void copyingRedFinished(bool, QVector<QString>);
void copyingLumFinished(bool, QVector<QString>);
void copyProgress(qint64, qint64);
void sunSedPopupClosing(std::string);



private:
    std::unique_ptr<Ui::FormAuxDataManagement> ui;
    DatasetRepo m_filter_repository;
    DatasetRepo m_seds_repository;
    DatasetRepo m_redenig_curves_repository;
    DatasetRepo m_luminosity_repository;
    std::unique_ptr<DataPackHandler> m_dataPackHandler;

    std::vector<MessageButton*> m_message_buttons;
    void addButtonsToSedItem(QStandardItem* item, SedTreeModel* treeModel_sed);
};

}
}

#endif // FORMAUXDATAMANAGEMENT_H
