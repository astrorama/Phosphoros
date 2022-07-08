/*
 * DialogLuminositySedGroup.h
 *
 *  Created on: September 4, 2015
 *      Author: fdubath
 */

#ifndef DIALOGLUMINOSITYSEDGROUP_H_
#define DIALOGLUMINOSITYSEDGROUP_H_

#include "ElementsKernel/Exception.h"
#include "PhzQtUI/LuminosityPriorConfig.h"
#include <QDialog>
#include <QVBoxLayout>
#include <map>
#include <memory>
#include <vector>

namespace boost {
namespace program_options {
class variable_value;
}
}  // namespace boost

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogLuminositySedGroup;
}

/**
 * @class DialogLuminositySedGroup
 * @brief This popup allows to define and manage the SED groups.
 */
class DialogLuminositySedGroup : public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogLuminositySedGroup(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogLuminositySedGroup();

  /**
   * @brief Set the existing groups to the popup.
   * @param groups A vector of SedGroup
   */
  void setGroups(std::vector<LuminosityPriorConfig::SedGroup> groups);

  /**
   * @param missing_seds
   * SEDs not present in the parameter space to be removed
   *
   * @param new_seds
   * SEDs not present in the config to be added
   */
  void setDiff(std::vector<std::string> missing_seds, std::vector<std::string> new_seds);

signals:
  /**
   * @brief SIGNAL popupClosing Called when the popup is closing
   * @param groups A vector of SedGroup containings the new groups
   */
  void popupClosing(std::vector<LuminosityPriorConfig::SedGroup> groups);

private slots:
  /**
   * @brief SLOT raised when the Cancel button is clicked.
   */
  void on_btn_cancel_clicked();

  /**
   * @brief SLOT raised when the Save button is clicked.
   */
  void on_btn_save_clicked();

  /**
   * @brief SLOT raised when the Add group button is clicked.
   */
  void on_btn_add_clicked();

  /**
   * @brief SLOT raised when the delete group button is clicked.
   */
  void onDeleteGroupClicked(size_t sed_group_id, size_t);

private:
  void                                          readNewGroups();
  void                                          addGroup(LuminosityPriorConfig::SedGroup group, size_t i, size_t i_max);
  std::unique_ptr<Ui::DialogLuminositySedGroup> ui;
  std::vector<LuminosityPriorConfig::SedGroup>  m_groups;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif /* DIALOGLUMINOSITYSEDGROUP_H_*/
