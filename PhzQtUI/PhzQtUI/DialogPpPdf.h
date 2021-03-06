#ifndef DialogPpPdf_H
#define DialogPpPdf_H

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
class DialogPpPdf;
}

class DialogPpPdf : public QDialog {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   */
  explicit DialogPpPdf(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogPpPdf();

  void setFolder(std::string result_folder);

private slots:

  /**
   * @brief
   */
  void updateOutCons();
  void processingFinished(int, QProcess::ExitStatus);
  void on_btn_cancel_clicked();
  void on_btn_save_clicked();

private:
  std::unique_ptr<Ui::DialogPpPdf> ui;
  std::string                      m_result_folder = "";
  QProcess*                        m_P;
  QTimer*                          m_timer;
  bool                             m_configured = false;
  bool                             m_processing = false;
  std::vector<std::string>         m_pps{};
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DialogPpPdf_H
