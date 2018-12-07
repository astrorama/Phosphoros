#ifndef DIALOGADDGALEBV_H
#define DIALOGADDGALEBV_H

#include <memory>
#include <QDialog>
#include <QProcess>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogAddGalEbv;
}

/**
 * @class DialogAddGalEbv

 */
class DialogAddGalEbv: public QDialog {
  Q_OBJECT
public:

  /**
   * @brief Constructor
   */
  explicit DialogAddGalEbv(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogAddGalEbv();

  /**
   * @brief Set a proposed name at the opening of the popup.
   * @param default_name The name to be proposed to the user.
   */
  void setDefaultName(std::string default_name);

  void setInputs(std::string input_name, std::string ra_col, std::string dec_col);

  /**
   * @brief Get the name chosen by the user.
   * @return The new catalog name.
   */
  std::string getOutputName() const;

private slots:

  void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void readError();
  void readOutput();
  void getError(QProcess::ProcessError error);

  /**
   * @brief SLOT on_btn_create_clicked
   */
  void on_btn_create_clicked();

  /**
   * @brief SLOT on_btn_cancel_clicked
   */
  void on_btn_cancel_clicked();

private:
  std::unique_ptr<Ui::DialogAddGalEbv> ui;
  std::string m_input_name;
  std::string m_ra_col;
  std::string m_dec_col;
  std::string m_name;
  QProcess* m_process = new QProcess(this);

};

}
}

#endif // DIALOGADDGALEBV_H
