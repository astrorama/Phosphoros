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
#include "PhzConfiguration/ComputeModelGridConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzModeling/SparseGridCreator.h"
#include "DefaultOptionsCompleter.h"
#include "Configuration/Utils.h"

// #include <future>

using namespace Euclid::PhzConfiguration;

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
    completeWithDefaults<ComputeModelGridConfig>(m_config);
    long config_manager_id = Configuration::getUniqueManagerId();
    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeModelGridConfig>();
    config_manager.closeRegistration();
    config_manager.initialize(m_config);
    
    auto& sed_provider = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();
    auto& reddening_provider = config_manager.getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
    const auto& filter_provider = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto& igm_abs_func = config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionFunction();
    
    Euclid::PhzModeling::SparseGridCreator creator {
                sed_provider, reddening_provider, filter_provider, igm_abs_func};
    
    std::function<void(size_t, size_t)> monitor_function = std::bind(
        &DialogGridGeneration::updateGridProgressBar, this,
        std::placeholders::_1, std::placeholders::_2);

    auto param_space_map = config_manager.getConfiguration<ParameterSpaceConfig>().getParameterSpaceRegions();
    auto filter_list = config_manager.getConfiguration<FilterConfig>().getFilterList();
    
    auto result = creator.createGrid(param_space_map, filter_list, monitor_function);

    auto output = config_manager.getConfiguration<ModelGridOutputConfig>().getOutputFunction();
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



