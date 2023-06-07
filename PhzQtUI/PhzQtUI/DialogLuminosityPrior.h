/*
 * DialogLuminosityPrior.h
 *
 *  Created on: September 1, 2015
 *      Author: fdubath
 */

#ifndef DIALOGLUMINOSITYPRIOR_H_
#define DIALOGLUMINOSITYPRIOR_H_

#include <QDialog>
#include <QModelIndex>
#include <QTimer>
#include <map>
#include <memory>

#include "ElementsKernel/Exception.h"
#include "PhzQtUI/LuminosityFunctionInfo.h"

#include "ModelSet.h"
#include "PhzQtUI/DatasetRepository.h"
#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/LuminosityPriorConfig.h"
#include "XYDataset/FileSystemProvider.h"

namespace boost {
namespace program_options {
class variable_value;
}
}  // namespace boost

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class DialogLuminosityPrior;
}

/**
 * @class DialogLuminosityPrior
 * @brief This popup Allows to create and configure Luminosity priors.
 */
class DialogLuminosityPrior : public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogLuminosityPrior(std::string filter,
						   	     DatasetRepo luminosity_repository,
							     DatasetRepo sed_repository,
							     DatasetRepo red_curve_repository,
							     QWidget* parent = 0);
  /**
   * @brief Destructor
   */
  ~DialogLuminosityPrior();

  /**
   * @brief Set the popup parameters
   * @param model A ModelSet the prior is build for. Contains the list of SED
   * (used to create the SED groups)
   * @param survey_name The name of the Catalog Type the prior is build for.
   * @param z_min The minimal value of the redshift (also contained in the model)
   * @param z_max The maximal value of the redshift (also contained in the model)
   *
   */
  void loadData(ModelSet model, std::string survey_name, std::string model_grid, double z_min, double z_max);

  /*
   * @brief called for updating the progress bar
   */
  void onProgress(int current, int total);

signals:
  /**
   * @brief SIGNAL Rised when the popup is closed.
   */
  void popupClosing();

private slots:

  /**
   * @brief SLOT Open the bulk schechter edition popup.
   */
  void on_bulk_btn_clicked();

  /**
   * @brief SLOT Edit the Luminosity Prior when a grid cell is double-clicked.
   */
  void priorGridDoubleClicked(QModelIndex);

  /**
   * @brief SLOT The user has changed the Prior (master) selection.
   * Will update the details part of the popup.
   * @param new_index The index for the selected Prior.
   */
  void priorSelectionChanged(QModelIndex new_index, QModelIndex);

  /**
   * @brief SLOT The user has clicked the new Prior button.
   */
  void on_btn_new_clicked();

  /**
   * @brief SLOT The user has clicked the duplicate Prior button.
   */
  void on_btn_duplicate_clicked();

  /**
   * @brief SLOT The user has clicked the delete Prior button.
   */
  void on_btn_delete_clicked();

  /**
   * @brief SLOT The user has clicked the edit Prior button.
   */
  void on_btn_edit_clicked();

  /**
   * @brief SLOT The user has clicked the save Prior button.
   */
  void on_btn_save_clicked();

  /**
   * @brief SLOT The user has clicked the Cancel Prior edition button.
   */
  void on_btn_cancel_clicked();

  /**
   * @brief SLOT raised when the MAGNITUDE/FLUX combobox is changed.
   */
  void on_cb_unit_currentIndexChanged(int);

  /**
   * @brief SLOT raised when the groups management button is clicked.
   */
  void on_btn_group_clicked();

  /**
   * @brief SLOT raised when the groups management popup is closing.
   */
  void groupPopupClosing(std::vector<LuminosityPriorConfig::SedGroup> groups);

  /**
   * @brief SLOT raised when the redshifts management button is clicked.
   */
  void on_btn_z_clicked();

  /**
   * @brief SLOT raised when the redshifts management popup is closing.
   */
  void zPopupClosing(std::vector<double> zs);

  /**
   * @brief SLOT raised when the luminosity function define button is clicked.
   */
  void onGridButtonClicked(size_t x, size_t y);

  /**
   * @brief SLOT raised when the Luminosity function popup is closing.
   */
  void luminosityFunctionPopupClosing(LuminosityFunctionInfo info, size_t x, size_t y);

  /**
   * @brief SLOT raised when the bulck schechter edit popup is closing.
   */
  void luminosityFunctionsPopupClosing(std::vector<std::vector<LuminosityFunctionInfo>> infos);

  /**
   * @brief SLOT raised when this popup is closing.
   */
  void on_btn_close_clicked();

private:
  void manageBtnEnability(bool in_edition, bool read_only);

  void manageBtnEnability(bool in_edition, bool read_only, bool has_selected_row);

  void loadMainGrid();

  void clearGrid();

  void loadGrid();

  bool validateInput(const size_t& current_index);
  void updateInfo(LuminosityPriorConfig& info);
  void updatePriorRow(QModelIndex& index, const size_t& row, const LuminosityPriorConfig& info);

  std::unique_ptr<Ui::DialogLuminosityPrior> ui;
  std::string                                m_filter;
  DatasetRepo                                m_luminosity_repository;
  DatasetRepo                                m_sed_repository;
  DatasetRepo                                m_red_curve_repository;
  QString                                    m_config_folder;

  QString m_grid_folder;

  ModelSet m_model;

  std::string m_survey_name;

  std::string m_model_grid_name;

  double m_z_min;

  double m_z_max;

  std::map<std::string, LuminosityPriorConfig> m_prior_configs;

  bool m_new = false;

  // currently select Prior informations (needed for the Lum. Function Grid)
  std::vector<std::vector<LuminosityFunctionInfo>> m_luminosityInfos{};
  std::vector<LuminosityPriorConfig::SedGroup>     m_groups{};
  std::vector<double>                              m_zs{};
  std::vector<GridButton*>                         m_grid_buttons{};
};
}  // namespace PhzQtUI
}  // namespace Euclid

#endif /* DIALOGLUMINOSITYPRIOR_H_*/
