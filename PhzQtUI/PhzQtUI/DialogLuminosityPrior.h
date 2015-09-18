/*
 * DialogLuminosityPrior.h
 *
 *  Created on: September 1, 2015
 *      Author: fdubath
 */

#ifndef DIALOGLUMINOSITYPRIOR_H_
#define DIALOGLUMINOSITYPRIOR_H_

#include <memory>
#include <QDialog>
#include <QTimer>
#include <map>
#include <QModelIndex>

#include "ElementsKernel/Exception.h"
#include "PhzQtUI/LuminosityFunctionInfo.h"

#include "PhzQtUI/LuminosityPriorConfig.h"
#include "PhzLuminosity/SedGroup.h"
#include "PhzQtUI/GridButton.h"
#include "ModelSet.h"

namespace boost{
namespace program_options{
 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogLuminosityPrior;
}

/**
 * @brief The DialogLuminosityPrior class.
 * This popup show the progress-bar during analysis run.
 */
class DialogLuminosityPrior : public QDialog{
  Q_OBJECT

public:
  explicit DialogLuminosityPrior(QWidget *parent = 0);
  ~DialogLuminosityPrior();

  void loadData(ModelSet model, std::string survey_name, double z_min,
      double z_max);

  // called for updating the progress bar
  void onProgress(int current,int total);

signals:
  void popupClosing();

private slots:
  //// Luminosity prior related slots
  // slot raised when the master grid (Prior grid) selection is changed
  void priorSelectionChanged(QModelIndex new_index, QModelIndex);

  // slot raised when the New (prior) button is clicked
  void on_btn_new_clicked();

  // slot raised when the delete (prior) button is clicked
  void on_btn_delete_clicked();

  // slot raised when the Edit (prior) button is clicked
  void on_btn_edit_clicked();

  // slot raised when the Save (prior) button is clicked
  void on_btn_save_clicked();

  // slot raised when the Cancel Edit (prior) button is clicked
  void on_btn_cancel_clicked();

  //// Details edition of the Luminosity prior related slots
  // slot raised when the MAGNITUDE/FLUX combobox is changed
  void on_cb_unit_currentIndexChanged(const QString &);

  // slot raised when the browse filter button is clicked
  void on_btn_filter_clicked();

  // slot raised when the filter selector popup is closing
  void filterPopupClosing(std::string filter);

  //// Luminosity Function grid related slots
  // slot raised when the groups management button is clicked
  void on_btn_group_clicked();

  // slot raised when the groups management popup is closing
  void groupPopupClosing(std::vector<PhzLuminosity::SedGroup> groups);

  // slot raised when the redshifts management button is clicked
  void on_btn_z_clicked();

  // slot raised when the redshifts management popup is closing
  void zPopupClosing(std::vector<double> zs);

  // slot raised when aluminosity function define button is clicked
  void onGridButtonClicked(size_t x,size_t y);

  // slot raised when the Luminosity function popup is closing
  void luminosityFunctionPopupClosing(LuminosityFunctionInfo info, size_t x, size_t y);

  //// Global
  // closing the popup
  void on_btn_close_clicked();

private:
  void manageBtnEnability(bool in_edition, bool read_only);

  void manageBtnEnability(bool in_edition, bool read_only, bool has_selected_row);

  void loadMainGrid();

  void clearGrid();

  void loadGrid();



  bool validateInput(const size_t& current_index) ;
  void updateInfo(LuminosityPriorConfig& info);
  void updatePriorRow(QModelIndex& index,const size_t& row, const LuminosityPriorConfig& info );
  void computeModelGrid(const LuminosityPriorConfig& info );

  std::unique_ptr<Ui::DialogLuminosityPrior> ui;

  QString m_config_folder;

  QString m_grid_folder;

  ModelSet m_model;

  std::string m_survey_name;

  double m_z_min;

  double m_z_max;

  std::map<std::string, LuminosityPriorConfig> m_prior_configs;

  bool m_new = false;

  // currently select Prior informations (needed for the Lum. Function Grid)
  std::vector<std::vector<LuminosityFunctionInfo>> m_luminosityInfos { };
  std::vector<PhzLuminosity::SedGroup> m_groups { };
  std::vector<double> m_zs { };
  std::vector<GridButton*> m_grid_buttons { };
};
}
}

#endif /* DIALOGLUMINOSITYPRIOR_H_*/
