#ifndef DIALOGNZ_H
#define DIALOGNZ_H

#include <memory>
#include <map>
#include <vector>
#include <QDialog>
#include "FilterMapping.h"


namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogNz;
}


/**
 * @class DialogNz
 * @brief This popup allows the user to select the N(z) configuration.
 */
class DialogNz: public QDialog {
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
  explicit DialogNz(std::vector<FilterMapping> filters, QWidget *parent = 0);

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

  void on_btn_save_clicked();
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogNz> ui;
  std::vector<FilterMapping>& m_filters;

};

}
}

#endif // DIALOGNZ_H
