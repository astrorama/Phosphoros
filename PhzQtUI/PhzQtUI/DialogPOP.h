#ifndef DIALOGPOP_H
#define DIALOGPOP_H

#include <memory>
#include <QDialog>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogPOP;
}


class DialogPOP: public QDialog {
  Q_OBJECT
public:

  /**
   * @brief Constructor
   */
  explicit DialogPOP(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogPOP();


  void setFolder(std::string output_folder);


private slots:

  /**
   * @brief SLOT on_btn_create_clicked
   */
  void on_btn_compute_clicked();

  /**
   * @brief SLOT on_btn_cancel_clicked
   */
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogPOP> ui;
  std::string m_folder;

};

}
}

#endif // DIALOGPOP_H
