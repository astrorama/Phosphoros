/*
 * DialogGridGeneration.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: fdubath
 */

#include <QDir>
#include <QMessageBox>
#include "PhzQtUI/DialogGridGeneration.h"
#include "ui_DialogGridGeneration.h"
#include "PhzConfiguration/CreatePhotometryGridConfiguration.h"
#include "PhzModeling/PhotometryGridCreator.h"

// #include <future>

namespace Euclid {
namespace PhzQtUI {

DialogGridGeneration::DialogGridGeneration(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogGridGeneration)
    {
        ui->setupUi(this);
        ui->progressBar->setValue(0);
        ui->btn_cancel->setEnabled(false);

        m_timer.reset(new QTimer(this));
        m_timer->setInterval(100);
        m_timer->setSingleShot(true);
        connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(run()));
        m_timer->start();
    }

DialogGridGeneration::~DialogGridGeneration() {}

void DialogGridGeneration::setValues(std::string grid_name,
    const std::map<std::string, boost::program_options::variable_value>& config) {
    ui->label_name->setText(QString::fromStdString(grid_name));
    m_config=config;

}

void DialogGridGeneration::updateGridProgressBar(size_t step, size_t total) {
  int value = (step * 100) / total;
  ui->progressBar->setValue(value);
}


void DialogGridGeneration::run(){
  try{
         PhzConfiguration::CreatePhotometryGridConfiguration conf { m_config };
         PhzModeling::PhotometryGridCreator creator { conf.getSedDatasetProvider(),
             conf.getReddeningDatasetProvider(), conf.getFilterDatasetProvider() };

         auto param_space = PhzDataModel::createAxesTuple(conf.getZList(),
             conf.getEbvList(), conf.getReddeningCurveList(), conf.getSedList());

         std::function<void(size_t,size_t)> monitor_function = std::bind(&DialogGridGeneration::updateGridProgressBar, this, std::placeholders::_1, std::placeholders::_2);

         auto grid = creator.createGrid(param_space, conf.getFilterList(),monitor_function);

//         auto run_function = std::bind(&PhzModeling::PhotometryGridCreator::createGrid, creator, param_space, conf.getFilterList(),monitor_function);
//
//        // std::future<std::map<std::string, boost::program_options::variable_value>>
//         auto the_grid = std::async(std::launch::async,run_function);

         auto output = conf.getOutputFunction();
         output(grid);
         this->accept();
         return;
         }
        catch(const Elements::Exception & e){
          QMessageBox::warning(this, "Error in the computation...",
                               QString::fromStdString("Sorry, an error occurred during the computation: "+std::string(e.what())),
                               QMessageBox::Close);

        }
        catch(const std::exception& e){
           QMessageBox::warning(this, "Error in the computation...",
               QString::fromStdString("Sorry, an error occurred during the computation: "+std::string(e.what())),
                                QMessageBox::Close);

         }
         catch(...){
           QMessageBox::warning(this, "Error in the computation...",
                             "Sorry, an error occurred during the computation.",
                             QMessageBox::Close);
         }
         this->reject();
}

}
}

