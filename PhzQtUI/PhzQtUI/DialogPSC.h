#ifndef DIALOGPSC_H
#define DIALOGPSC_H

#include <memory>
#include <list>
#include <QDialog>
#include <string>
#include <vector>
#include <QProcess>
#include <QTimer>
#include <QString>

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogPSC;
}


class DialogPSC: public QDialog {
  Q_OBJECT
public:

  /**
   * @brief Constructor
   */
  explicit DialogPSC(QWidget *parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogPSC();


  void setFolder(std::string output_folder);
  void setDefaultColumn(std::string id_column, std::string zref_column);

private slots:

  /**
   * @brief SLOT on_btn_create_clicked
   */
  void on_btn_compute_clicked();

  void on_gb_scater_clicked(bool);
  void on_gb_stacked_clicked(bool);

  /**
   * @brief SLOT on_btn_cancel_clicked
   */
  void on_btn_cancel_clicked();
  void on_btn_close_clicked();
  void on_btn_browse_clicked();

  void processingFinished(int, QProcess::ExitStatus);

  void updateOutCons();

  void checkComputePossible();

private:
  std::unique_ptr<Ui::DialogPSC> ui;
  std::string m_folder;
  std::string m_catalog;
  std::string m_id_column;
  std::string m_z_ref_column;

  const std::list<std::string>  m_list_columns_ok = {"MEDIAN", "PHZ_MODE_1_SAMP",
      "PHZ_MODE_1_MEAN", "PHZ_MODE_1_FIT", "PHZ_MODE_2_SAMP", "PHZ_MODE_2_MEAN", "PHZ_MODE_2_FIT"};

  QProcess *m_P;
  QTimer *m_timer;
  bool m_processing=false;
  void setCatalogFile(QString path);


};

}
}

#endif // DIALOGPSC_H
