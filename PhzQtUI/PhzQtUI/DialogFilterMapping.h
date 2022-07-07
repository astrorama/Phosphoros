#ifndef DIALOGFILTERMAPPING_H
#define DIALOGFILTERMAPPING_H

#include "FilterMapping.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include <QDialog>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class DialogFilterMapping;
}

/**
 * @class DialogFilterMapping
 * @brief This popup allows the user to select the filters to be mapped
 * in the calling form. It instanciate and access directly the filters provider.
 */
class DialogFilterMapping : public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogFilterMapping(DatasetRepo filter_repository, QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogFilterMapping();

  /**
   * @brief Initialise the popup by setting its internal data
   * @param filters
   * A vector of string containing the names of the selected filters at the
   * opening of the popup.
   */
  void setFilters(const std::vector<std::string>& filters);

signals:
  /**
   * @brief SIGNAL popupClosing: rised when the FilterMapping is saved and
   * the popup close.
   * The argument is the vector of selected filters.
   */
  void popupClosing(std::vector<std::string>);

private slots:
  /**
   * @brief SLOT on_btn_cancel_clicked: close the popup
   */
  void on_btn_cancel_clicked();

  /**
   * @brief SLOT on_btn_save_clicked: call the SIGNAL popupClosing
   * and close the popup.
   */
  void on_btn_save_clicked();

private:
  std::unique_ptr<Ui::DialogFilterMapping> ui;
  DatasetRepo                              m_filter_repository;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DIALOGFILTERMAPPING_H
