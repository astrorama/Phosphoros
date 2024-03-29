#ifndef FORMMODELSET_H
#define FORMMODELSET_H

#include "ParameterRule.h"
#include "PhzQtUI/DatasetRepository.h"
#include "PhzQtUI/ModelSetModel.h"
#include "XYDataset/FileSystemProvider.h"
#include <QModelIndex>
#include <QWidget>
#include <memory>
#include <set>
#include <vector>

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
class FormModelSet : public QWidget {
  Q_OBJECT

public:
  explicit FormModelSet(QWidget* parent = 0);
  ~FormModelSet();

  void loadSetPage(std::shared_ptr<ModelSetModel> model_set_model_ptr, DatasetRepo seds_repository,
                   DatasetRepo redenig_curves_repository);

  void updateSelection();

signals:

  void navigateToCatalog(bool);

  void navigateToConfig();

  void navigateToComputeRedshift(bool);

  void navigateToPostProcessing(bool);

  void quit(bool);

private slots:

  void on_btn_ToAnalysis_clicked();
  void on_btn_ToOption_clicked();
  void on_btn_ToCatalog_clicked();
  void on_btn_ToPP_clicked();
  void on_btn_exit_clicked();

  void on_btn_conf_z_clicked();
  void on_btn_conf_ebv_clicked();

  void on_cb_selSpace_currentIndexChanged(int);

  void rulesSelectionChanged(QModelIndex, QModelIndex);

  void parameterGridDoubleClicked(QModelIndex);

  void setEditionPopupClosing(int, ParameterRule, bool);
  void redshiftRangePopupClosing(std::vector<Range>, std::set<double>);
  void ebvRangePopupClosing(std::vector<Range>, std::set<double>);

  void on_btn_SetCancel_clicked();

  void on_btn_SetSave_clicked();

  void on_btn_SetNew_clicked();

  void on_btn_SetDuplicate_clicked();

  void on_btn_SetDelete_clicked();

  void on_btn_open_region_clicked();

  void on_btn_new_region_clicked();

  void on_btn_duplicate_region_clicked();

  void on_btn_delete_region_clicked();

  void on_txt_SetName_textEdited(const QString& text);

private:
  std::unique_ptr<Ui::FormModelSet> ui;
  void                              setModelInEdition();
  void                              setModelInView();
  void                              selectFromGrid();
  void                              reload_cb();
  bool                              checkEbvZ();

  std::shared_ptr<ModelSetModel> m_model_set_model_ptr;
  bool                           m_setInsert;
  DatasetRepo                    m_seds_repository;
  DatasetRepo                    m_redenig_curves_repository;
  bool                           m_diconnect_cb = false;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // FORMMODELSET_H
