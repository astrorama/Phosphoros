/** 
 * @file CreatePhzCatalog.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <tuple>
#include <memory>
#include <chrono>
#include "ElementsKernel/ProgramHeaders.h"
#include "PhzLikelihood/ParallelCatalogHandler.h"
#include "PhzConfiguration/CreatePhzCatalogConfiguration.h"

#include <fstream>
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/Pdf1D.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("CreatePhzCatalog");

class ProgressReporter {
  
public:
  
  void operator()(size_t step, size_t total) {
    int percentage_done = 100. * step / total;
    auto now_time = chrono::system_clock::now();
    auto time_diff = now_time - m_last_time;
    if (percentage_done > m_last_progress || chrono::duration_cast<chrono::seconds>(time_diff).count() >= 5) {
      m_last_progress = percentage_done;
      m_last_time = now_time;
      logger.info() << "Progress: " << percentage_done << " % (" << step << "/" << total << ")";
    }
  }
  
private:
  
  int m_last_progress = -1;
  chrono::time_point<chrono::system_clock> m_last_time = chrono::system_clock::now();
  
};

class CreatePhzCatalog : public Elements::Program {
  
  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::CreatePhzCatalogConfiguration::getProgramOptions();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    PhzConfiguration::CreatePhzCatalogConfiguration conf {args};
    
    auto model_phot_grid = conf.getPhotometryGrid();
    auto marginalization_func = conf.getMarginalizationFunc();
    
    PhzLikelihood::ParallelCatalogHandler handler {conf.getPhotometricCorrectionMap(),
                                                   model_phot_grid, marginalization_func};
    
    auto catalog = conf.getCatalog();
    auto out_ptr = conf.getOutputHandler();
    
    handler.handleSources(catalog.begin(), catalog.end(), *out_ptr, ProgressReporter{});
    
    logger.info("Done!");
    return Elements::ExitCode::OK;
  }
  
};

MAIN_FOR(CreatePhzCatalog)