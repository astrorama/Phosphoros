/*
 * DialogRunAnalysis.h
 *
 *  Created on: Mar 6, 2015
 *      Author: fdubath
 */

#ifndef DIALOGRUNANALYSIS_H_
#define DIALOGRUNANALYSIS_H_

#include <memory>
#include <QDialog>
#include <QFutureWatcher>
#include <QTimer>
#include <map>
#include "ElementsKernel/Exception.h"

namespace boost{
namespace program_options{
 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogRunAnalysis;
}

/**
 * @brief The DialogRunAnalysis class.
 * This popup show the progress-bar during analysis run.
 */
class DialogRunAnalysis : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    explicit DialogRunAnalysis(QWidget *parent = 0);

    /**
     * @brief Destructor
     */
    ~DialogRunAnalysis();

    /**
     * @brief Initialise the popup by setting its internal data
     */
    void setValues(std::string output_cat_name,
                   std::string output_pdf_name,
                   std::string output_lik_name,
                   std::string output_pos_name,
                   const std::map<std::string, boost::program_options::variable_value>& config,
                   const std::map<std::string, boost::program_options::variable_value>& luminosity_config);
  
private slots:

  std::string runFunction();

  void run();

  void runFinished();

  void on_btn_cancel_clicked();

signals:

  void signalUpdateBar(int);

private:

  QFutureWatcher<std::string> m_future_watcher {};
  std::map<std::string, boost::program_options::variable_value> m_config;
  std::map<std::string, boost::program_options::variable_value> m_lum_config;
  std::unique_ptr<Ui::DialogRunAnalysis> ui;
  std::unique_ptr<QTimer> m_timer;
  

    bool checkLuminosityGrid();
    void computeLuminosityGrid();
};

}
}



#endif /* DIALOGRUNANALYSIS_H_*/
