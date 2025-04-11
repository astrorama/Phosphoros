#ifndef DIALOGINTERPOLATESED_H
#define DIALOGINTERPOLATESED_H

#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include <QDialog>
#include <QFrame>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <memory>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogInterpolateSed;
}

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;
/**
 * @class DialogInterpolateSed

 */
class DialogInterpolateSed : public QDialog {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   */
  explicit DialogInterpolateSed(DatasetRepo sed_repo, DatasetRepo filter_repo, QString solar_sed, QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogInterpolateSed();

private slots:

  void on_btn_plus_clicked();
  void on_btn_rma_clicked();
  void on_btn_filterSelection_clicked();
  void sedPopupClosing(std::vector<std::string>);

  void onDelButtonClicked(const QString&);
  void setLumFilter(std::string );

  /**
   * @brief SLOT on_btn_create_clicked
   */
  void on_btn_create_clicked();

  /**
   * @brief SLOT on_btn_cancel_clicked
   */
  void on_btn_cancel_clicked();

  void processingFinished(int, QProcess::ExitStatus);

private:
  std::unique_ptr<Ui::DialogInterpolateSed> ui;

  QFrame*     createControls(bool first, bool del, std::string sed);
  DatasetRepo m_seds_repository;
  DatasetRepo m_filter_repo;
  QStringList m_sed_list{};
  QProcess*   m_is = nullptr;
  QString     m_solar_sed;
  QString     m_ref_filter;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DIALOGINTERPOLATESED_H
