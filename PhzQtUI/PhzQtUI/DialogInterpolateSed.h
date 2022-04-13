#ifndef DIALOGINTERPOLATESED_H
#define DIALOGINTERPOLATESED_H

#include <memory>
#include <QDialog>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <string>
#include <vector>
#include <QFrame>
#include <QVBoxLayout>
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogInterpolateSed;
}

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;
/**
 * @class DialogInterpolateSed

 */
class DialogInterpolateSed: public QDialog {
  Q_OBJECT
public:

  /**
   * @brief Constructor
   */
  explicit DialogInterpolateSed(DatasetRepo sed_repo, QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogInterpolateSed();





private slots:

  void on_btn_plus_clicked();
  void on_btn_rma_clicked();
  void sedPopupClosing(std::vector<std::string>);


  void onDelButtonClicked(const QString&);

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

  QFrame* createControls(bool first, bool del, std::string sed);
  DatasetRepo m_seds_repository;
  QStringList m_sed_list{};
  QProcess * m_is = nullptr;

};

}
}

#endif // DIALOGINTERPOLATESED_H
