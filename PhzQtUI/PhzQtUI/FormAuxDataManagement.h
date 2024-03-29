#ifndef FORMAUXDATAMANAGEMENT_H
#define FORMAUXDATAMANAGEMENT_H
#include "PhzQtUI/DataPackHandler.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "PhzQtUI/DatasetRepository.h"
#include "PhzQtUI/MessageButton.h"
#include "PhzQtUI/SedTreeModel.h"
#include "XYDataset/FileSystemProvider.h"
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QProgressDialog>
#include <QTreeView>
#include <QWidget>
#include <memory>
#include <vector>

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
class FormAuxDataManagement : public QWidget {
  Q_OBJECT

public:
  explicit FormAuxDataManagement(QWidget* parent = 0);
  ~FormAuxDataManagement();

  void setRepositories(DatasetRepo filter_repository, DatasetRepo seds_repository,
                       DatasetRepo redenig_curves_repository, DatasetRepo luminosity_repository);
  void loadManagementPage(int index = 0);

signals:

  void lockNavigation(int);
  void unlockNavigation();

private slots:

  void sedProcessStarted();
  void sedProcessfinished(int, QProcess::ExitStatus);

  void deletFilterGroupButtonClicked(const QString& group);
  void deletSedGroupButtonClicked(const QString& group);
  void deletRedGroupButtonClicked(const QString& group);
  void deletLumGroupButtonClicked(const QString& group);

  void addEmissionLineButtonClicked(const QString& group);
  void getParameterInfoClicked(const QString& file);

  void on_btn_import_filter_clicked();
  void on_btn_import_sed_clicked();
  void on_btn_sun_sed_clicked();
  void on_btn_import_reddening_clicked();
  void on_btn_import_luminosity_clicked();
  void on_btn_interp_clicked();

  void on_bt_reloadDP_clicked();
  void on_btn_planck_clicked();
  void reloadAuxData();

  void displayFilter();
  void displaySED();
  void displayRed();
  void displayLum();

  void copyingFilterFinished(bool, QVector<QString>);
  void copyingSEDFinished(bool, QVector<QString>);
  void copyingRedFinished(bool, QVector<QString>);
  void copyingLumFinished(bool, QVector<QString>);
  void copyProgress(qint64, qint64);
  void sunSedPopupClosing(std::vector<std::string>);

  void httpReadyPlanckRead();
  void cancelDownloadPlanck();
  void updateDownloadProgress(qint64 bytesRead, qint64 totalBytes);

private:
  QProgressDialog* m_progress_dialog = nullptr;

  QNetworkAccessManager* m_network_manager    = nullptr;
  QFile*                 m_downloaded_file    = nullptr;
  bool                   m_httpRequestAborted = false;
  QNetworkReply*         m_reply              = nullptr;
  std::string            m_planck_file        = "";
  std::string            m_planck_url =
      "https://lambda.gsfc.nasa.gov/data/foregrounds/EBV/lambda_meisner_finkbeiner_2015_dust_map.fits";

  std::unique_ptr<Ui::FormAuxDataManagement> ui;
  DatasetRepo                                m_filter_repository;
  DatasetRepo                                m_seds_repository;
  DatasetRepo                                m_redenig_curves_repository;
  DatasetRepo                                m_luminosity_repository;
  std::unique_ptr<DataPackHandler>           m_dataPackHandler;

  std::vector<std::unique_ptr<MessageButton>> m_message_buttons;
  std::vector<std::unique_ptr<MessageButton>> m_filter_del_buttons;
  std::vector<std::unique_ptr<MessageButton>> m_sed_del_buttons;
  std::vector<std::unique_ptr<MessageButton>> m_red_del_buttons;
  std::vector<std::unique_ptr<MessageButton>> m_lum_del_buttons;

  template <class TreeModel>
  void addDeleteButtonsToItem(QStandardItem* item, TreeModel* treeModel, QTreeView* view, const char* slot,
                              std::vector<std::unique_ptr<MessageButton>>& btn_vector, int index);

  void addButtonsToSedItem(QStandardItem* item, SedTreeModel* treeModel_sed);

  void handleDataException(std::string message);
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // FORMAUXDATAMANAGEMENT_H
