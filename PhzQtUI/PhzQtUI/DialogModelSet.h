#ifndef DIALOGMODELSET_H
#define DIALOGMODELSET_H

#include <QDialog>
#include <QItemSelection>
#include <QProcess>
#include <QVBoxLayout>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "ParameterRule.h"
#include "PhzQtUI/DatasetRepository.h"
#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/MessageButton.h"
#include "PhzQtUI/SedTreeModel.h"
#include "XYDataset/FileSystemProvider.h"
namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogModelSet;
}

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

/**
 * @class DialogModelSet
 * @brief This popup allows the user to view/edit a Parameter Rules of a Model Set.
 */
class DialogModelSet : public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param seds_repository a pointer on  DatasetRepository<FileSystemProvider>
   * storing the available SEDs
   * @param redenig_curves_repository a pointer on  DatasetRepository<FileSystemProvider>
   * storing the available Reddening Curves
   *
   */
  explicit DialogModelSet(DatasetRepo seds_repository, DatasetRepo redenig_curves_repository, QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogModelSet();

  /**
   * @brief Initialise the popup by setting its internal data
   * @param init_parameter_rules
   * the map of ParameterRules to be displayed/modified
   */
  void loadData(int ref, const std::map<int, ParameterRule>& init_parameter_rules, std::vector<Range> z_ranges,
                std::set<double> z_values, std::vector<Range> ebv_ranges, std::set<double> ebv_values);

  /**
   * @brief When called, set the popup in read-only.
   */
  void setViewMode();

signals:
  /**
   * @brief SIGNAL popupClosing: rised when the the popup close.
   * The argument is the modifed  ParameterRule.
   */
  void popupClosing(int, ParameterRule, bool);

private slots:
  void sedProcessStarted();
  void sedProcessfinished(int, QProcess::ExitStatus);

  void addEmissionLineButtonClicked(const QString&);
  /**
   * @brief SLOT on_buttonBox_rejected: The user close the popup
   */
  void on_buttonBox_rejected();

  /**
   * @brief SLOT on_buttonBox_accepted: The user close the popup,
   * and rise the SIGNAL popupClosing
   */

  void on_buttonBox_accepted();

private:
  void loadSeds();
  void turnControlsInEdition();
  void turnControlsInView();

  std::unique_ptr<Ui::DialogModelSet> ui;
  bool                                m_view_popup = false;

  std::vector<Range> m_z_ranges;
  std::set<double>   m_z_values;
  std::vector<Range> m_ebv_ranges;
  std::set<double>   m_ebv_values;

  DatasetRepo m_seds_repository;
  DatasetRepo m_redenig_curves_repository;

  int                          m_ref;
  std::map<int, ParameterRule> m_rules;
  std::vector<MessageButton*>  m_message_buttons;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DIALOGMODELSET_H
