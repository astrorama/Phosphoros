#ifndef DIALOGSEDPARAM_H
#define DIALOGSEDPARAM_H

#include "FilterMapping.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/XYDatasetProvider.h"
#include "XYDataset/QualifiedName.h"
#include <QDialog>
#include <QProcess>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::XYDatasetProvider>>> DatasetRepo;

namespace Ui {
class DialogSedParam;
}

/**
 * @class DialogSedParam
 */
class DialogSedParam : public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogSedParam(DatasetRepo sed_repository, QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogSedParam();

  /**
   * @brief Initialise the popup by setting its internal data
   */
  void setSed(const XYDataset::QualifiedName& sed);

private slots:
  /**
   * @brief SLOT on_btn_cancel_clicked: close the popup
   */
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogSedParam> ui;
  DatasetRepo                         m_sed_repository;
  std::string                         m_file_path = "";
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DIALOGSEDPARAM_H
