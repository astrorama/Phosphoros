#ifndef DialogResid_H
#define DialogResid_H

#include <QDialog>
#include <QProcess>
#include <QString>
#include <QTimer>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogResid;
}

class DialogResid : public QDialog {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   */
  explicit DialogResid(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogResid();

  void setFolder(std::string output_folder);

private slots:

  /**
   * @brief SLOT on_btn_create_clicked
   */
  void on_btn_compute_clicked();
  void on_btn_cancel_clicked();
  void on_btn_close_clicked();

  void processingFinished(int, QProcess::ExitStatus);

  void updateOutCons();

  void checkComputePossible();

private:
  std::unique_ptr<Ui::DialogResid> ui;
  std::string                      m_folder;

  QProcess* m_P;
  QTimer*   m_timer;
  bool      m_processing = false;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DialogResid_H
