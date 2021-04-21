#ifndef DIALOGSEDPARAM_H
#define DIALOGSEDPARAM_H

#include <set>
#include <string>
#include <vector>
#include <memory>
#include <QDialog>
#include <QProcess>
#include "FilterMapping.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzQtUI {


typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class DialogSedParam;
}

/**
 * @class DialogSedParam
 */
class DialogSedParam: public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogSedParam(DatasetRepo sed_repository,  QWidget *parent = 0);

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
   * @brief SLOT on_btn_new_clicked: Add a parameter
   */
  void on_btn_new_clicked();


  void processingFinished(int, QProcess::ExitStatus);


  void delParamClicked(const QString& param);

  /**
   * @brief SLOT on_btn_cancel_clicked: close the popup
   */
  void on_btn_cancel_clicked();

  /**
   * @brief SLOT on_btn_save_clicked: save
   * and close the popup.
   */
  void on_btn_save_clicked();

  std::string getFile(const XYDataset::QualifiedName& sed) const;

private:
  std::unique_ptr<Ui::DialogSedParam> ui;
  DatasetRepo m_sed_repository;
  QProcess *m_P;
  std::string m_file_path = "";
};

}
}

#endif // DIALOGSEDPARAM_H
