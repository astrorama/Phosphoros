#ifndef DIALOGRANGE_H
#define DIALOGRANGE_H

#include <memory>
#include <map>
#include <set>
#include <vector>
#include <QDialog>
#include <QProcess>
#include <QItemSelection>
#include <QVBoxLayout>

#include "ParameterRule.h"
#include "PhzQtUI/GridButton.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzQtUI/SedTreeModel.h"
#include "PhzQtUI/MessageButton.h"
#include "PhzQtUI/Range.h"
namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogRange;
}

typedef std::shared_ptr<
    PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>>DatasetRepo;

/**
 * @class DialogRange
 * @brief This popup allows the user to view/edit a Parameter Rules of a Model Set.
 */
class DialogRange: public QDialog {
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
  explicit DialogRange(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogRange();

  void setData(std::vector<Range> ranges, std::set<double> values, bool redshift);





  signals:
  void popupClosing(std::vector<Range>, std::set<double>);


  private slots:
  /**
   * @brief SLOT raised when the user delete a  range
   */
  void onDeleteClicked(size_t, size_t);

  /**
   * @brief SLOT raised when the user add a range
   */
  void on_btn_add_range_clicked();

  /**
   * @brief SLOT on_buttonBox_rejected: The user close the popup
   */
  void on_buttonBox_rejected();

  /**
   * @brief SLOT on_buttonBox_accepted: The user close the popup,
   * and rise the SIGNAL popupClosing
   */
  void on_buttonBox_accepted();


  void on_rb_val_clicked();

  void on_rb_range_clicked();


private:
  std::unique_ptr<Ui::DialogRange> ui;
  size_t m_current_range_id = 0;
  bool m_is_redshift;
  std::vector<Range> m_ranges;
  std::set<double> m_values;

  QFrame* createRangeControls(GridButton* del_button, int range_id,
      bool enabled);
  QFrame* createRangeControls(GridButton* del_button, int range_id,
      bool enabled, const Range& range);

  void populateRangesAndValues();
  void cleanRangeControl(QVBoxLayout* ranges_layout);
  void SetRangeControlsEnabled(QVBoxLayout* ranges_layout, bool is_enabled);
  void deleteRangeAt(QVBoxLayout* ranges_layout, size_t range_id);
  std::vector<Range> getRanges(QVBoxLayout* ranges_layout);

};

}
}

#endif // DIALOGRANGE_H
