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

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogInterpolateSed;
}

/**
 * @class DialogInterpolateSed

 */
class DialogInterpolateSed: public QDialog {
  Q_OBJECT
public:

  /**
   * @brief Constructor
   */
  explicit DialogInterpolateSed(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogInterpolateSed();





private slots:

  void on_btn_plus_clicked();
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

  QFrame* createControls(bool first, bool del);

  QStringList m_sed_list{};
  QProcess * m_is = nullptr;

};

}
}

#endif // DIALOGINTERPOLATESED_H
