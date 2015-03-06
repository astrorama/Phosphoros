/*
 * DialogGridGeneration.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: fdubath
 */

#include <QDir>
#include <QMessageBox>
#include "PhzQtUI/DialogRunAnalysis.h"
#include "ui_DialogRunAnalysis.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "PhzConfiguration/CreatePhzCatalogConfiguration.h"

// #include <future>

namespace Euclid {
namespace PhzQtUI {

DialogRunAnalysis::DialogRunAnalysis(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogRunAnalysis)
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

DialogRunAnalysis::~DialogRunAnalysis() {}

void DialogRunAnalysis::setValues(std::string output_cat_name,std::string output_pdf_name,
    const std::map<std::string, boost::program_options::variable_value>& config) {
    ui->label_name_cat->setText(QString::fromStdString(output_cat_name));
    ui->label_name_pdf->setText(QString::fromStdString(output_pdf_name));
    m_config=config;

}

void DialogRunAnalysis::updateProgressBar(size_t step, size_t total) {
  int value = (step * 100) / total;
  ui->progressBar->setValue(value);
}


void DialogRunAnalysis::run(){
  try{

      PhzConfiguration::CreatePhzCatalogConfiguration conf { m_config };
      auto model_phot_grid = conf.getPhotometryGrid();
      auto marginalization_func = conf.getMarginalizationFunc();

      PhzLikelihood::ParallelCatalogHandler handler {conf.getPhotometricCorrectionMap(),
                                                        model_phot_grid, marginalization_func};
      auto catalog = conf.getCatalog();
      auto out_ptr = conf.getOutputHandler();
      std::function<void(size_t,size_t)> monitor_function = std::bind(&DialogRunAnalysis::updateProgressBar, this, std::placeholders::_1, std::placeholders::_2);
      handler.handleSources(catalog.begin(), catalog.end(), *out_ptr, monitor_function);
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

