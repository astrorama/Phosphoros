/*
 * DialogGridGeneration.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: fdubath
 */
#include <QFuture>
#include <qtconcurrentrun.h>
#include <QDir>
#include <QMessageBox>
#include "PhzQtUI/DialogRunAnalysis.h"
#include <boost/program_options.hpp>
#include "ui_DialogRunAnalysis.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "PhzConfiguration/ComputeRedshiftsConfiguration.h"

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

void DialogRunAnalysis::setValues(std::string output_cat_name,
    std::string output_pdf_name, std::string output_lik_name,
    const std::map<std::string, boost::program_options::variable_value>& config) {
  if (output_cat_name.length() > 0) {
    ui->label_name_cat->setText(QString::fromStdString(output_cat_name));
  } else {
    ui->label_name_cat->setVisible(false);
    ui->label_cat->setVisible(false);
  }

  if (output_pdf_name.length() > 0) {
    ui->label_name_pdf->setText(QString::fromStdString(output_pdf_name));
  } else {
    ui->label_name_pdf->setVisible(false);
    ui->label_pdf->setVisible(false);
  }

  if (output_lik_name.length() > 0) {
    ui->label_name_lik->setText(QString::fromStdString(output_lik_name));
  } else {
    ui->label_name_lik->setVisible(false);
    ui->label_lik->setVisible(false);
  }
  m_config = config;

}

void DialogRunAnalysis::updateProgressBar(size_t step, size_t total) {
  int value = (step * 100) / total;
  ui->progressBar->setValue(value);
}

std::string DialogRunAnalysis::runFunction(){
  try {
    PhzConfiguration::ComputeRedshiftsConfiguration conf { m_config };
    auto model_phot_grid = conf.getPhotometryGrid();
    auto marginalization_func = conf.getMarginalizationFunc();

    PhzLikelihood::ParallelCatalogHandler handler {
        conf.getPhotometricCorrectionMap(), model_phot_grid,
        conf.getPriors(), marginalization_func };
    auto catalog = conf.getCatalog();
    auto out_ptr = conf.getOutputHandler();
    std::function<void(size_t, size_t)> monitor_function = std::bind(
        &DialogRunAnalysis::updateProgressBar, this, std::placeholders::_1,
        std::placeholders::_2);
    handler.handleSources(catalog.begin(), catalog.end(), *out_ptr,
        monitor_function);

    return "";
  }
  catch (const Elements::Exception & e) {
    return "Sorry, an error occurred during the computation: "
        + std::string(e.what());
  }
  catch (const std::exception& e) {
    return "Sorry, an error occurred during the computation: "
        + std::string(e.what());
  }
  catch (...) {
    return "Sorry, an error occurred during the computation.";
  }
}

void DialogRunAnalysis::run(){
  QFuture<std::string> future = QtConcurrent::run(this,
      &DialogRunAnalysis::runFunction);
  std::string message = future.result();
  if (message.length() == 0) {
    this->accept();
    return;
  } else {
    QMessageBox::warning(this, "Error in the computation...",
        QString::fromStdString(message), QMessageBox::Close);
    this->reject();
  }
}


}
}

