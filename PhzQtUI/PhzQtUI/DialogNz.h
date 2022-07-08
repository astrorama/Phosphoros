#ifndef DIALOGNZ_H
#define DIALOGNZ_H

#include "FilterMapping.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include <QDialog>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class DialogNz;
}

/**
 * @class DialogNz
 * @brief This popup allows the user to select the N(z) configuration.
 */
class DialogNz : public QDialog {
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
  explicit DialogNz(std::vector<FilterMapping> filters, DatasetRepo filter_repository, std::string b_filter,
                    std::string i_filter, QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogNz();

signals:
  /**
   * @brief SIGNAL popupClosing: rised when the the popup close.
   * The argument are the modifed  filters.
   */
  void popupClosing(std::string, std::string);

private slots:
  void filterPopupClosing(std::string filter);
  void on_btn_select_filter_clicked();
  void on_btn_save_clicked();
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogNz> ui;
  DatasetRepo                   m_filter_repository;
  std::vector<FilterMapping>&   m_filters;
  std::string                   m_b_filter;
  std::string                   m_i_filter;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DIALOGNZ_H
