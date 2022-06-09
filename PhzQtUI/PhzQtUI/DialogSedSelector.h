#ifndef DialogSedSelector_H
#define DialogSedSelector_H

#include <set>
#include <vector>
#include <memory>
#include <QDialog>
#include "FilterMapping.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class DialogSedSelector;
}

/**
 * @class DialogSedSelector
 * @brief This popup allows the user to select a single SED.
 */
class DialogSedSelector: public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogSedSelector(DatasetRepo filter_repository, bool single_sed = true, QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogSedSelector();

  /**
   * @brief Set the initial sed value
   * @param sed_name Name of the initially selected sed.
   */
  void setSed(std::string sed_names);

  signals:
  /**
   * @brief SIGNAL popupClosing: rised when the popup is closed.
   * The argument is the selected SED Name.
   */
  void popupClosing(std::vector<std::string>);

private slots:
  /**
   * @brief SLOT on_btn_cancel_clicked: close the popup
   */
  void on_btn_cancel_clicked();

  /**
   * @brief SLOT on_btn_save_clicked: rises the
   *  SIGNAL popupClosing and close the popup.
   */
  void on_btn_save_clicked();

private:
  std::unique_ptr<Ui::DialogSedSelector> ui;
  DatasetRepo m_sed_repository;
  bool m_single_sed;
};

}
}

#endif // DialogSedSelector_H
