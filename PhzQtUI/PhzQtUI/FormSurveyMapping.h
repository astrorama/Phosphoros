#ifndef FORMSURVEYMAPPING_H
#define FORMSURVEYMAPPING_H

#include "FilterMapping.h"
#include "PhzQtUI/DatasetRepository.h"
#include "PhzQtUI/SurveyModel.h"
#include "XYDataset/FileSystemProvider.h"
#include <QItemSelection>
#include <QModelIndex>
#include <QStandardItem>
#include <QWidget>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class FormSurveyMapping;
}

/**
 * @brief The FormSurveyMapping class
 * This widget allows the user to create and manage Survey Filter Mappings.
 */
class FormSurveyMapping : public QWidget {
  Q_OBJECT

public:
  explicit FormSurveyMapping(QWidget* parent = 0);
  ~FormSurveyMapping();

  void loadMappingPage(std::shared_ptr<SurveyModel> survey_model_ptr, DatasetRepo filter_repository,
                       std::string new_path);

  void updateSelection(bool force_reload_cb = false);

signals:

  void navigateToParameter(bool);

  void navigateToConfig();

  void navigateToComputeRedshift(bool);

  void navigateToPostProcessing(bool);

  void quit(bool);

private slots:

  void on_btn_ToAnalysis_clicked();
  void on_btn_ToOption_clicked();
  void on_btn_ToModel_clicked();
  void on_btn_ToPP_clicked();
  void on_btn_exit_clicked();
  void copyingFinished(bool, QVector<QString>);
  void copyProgress(qint64, qint64);
  void filter_model_changed(QStandardItem*);

  void filterMappingSelectionChanged(const QItemSelection&, const QItemSelection&);

  void filterEditionPopupClosing(std::vector<std::string>);

  void on_btn_MapNew_clicked();

  void on_btn_MapDuplicate_clicked();

  void on_btn_map_delete_clicked();

  void on_btn_purgeGrids_clicked();

  void on_btn_MapCancel_clicked();

  void on_btn_MapSave_clicked();

  void on_btn_ImportColumn_clicked();

  void on_btn_SelectFilters_clicked();

  void on_cb_SourceId_currentIndexChanged(int index);

  void on_cb_Dec_currentIndexChanged(int index);

  void on_cb_Ra_currentIndexChanged(int index);

  void on_cb_GalEbv_currentIndexChanged(int index);

  void on_cb_RefZ_currentIndexChanged(int index);

  void on_txt_nonDetection_textEdited(const QString& text);

  void on_txt_UpperLimit_textEdited(const QString& text);

  void on_cb_catalog_type_currentIndexChanged(int);

  void on_btn_prop_err_clicked();

  void on_ckb_error_prop_stateChanged(int state);

  void on_ckb_filterShift_stateChanged(int state);

private:
  std::unique_ptr<Ui::FormSurveyMapping> ui;
  DatasetRepo                            m_filter_repository;
  bool                                   m_mappingInsert;
  bool                                   m_filterInsert;
  std::set<std::string>                  m_column_from_file;
  std::string                            m_default_survey;
  std::shared_ptr<SurveyModel>           m_survey_model_ptr;
  bool                                   m_diconnect_cb = false;
  bool                                   m_loading      = false;

  void fillControlsWithSelected();

  std::set<std::string> getFilteredColumns();
  void                  loadCatalogCB(std::string selected);
  void                  setFilterMappingInEdition();
  void                  setFilterMappingInView();
  void                  loadColumnFromFile(std::string path);
  void                  fillCbColumns(std::string current_id_value = "", std::string current_ra_value = "",
                                      std::string current_dec_value = "", std::string current_gebv_value = "",
                                      std::string current_refz_value = "");

  std::vector<std::string>   getGridFiltersNames() const;
  std::vector<FilterMapping> getMappingFromGrid() const;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // FORMSURVEYMAPPING_H
