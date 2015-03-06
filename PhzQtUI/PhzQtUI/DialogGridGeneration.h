/*
 * DialogGridGeneration.h
 *
 *  Created on: Mar 6, 2015
 *      Author: fdubath
 */

#ifndef DIALOGGRIDGENERATION_H_
#define DIALOGGRIDGENERATION_H_

#include <memory>
#include <QDialog>
#include <QTimer>
#include <boost/program_options.hpp>
#include "ElementsKernel/Exception.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogGridGeneration;
}

/**
 * @brief The DialogGridGeneration class.
 * This popup show the progress-bar during grid computation.
 */
class DialogGridGeneration : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGridGeneration(QWidget *parent = 0);
    ~DialogGridGeneration();

    /**
     * @brief Initialise the popup by setting its internal data
     */
    void setValues(std::string grid_name,const std::map<std::string, boost::program_options::variable_value>& config);


private slots:

void run();

private:
    std::map<std::string, boost::program_options::variable_value> m_config;
    std::unique_ptr<Ui::DialogGridGeneration> ui;
    std::unique_ptr<QTimer> m_timer;

    void updateGridProgressBar(size_t step, size_t total);
};

}
}



#endif /* DIALOGGRIDGENERATION_H_ */
