#ifndef DialogAbsMag_H
#define DialogAbsMag_H

#include <QDialog>
#include <QString>
#include <string>
#include <list>
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"

namespace Euclid {
namespace PhzQtUI {

typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;


namespace Ui {
class DialogAbsMag;
}

class DialogAbsMag : public QDialog {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   */
  explicit DialogAbsMag(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogAbsMag();

  void setData(DatasetRepo filter_repository, std::list<std::pair<QString, QString>> abs_mag_config);
  std::list<std::pair<QString, QString>> getAbsMagConfig();
  
private slots:
  void filterPopupClosing(std::string filter);

  void on_btn_SelectFilter_clicked();
  
  void deletButtonClicked(const QString& filter);
  
  void on_btn_cancel_clicked();
  
  void on_btn_save_clicked();

private:
  std::unique_ptr<Ui::DialogAbsMag>      ui;
  DatasetRepo                            m_filter_repository;
  std::list<std::pair<QString, QString>> m_abs_mag_config {};
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DialogAbsMag_H
