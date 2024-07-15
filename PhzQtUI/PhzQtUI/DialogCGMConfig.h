#ifndef DIALOGCMCONFIG_H
#define DIALOGCMCONFIG_H

#include <QDialog>
#include <memory>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogCGMConfig;
}

/**
 * @class DialogCGMConfig
 * @brief This popup allows the user to provide the parameters for configuring the IGM CGM.
 */
class DialogCGMConfig : public QDialog {
  Q_OBJECT
public:
  /**
   * @brief Constructor
   */
  explicit DialogCGMConfig(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogCGMConfig();

  /**
   * @brief Set a proposed name at the opening of the popup.
   * @param default_name The name to be proposed to the user.
   */
  void setValues(double A, double a, double c);

  double get_A_param() const;
  double get_a_param() const;
  double get_c_param() const;


private slots:

  void on_pb_def_A_clicked();
  void on_pb_def_a_clicked();
  void on_pb_def_c_clicked();

  /**
   * @brief SLOT on_btn_create_clicked
   */
  void on_btn_save_clicked();

  /**
   * @brief SLOT on_btn_cancel_clicked
   */
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogCGMConfig> ui;
  double m_A;
  double m_a;
  double m_c;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // DIALOGCMCONFIG_H
