/*
 * DialogGalCorrGridGeneration.cpp
 *
 *  Created on: 2018 12 05
 *      Author: fdubath
 */
#include <tuple>
#include <map>
#include <memory>
#include <chrono>
#include <QFuture>
#include <qtconcurrentrun.h>
#include <QDir>
#include <QMessageBox>
#include "PhzQtUI/DialogGalCorrGridGeneration.h"
#include <boost/program_options.hpp>
#include "ui_DialogGalCorrGridGeneration.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzModeling/SparseGridCreator.h"
#include "DefaultOptionsCompleter.h"
#include "Configuration/Utils.h"
#include "PhzUtils/Multithreading.h"


#include "Configuration/ConfigManager.h"
#include "PhzGalacticCorrection/GalacticCorrectionFactorSingleGridCreator.h"
#include "PhzConfiguration/ComputeModelGalacticCorrectionCoefficientConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/MilkyWayReddeningConfig.h"
#include "PhzConfiguration/CorrectionCoefficientGridOutputConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/IgmConfig.h"

// #include <future>

using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzQtUI {


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



DialogGalCorrGridGeneration::DialogGalCorrGridGeneration(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogGalCorrGridGeneration)
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

DialogGalCorrGridGeneration::~DialogGalCorrGridGeneration() {}

void DialogGalCorrGridGeneration::setValues(std::string grid_name,
    const std::map<std::string, boost::program_options::variable_value>& config) {
    ui->label_name->setText(QString::fromStdString(grid_name));
    m_config=config;

}

std::string DialogGalCorrGridGeneration::runFunction() {
  try {
    completeWithDefaults<PhzConfiguration::ComputeModelGalacticCorrectionCoefficientConfig>(m_config);
    long config_manager_id = Configuration::getUniqueManagerId();
    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<PhzConfiguration::ComputeModelGalacticCorrectionCoefficientConfig>();
    config_manager.closeRegistration();
    config_manager.initialize(m_config);

     auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGridInfo();
     auto& sed_provider = config_manager.template getConfiguration<SedProviderConfig>().getSedDatasetProvider();
     auto& reddening_provider = config_manager.template getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
     const auto& filter_provider = config_manager.template getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
     auto& igm_abs_func = config_manager.template getConfiguration<IgmConfig>().getIgmAbsorptionFunction();
     auto miky_way_reddening_curve = config_manager.template getConfiguration<MilkyWayReddeningConfig>().getMilkyWayReddeningCurve();
     auto output_function = config_manager.template getConfiguration<CorrectionCoefficientGridOutputConfig>().getOutputFunction();

     std::map<std::string, PhzDataModel::PhotometryGrid> result_map{};

     // Compute the total number of models
      size_t total = 0;
      for (auto& pair : model_phot_grid.region_axes_map) {
        total += GridContainer::makeGridIndexHelper(pair.second).m_axes_index_factors.back();
      }

      auto monitor_function = [this](size_t step, size_t total) {
           int value = (step * 100) / total;
           if (!PhzUtils::getStopThreadsFlag()) {
             // If the user has canceled we do not want to update the progress bar,
             // because the GUI thread might have already deleted it
             emit signalUpdateBar(value);
           }
         };


     PhzGalacticCorrection::GalacticCorrectionSingleGridCreator grid_creator{
       sed_provider,
       reddening_provider,
       filter_provider,
       igm_abs_func,
       miky_way_reddening_curve
     };
     size_t already_done = 0;

     for(auto& grid_pair : model_phot_grid.region_axes_map){
       if (!PhzUtils::getStopThreadsFlag()) {
         logger.info() << "Correction computation for region '"<<grid_pair.first<<"'";
         SparseProgressReporter reporter {monitor_function, already_done, total};
         result_map.emplace(std::make_pair(grid_pair.first, grid_creator.createGrid(grid_pair.second, model_phot_grid.filter_names, reporter)));
         already_done+=GridContainer::makeGridIndexHelper(grid_pair.second).m_axes_index_factors.back();
       }
     }
     monitor_function(already_done,total);
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

void DialogGalCorrGridGeneration::on_btn_cancel_clicked() {
  PhzUtils::getStopThreadsFlag() = true;
}

void DialogGalCorrGridGeneration::runFinished() {
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

void DialogGalCorrGridGeneration::run() {
  m_future_watcher.setFuture(QtConcurrent::run(this, &DialogGalCorrGridGeneration::runFunction));
}

}
}



