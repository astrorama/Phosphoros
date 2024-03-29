#ifndef FORMPOSTPROCESSING_H
#define FORMPOSTPROCESSING_H

#include "ParameterRule.h"
#include "PhzQtUI/SurveyModel.h"
#include <QModelIndex>
#include <QWidget>
#include <memory>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class FormPostProcessing;
}

/**
 * @brief The FormPostProcessing class
 */
class FormPostProcessing : public QWidget {
  Q_OBJECT

public:
  explicit FormPostProcessing(QWidget* parent = 0);
  ~FormPostProcessing();
  void loadPostProcessingPage(std::shared_ptr<SurveyModel> survey_model_ptr);

  void updateSelection(bool force_reload_cb = false);

signals:

  void navigateToCatalog(bool);

  void navigateToConfig();

  void navigateToComputeRedshift(bool);

  void navigateToParameter(bool);

  void quit(bool);

private slots:

  void on_btn_ToAnalysis_clicked();
  void on_btn_ToOption_clicked();
  void on_btn_ToCatalog_clicked();
  void on_btn_ToModel_clicked();
  void on_btn_exit_clicked();

  void on_cb_catalog_currentIndexChanged(int);

  void computePdfStat(int);
  void plotZVsZref(int);
  void plotResiduals(int);
  void computePpPdf(int);

private:
  std::unique_ptr<Ui::FormPostProcessing> ui;
  std::shared_ptr<SurveyModel>            m_survey_model_ptr;
  bool                                    m_disconnect_cb = false;

  void loadCbCatalog(std::string selected);
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // FORMPOSTPROCESSING_H
