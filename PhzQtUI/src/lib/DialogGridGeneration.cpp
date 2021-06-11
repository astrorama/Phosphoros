/*
 * DialogGridGeneration.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: fdubath
 */
#include <QFuture>
#include <QtConcurrent>
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
#include "PhzUtils/Multithreading.h"
#include "PhzConfiguration/ModelNormalizationConfig.h"
#include "PhzModeling/NormalizationFunctorFactory.h"
#include "PhzConfiguration/CosmologicalParameterConfig.h"

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
        ui->btn_cancel->setEnabled(true);

        m_timer.reset(new QTimer(this));
        m_timer->setInterval(100);
        m_timer->setSingleShot(true);
        connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(run()));
        m_timer->start();
        
        connect(&m_future_watcher, SIGNAL(finished()), this, SLOT(runFinished()));
        connect(this, SIGNAL(signalUpdateBar(int)), ui->progressBar, SLOT(setValue(int)));
    }

DialogGridGeneration::~DialogGridGeneration() {}

void DialogGridGeneration::setValues(std::string grid_name,
    const std::map<std::string, boost::program_options::variable_value>& config) {
    ui->label_name->setText(QString::fromStdString(grid_name));
    m_config=config;

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
    auto& cosmology = config_manager.template getConfiguration<CosmologicalParameterConfig>().getCosmologicalParam();
    
    auto lum_filter_name = config_manager.template getConfiguration<ModelNormalizationConfig>().getNormalizationFilter();
    auto sun_sed_name = config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed();
    auto normalizer_functor =
             Euclid::PhzModeling::NormalizationFunctorFactory::NormalizationFunctorFactory::GetFunction(filter_provider, lum_filter_name, sed_provider, sun_sed_name);


    Euclid::PhzModeling::SparseGridCreator creator {
                sed_provider, reddening_provider, filter_provider, igm_abs_func, normalizer_functor};
    
    auto monitor_function = [this](size_t step, size_t total) {
      int value = (step * 100) / total;
      if (!PhzUtils::getStopThreadsFlag()) {
        // If the user has canceled we do not want to update the progress bar,
        // because the GUI thread might have already deleted it
        emit signalUpdateBar(value);
      }
    };

    auto param_space_map = config_manager.getConfiguration<ParameterSpaceConfig>().getParameterSpaceRegions();
    auto filter_list = config_manager.getConfiguration<FilterConfig>().getFilterList();
    
    auto result = creator.createGrid(param_space_map, filter_list, cosmology, monitor_function);

    auto output = config_manager.getConfiguration<ModelGridOutputConfig>().getOutputFunction();
    output(result);
    return "";
  }
  catch (const Elements::Exception & e) {
    return "Sorry, an error occurred during the computation:\n"
        + std::string(e.what());
  }
  catch (const std::exception& e) {
    return "Sorry, an error occurred during the computation:\n"
        + std::string(e.what());
  }
  catch (...) {
    return "Sorry, an error occurred during the computation.";
  }
}

void DialogGridGeneration::on_btn_cancel_clicked() {
  PhzUtils::getStopThreadsFlag() = true;
}

void DialogGridGeneration::runFinished() {
  auto message = m_future_watcher.result();
  if (message.length() == 0) {
    this->accept();
    return;
  } else {
    QMessageBox::warning(this, "Error in the computation...",
        QString::fromStdString(message), QMessageBox::Close);
    this->reject();
  }
}

void DialogGridGeneration::run() {
  m_future_watcher.setFuture(QtConcurrent::run(this, &DialogGridGeneration::runFunction));
}

}
}



