/*
 * DialogGridGeneration.h
 *
 *  Created on: 2021/09/15
 *      Author: fdubath
 */

#ifndef DIALOG_FILTER_SHIFT_GRIDGENERATION_H_
#define DIALOG_FILTER_SHIFT_GRIDGENERATION_H_

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
class DialogFilterShiftGridGeneration;
}

/**
 * @class DialogFilterShiftGridGeneration
 * @brief This popup show the progress-bar during grid computation.
 */
class DialogFilterShiftGridGeneration : public QDialog {
  Q_OBJECT

public:
  /**
   * @brief Constructor
   */
  explicit DialogFilterShiftGridGeneration(QWidget* parent = 0);

  /**
   * @brief Destructor
   */
  ~DialogFilterShiftGridGeneration();

  /**
   * @brief Initialise the popup by setting its internal data
   * @param grid_name The name of the grid being computed
   * @param config The configuration of the engine which is in charge of
   * computing the grid.
   */
  void setValues(std::string grid_name, const std::map<std::string, boost::program_options::variable_value>& config);

private slots:

  /**
   * @brief Start the (asynchronous) computation
   */
  void run();

  /**
   * @brief Callback at the end of the computation
   */
  void runFinished();

  /**
   * @brief Function to be provider to the worker thread
   */
  std::string runFunction();

  /**
   * @brief Cancel the computation.
   */
  void on_btn_cancel_clicked();

signals:

  /**
   * @brief SIGNAL Update the progress bar.
   */
  void signalUpdateBar(int);

private:
  QFutureWatcher<std::string>                                   m_future_watcher{};
  std::map<std::string, boost::program_options::variable_value> m_config;
  std::unique_ptr<Ui::DialogFilterShiftGridGeneration>          ui;
  std::unique_ptr<QTimer>                                       m_timer;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif /* DIALOG_FILTER_SHIFT_GRIDGENERATION_H_ */
