/** 
 * @file CreatePhotometryGrid.cpp
 * @date November 20, 2014
 * @author Nikolaos Apostolakos
 */

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "ElementsKernel/Program.h"
#include "ElementsKernel/Logging.h"
#include "PhzConfiguration/CreatePhotometryGridConfiguration.h"
#include "PhzModeling/PhotometryGridCreator.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("CreatePhotometryGrid");

class CreatePhotometryGrid : public Elements::Program {

public:
  
  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::CreatePhotometryGridConfiguration::getProgramOptions();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    PhzConfiguration::CreatePhotometryGridConfiguration conf {args};
    
    PhzModeling::PhotometryGridCreator creator {conf.getSedDatasetProvider(),
                                                conf.getReddeningDatasetProvider(),
                                                conf.getFilterDatasetProvider(), 5};
    
    auto param_space = PhzDataModel::createAxesTuple(conf.getZList(), conf.getEbvList(),
                                                     conf.getReddeningCurveList(),
                                                     conf.getSedList());
    
    auto grid = creator.createGrid(param_space, conf.getFilterList());
    
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