#ifndef DialogLuminosityFunctionCurveSelector_H
#define DialogLuminosityFunctionCurveSelector_H

#include "FilterMapping.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include <QDialog>
#include <memory>
#include <set>

namespace Euclid {
namespace PhzQtUI {
typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class DialogLuminosityFunctionCurveSelector;
}

/**
 * @class DialogLuminosityFunctionCurveSelector
 * @brief This popup allows the user to select a luminosity function curve.
 */
class DialogLuminosityFunctionCurveSelector : public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogLuminosityFunctionCurveSelector(DatasetRepo luminosity_repository, QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogLuminosityFunctionCurveSelector();

  /**
   * @brief set the curve name at the opening of the popup.
   * @parame curve_name The name of the curve displayed when the popup opens.
   */
  void setCurve(std::string curve_name);

signals:
  /**
   * @brief SIGNAL popupClosing: rised when the popup is closed.
   * The argument is the curve Name.
   */
  void popupClosing(std::string);

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
  std::unique_ptr<Ui::DialogLuminosityFunctionCurveSelector> ui;
  DatasetRepo                                                m_luminosity_repository;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DialogLuminosityFunctionCurveSelector_H
