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
#include "ElementsKernel/Exception.h"
#include "PhzQtUI/DialogRunAnalysis.h"
#include <boost/program_options.hpp>
#include "ui_DialogRunAnalysis.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"


#include "Configuration/ConfigManager.h"
#include "DefaultOptionsCompleter.h"
#include "PhzConfiguration/ComputeRedshiftsConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzConfiguration/LikelihoodGridFuncConfig.h"
#include "PhzConfiguration/MarginalizationConfig.h"
#include "PhzConfiguration/PhotometricCorrectionConfig.h"
#include "PhzConfiguration/PriorConfig.h"
#include "Configuration/CatalogConfig.h"
#include "PhzConfiguration/ComputeLuminosityModelGridConfig.h"
#include "PhzConfiguration/ComputeModelGridConfig.h"
#include "PhzConfiguration/SedProviderConfig.h"
#include "PhzConfiguration/ReddeningProviderConfig.h"
#include "PhzConfiguration/FilterProviderConfig.h"
#include "PhzConfiguration/LuminosityBandConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzModeling/SparseGridCreator.h"
#include "PhzModeling/NoIgmFunctor.h"
#include "PhzUtils/Multithreading.h"
#include "Configuration/Utils.h"

// #include <future>

using namespace Euclid::PhzConfiguration;

namespace Euclid {
namespace PhzQtUI {

bool DialogRunAnalysis::checkLuminosityGrid(){
  if (m_config.count("luminosity-prior")==1 && m_config.at("luminosity-prior").as<std::string>()=="YES"){
    //check the grid
    try{
      completeWithDefaults<ComputeRedshiftsConfig>(m_config);

      long config_manager_id = Configuration::getUniqueManagerId();
      auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
      config_manager.registerConfiguration<ComputeRedshiftsConfig>();
      config_manager.closeRegistration();
      config_manager.initialize(m_config);

      return true;

    } catch(const Elements::Exception&) {}

    return false;

  } else {
    // The luminosity prior is not enabled: no need for a luminosity grid
    return true;
  }
}

class ProgressReporter {
public:

  ProgressReporter(){}

  void operator()(int,int) {}

};

void DialogRunAnalysis::computeLuminosityGrid(){

   completeWithDefaults<ComputeLuminosityModelGridConfig>(m_lum_config);

   long config_manager_id = Configuration::getUniqueManagerId();
   auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
   config_manager.registerConfiguration<ComputeLuminosityModelGridConfig>();
   config_manager.closeRegistration();

   config_manager.initialize(m_lum_config);


   auto& sed_provider = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider();
   auto& reddening_provider = config_manager.getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider();
   const auto& filter_provider = config_manager.getConfiguration<FilterProviderConfig>().getFilterDatasetProvider();
   auto& igm_abs_func = config_manager.getConfiguration<IgmConfig>().getIgmAbsorptionFunction();

   Euclid::PhzModeling::SparseGridCreator creator {
                sed_provider, reddening_provider, filter_provider, igm_abs_func};

   auto param_space_map = config_manager.getConfiguration<ComputeLuminosityModelGridConfig>().getParameterSpaceRegions();
   std::vector<Euclid::XYDataset::QualifiedName> filter_list = {config_manager.getConfiguration<Euclid::PhzConfiguration::LuminosityBandConfig>().getLuminosityFilter()};

   auto results = creator.createGrid(param_space_map, filter_list, ProgressReporter{});


   auto output = config_manager.getConfiguration<ModelGridOutputConfig>().getOutputFunction();
   output(results);
}







DialogRunAnalysis::DialogRunAnalysis(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DialogRunAnalysis)
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

DialogRunAnalysis::~DialogRunAnalysis() {}

void DialogRunAnalysis::setValues(std::string output_cat_name,
                                  std::string output_pdf_name,
                                  std::string output_lik_name,
                                  std::string output_pos_name,
    const std::map<std::string, boost::program_options::variable_value>& config,
    const std::map<std::string, boost::program_options::variable_value>& luminosity_config) {
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

  if (output_pos_name.length() > 0) {
     ui->label_name_pos->setText(QString::fromStdString(output_pos_name));
   } else {
     ui->label_name_pos->setVisible(false);
     ui->label_pos->setVisible(false);
   }
  m_config = config;
  m_lum_config=luminosity_config;

}

std::string DialogRunAnalysis::runFunction(){
  try {
    completeWithDefaults<ComputeRedshiftsConfig>(m_config);

    long config_manager_id = Configuration::getUniqueManagerId();
    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<ComputeRedshiftsConfig>();
    config_manager.closeRegistration();
    config_manager.initialize(m_config);



    auto& model_phot_grid = config_manager.getConfiguration<PhotometryGridConfig>().getPhotometryGrid();
    auto& marginalization_func = config_manager.getConfiguration<MarginalizationConfig>().getMarginalizationFunc();
    auto& likelihood_grid_func = config_manager.getConfiguration<LikelihoodGridFuncConfig>().getLikelihoodGridFunction();
    auto& correction_map = config_manager.getConfiguration<PhotometricCorrectionConfig>().getPhotometricCorrectionMap();
    auto& priors = config_manager.getConfiguration<PriorConfig>().getPriors();

    std::map<int,std::vector<XYDataset::QualifiedName>> non_numerical_axis_order{};
    non_numerical_axis_order[PhzDataModel::ModelParameter::REDDENING_CURVE] = config_manager.getConfiguration<ReddeningProviderConfig>().getReddeningDatasetProvider()->listContents("");
    non_numerical_axis_order[PhzDataModel::ModelParameter::SED] = config_manager.getConfiguration<SedProviderConfig>().getSedDatasetProvider()->listContents("");


    PhzLikelihood::ParallelCatalogHandler handler { correction_map,
                                                    model_phot_grid,
                                                    likelihood_grid_func,
                                                    priors,
                                                    non_numerical_axis_order,
                                                    marginalization_func };

    auto& catalog = config_manager.getConfiguration<Configuration::CatalogConfig>().getCatalog();
    auto out_ptr = config_manager.getConfiguration<ComputeRedshiftsConfig>().getOutputHandler();

    auto monitor_function = [this](size_t step, size_t total) {
      int value = (step * 100) / total;
      // If the user has canceled we do not want to update the progress bar,
      // because the GUI thread might have already deleted it
      if (!PhzUtils::getStopThreadsFlag()) {
        emit signalUpdateBar(value);
      }
    };

    handler.handleSources(catalog.begin(), catalog.end(), *out_ptr, monitor_function);


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

void DialogRunAnalysis::runFinished() {
  auto message = m_future_watcher.result();
  if (message.length() == 0) {
    this->accept();
    QMessageBox::information(this, "", "Requested computation completed successfully", QMessageBox::Close);
    return;
  } else {
    QMessageBox::warning(this, "Error in the computation...",
        QString::fromStdString(message), QMessageBox::Close);
    this->reject();
  }
}

void DialogRunAnalysis::on_btn_cancel_clicked() {
  PhzUtils::getStopThreadsFlag() = true;
}

void DialogRunAnalysis::run() {
  if (!checkLuminosityGrid()){

    computeLuminosityGrid();
  }

  m_future_watcher.setFuture(QtConcurrent::run(this, &DialogRunAnalysis::runFunction));
}

}
}

