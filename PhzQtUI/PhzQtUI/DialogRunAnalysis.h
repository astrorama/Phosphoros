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
#include <QTimer>
#include <boost/program_options.hpp>
#include "ElementsKernel/Exception.h"

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
    explicit DialogRunAnalysis(QWidget *parent = 0);
    ~DialogRunAnalysis();

    /**
     * @brief Initialise the popup by setting its internal data
     */
    void setValues(std::string output_cat_name,std::string output_pdf_name,const std::map<std::string, boost::program_options::variable_value>& config);


private slots:

void run();

private:
    std::map<std::string, boost::program_options::variable_value> m_config;
    std::unique_ptr<Ui::DialogRunAnalysis> ui;
    std::unique_ptr<QTimer> m_timer;

    void updateProgressBar(size_t step, size_t total);
};

}
}



#endif /* DIALOGRUNANALYSIS_H_*/
