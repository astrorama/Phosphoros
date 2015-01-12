/** 
 * @file CreatePhotometryGrid.cpp
 * @date November 20, 2014
 * @author Nikolaos Apostolakos
 */

#include <map>
#include <string>
#include <chrono>
#include <boost/program_options.hpp>
#include "ElementsKernel/Program.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/CreatePhotometryGridConfiguration.h"
#include "PhzModeling/PhotometryGridCreator.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("CreatePhotometryGrid");

class ProgressReporter {
  
public:
  
  void operator()(size_t step, size_t total) {
    int percentage_done = 100. * step / total;
    auto now_time = std::chrono::system_clock::now();
    auto time_diff = now_time - m_last_time;
    if (percentage_done > m_last_progress || std::chrono::duration_cast<std::chrono::seconds>(time_diff).count() >= 5) {
      m_last_progress = percentage_done;
      m_last_time = now_time;
      logger.info() << "Progress: " << percentage_done << " % (" << step << "/" << total << ")";
    }
  }
  
private:
  
  int m_last_progress = -1;
  std::chrono::time_point<std::chrono::system_clock> m_last_time = std::chrono::system_clock::now();
  
};

class CreatePhotometryGrid : public Elements::Program {

public:
  
  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::CreatePhotometryGridConfiguration::getProgramOptions();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    PhzConfiguration::CreatePhotometryGridConfiguration conf {args};
    
    PhzModeling::PhotometryGridCreator creator {conf.getSedDatasetProvider(),
                                                conf.getReddeningDatasetProvider(),
                                                conf.getFilterDatasetProvider()};
    
    auto param_space = PhzDataModel::createAxesTuple(conf.getZList(), conf.getEbvList(),
                                                     conf.getReddeningCurveList(),
                                                     conf.getSedList());
    
    auto grid = creator.createGrid(param_space, conf.getFilterList(), ProgressReporter{});
    
    logger.info() << "Creating the output";
    auto output = conf.getOutputFunction();
    output(grid);
    
    logger.info() << "Done!";
    return Elements::ExitCode::OK;
  }
  
  string getVersion() override {
    return "0.1";
  }
  
};

MAIN_FOR(CreatePhotometryGrid)