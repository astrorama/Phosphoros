#ifndef DialogExtractZ_H
#define DialogExtractZ_H

#include <QDialog>
#include <QProcess>
#include <QString>
#include <QTimer>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include "ElementsKernel/Temporary.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogExtractZ;
}

class DialogExtractZ : public QDialog {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   */
  explicit DialogExtractZ(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogExtractZ();

  int exec() override;

  void setData(std::string input_cat, std::string col, int start, int max_num, double tol, bool do_scale);
  std::list<float> getRedshiftList();

private slots:

  /**
   * @brief
   */
  void updateOutCons();
  void processingFinished(int, QProcess::ExitStatus);
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogExtractZ> ui;
  std::string                      m_input_cat;
  std::string                      m_col;
  int                              m_start;
  int                              m_max_num;
  double                           m_tol;
  bool                             m_do_scale;
  QProcess*                        m_P;
  QTimer*                          m_timer;
  std::list<float>                 m_zs{};
  Elements::TempDir                m_temp_dir;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DialogExtractZ_H
