#ifndef FORMAUXDATAMANAGEMENT_H
#define FORMAUXDATAMANAGEMENT_H
#include <memory>
#include <QWidget>
#include <QProcess>
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzQtUI/SedTreeModel.h"
#include "PhzQtUI/MessageButton.h"

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




private:
    std::unique_ptr<Ui::FormAuxDataManagement> ui;
    DatasetRepo m_filter_repository;
    DatasetRepo m_seds_repository;
    DatasetRepo m_redenig_curves_repository;
    DatasetRepo m_luminosity_repository;

    std::vector<MessageButton*> m_message_buttons;
    void addButtonsToSedItem(QStandardItem* item, SedTreeModel* treeModel_sed);
};

}
}

#endif // FORMAUXDATAMANAGEMENT_H
