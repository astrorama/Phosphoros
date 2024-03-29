#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include <QMainWindow>
#include <QProcess>
#include <map>
#include <memory>
#include <experimental/memory_resource>

#include "PhzQtUI/DataPackHandler.h"
#include "PhzQtUI/ModelSetModel.h"
#include "PhzQtUI/OptionModel.h"
#include "PhzQtUI/SurveyModel.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class MainWindow;
}

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

signals:
  void changeMainStackedWidgetIndex(int index);

private slots:

  void on_btn_HomeToModel_clicked();

  void on_btn_HomeToAnalysis_clicked();

  void on_btn_HomeToOption_clicked();

  void on_btn_HomeToCatalog_clicked();

  void on_btn_HomeToPP_clicked();

  void navigateToNewCatalog(std::string);

  void navigateToHome();

  void navigateToHomeWithReset(bool);

  void navigateToParameter(bool);

  void navigateToCatalog(bool);

  void navigateToComputeRedshift(bool);

  void navigateToConfig();

  void navigateToPostProcessing(bool);

  void quit(bool);

  void loadAuxData();

private:
  std::unique_ptr<Ui::MainWindow> ui;
  void                            resetRepo();

  DatasetRepo                      m_filter_repository;
  DatasetRepo                      m_seds_repository;
  DatasetRepo                      m_redenig_curves_repository;
  DatasetRepo                      m_luminosity_repository;
  std::shared_ptr<OptionModel>     m_option_model_ptr{new OptionModel};
  std::shared_ptr<SurveyModel>     m_survey_model_ptr{new SurveyModel};
  std::shared_ptr<ModelSetModel>   m_model_set_model_ptr=nullptr;
  std::unique_ptr<DataPackHandler> m_dataPackHandler;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // MAINWINDOW_H
