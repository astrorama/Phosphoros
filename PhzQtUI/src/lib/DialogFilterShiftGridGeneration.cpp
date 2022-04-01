/*
 * DialogGridGeneration.cpp
 *
 *  Created on: 2021/09/15
 *      Author: fdubath
 */
#include <QFuture>
#include <QtConcurrent>
#include <QDir>
#include <QMessageBox>
#include "PhzQtUI/DialogFilterShiftGridGeneration.h"
#include <boost/program_options.hpp>
#include "ui_DialogFilterShiftGridGeneration.h"
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
#include "PhzConfiguration/FilterVariationConfig.h"
#include "PhzConfiguration/ComputeFilterVariationCoefficientConfig.h"
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "PhzFilterVariation/FilterVariationSingleGridCreator.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/FilterVariationCoefficientGridOutputConfig.h"



using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger_DGC = Elements::Logging::getLogger("DialogFilterShiftGridGeneration");

class SparseProgressReporter {

public:

  SparseProgressReporter(std::function<void(size_t step, size_t total)>  parent_listener,
                         size_t already_done, size_t total)
          : m_parent_listener {parent_listener}, m_already_done {already_done},
            m_total {total} {
  }

  void operator()(size_t step, size_t) {
    m_parent_listener(m_already_done + step, m_total);
  }

private:
  std::function<void(size_t step, size_t total)>  m_parent_listener;
  size_t m_already_done;
  size_t m_total;

};


DialogFilterShiftGridGeneration::DialogFilterShiftGridGeneration(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogFilterShiftGridGeneration) {
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

DialogFilterShiftGridGeneration::~DialogFilterShiftGridGeneration() {}

void DialogFilterShiftGridGeneration::setValues(std::string grid_name,
    const std::map<std::string, boost::program_options::variable_value>& config) {
    ui->label_name->setText(QString::fromStdString(grid_name));
    m_config=config;

}

std::string DialogFilterShiftGridGeneration::runFunction() {
  try {
    completeWithDefaults<PhzConfiguration::ComputeFilterVariationCoefficientConfig>(m_config);
    long config_manager_id = Configuration::getUniqueManagerId();
    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeFilterVariationCoefficientConfig>();
    config_manager.closeRegistration();
    config_manager.initialize(m_config);
    
    auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();
    auto& sed_provider = config_manager.template getConfiguration<SedProviderConfig>().getSedDatasetProvider();
    auto& reddening_provider = config_manager.template getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
    const auto& filter_provider = config_manager.template getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
    auto& igm_abs_func = config_manager.template getConfiguration<IgmConfig>().getIgmAbsorptionFunction();
    auto output_function = config_manager.template getConfiguration<FilterVariationCoefficientGridOutputConfig>().getOutputFunction();
    auto shift_samples = config_manager.template getConfiguration<FilterVariationConfig>().getSampling();
    auto cosmology =  config_manager.template getConfiguration<CosmologicalParameterConfig>().getCosmologicalParam();
    auto lum_filter_name = config_manager.getConfiguration<ModelNormalizationConfig>().getNormalizationFilter();
    auto sun_sed_name = config_manager.getConfiguration<ModelNormalizationConfig>().getReferenceSolarSed();
    
    auto normalizer_functor =
         Euclid::PhzModeling::NormalizationFunctorFactory::NormalizationFunctorFactory::GetFunction(filter_provider, lum_filter_name, sed_provider, sun_sed_name);


    std::map<std::string, PhzDataModel::PhotometryGrid> result_map{};

    // Compute the total number of models
    size_t total = 0;
    for (auto& pair : model_phot_grid.region_axes_map) {
      total += GridContainer::makeGridIndexHelper(pair.second).m_axes_index_factors.back();
    }


    auto monitor_function = [this](size_t step, size_t mon_total) {
         int value = (step * 100) / mon_total;
         if (!PhzUtils::getStopThreadsFlag()) {
           // If the user has canceled we do not want to update the progress bar,
           // because the GUI thread might have already deleted it
           emit signalUpdateBar(value);
         }
    };
    
    PhzFilterVariation::FilterVariationSingleGridCreator grid_creator{
     sed_provider,
     reddening_provider,
     filter_provider,
     igm_abs_func,
     normalizer_functor,
     shift_samples
    };
    size_t already_done = 0;
    for (auto& grid_pair : model_phot_grid.region_axes_map) {
     SparseProgressReporter reporter {monitor_function, already_done, total};
     result_map.emplace(std::make_pair(grid_pair.first,
                                       grid_creator.createGrid(grid_pair.second,
                                                               model_phot_grid.filter_names,
                                                               cosmology, reporter)));
    }
    monitor_function(already_done, total);
    if (!PhzUtils::getStopThreadsFlag()) {
       output_function(result_map);
    }

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

void DialogFilterShiftGridGeneration::on_btn_cancel_clicked() {
  PhzUtils::getStopThreadsFlag() = true;
}

void DialogFilterShiftGridGeneration::runFinished() {
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

void DialogFilterShiftGridGeneration::run() {
  m_future_watcher.setFuture(QtConcurrent::run(this, &DialogFilterShiftGridGeneration::runFunction));
}

}
}



