/*
 * DialogRunAnalysis.h
 *
 *  Created on: Mar 6, 2015
 *      Author: fdubath
 */

#ifndef DIALOGRUNANALYSIS_H_
#define DIALOGRUNANALYSIS_H_

#include "ElementsKernel/Exception.h"
#include <QDialog>
#include <QFutureWatcher>
#include <QTimer>
#include <map>
#include <memory>

namespace boost {
namespace program_options {
class variable_value;
}
}  // namespace boost

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogRunAnalysis;
}

/**
 * @brief The DialogRunAnalysis class.
 * This popup show the progress-bar during analysis run.
 */
class DialogRunAnalysis : public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogRunAnalysis(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogRunAnalysis();

  /**
   * @brief Initialise the popup by setting its internal data
   */
  void setValues(std::string output_dir, const std::map<std::string, boost::program_options::variable_value>& config,
                 const std::map<std::string, boost::program_options::variable_value>& sed_config);

private slots:

  std::string runFunction();
  std::string runSedFunction();

  void run();

  void runFinished();
  void sedFinished();

  void on_btn_cancel_clicked();

signals:

  void signalUpdateBar(int);
  void signalUpdateSedBar(int);

private:
  bool needLuminosityGrid();
  bool needSedWeights();

  QFutureWatcher<std::string>                                   m_future_watcher{};
  QFutureWatcher<std::string>                                   m_future_sed_watcher{};
  std::map<std::string, boost::program_options::variable_value> m_config;
  std::map<std::string, boost::program_options::variable_value> m_original_config;
  std::map<std::string, boost::program_options::variable_value> m_sed_config;
  std::unique_ptr<Ui::DialogRunAnalysis>                        ui;
  std::unique_ptr<QTimer>                                       m_timer;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif /* DIALOGRUNANALYSIS_H_*/
