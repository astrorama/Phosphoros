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
#include "PhzQtUI/DialogGridGeneration.h"
#include <boost/program_options.hpp>
#include "ui_DialogGridGeneration.h"
#include "PhzConfiguration/ComputeModelGridConfiguration.h"
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

std::string DialogGridGeneration::runFunction() {
  try {
    PhzConfiguration::ComputeModelGridConfiguration conf { m_config };
    PhzModeling::PhotometryGridCreator creator { conf.getSedDatasetProvider(),
        conf.getReddeningDatasetProvider(), conf.getFilterDatasetProvider(),
        conf.getIgmAbsorptionFunction() };

    auto param_space = PhzDataModel::createAxesTuple(conf.getZList().at(""),
        conf.getEbvList().at(""), conf.getReddeningCurveList().at(""), conf.getSedList().at(""));

    std::function<void(size_t, size_t)> monitor_function = std::bind(
        &DialogGridGeneration::updateGridProgressBar, this,
        std::placeholders::_1, std::placeholders::_2);

    std::map<std::string, PhzDataModel::PhotometryGrid> result {};
    result.emplace(std::make_pair(std::string{""}, creator.createGrid(param_space, conf.getFilterList(), monitor_function)));
//    auto grid = creator.createGrid(param_space, conf.getFilterList(),
//        monitor_function);

    auto output = conf.getOutputFunction();
    output(result);
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

void DialogGridGeneration::run() {
  QFuture<std::string> future = QtConcurrent::run(this,
      &DialogGridGeneration::runFunction);
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



