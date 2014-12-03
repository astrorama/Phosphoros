/** 
 * @file CreatePhzCatalog.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <vector>
#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Program.h"
#include "PhzLikelihood/CatalogHandler.h"
#include "PhzConfiguration/CreatePhzCatalogConfiguration.h"

#include <fstream>
#include "PhzDataModel/PhzModel.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("CreatePhzCatalog");

class CreatePhzCatalog : public Elements::Program {
  
  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::CreatePhzCatalogConfiguration::getProgramOptions();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    PhzConfiguration::CreatePhzCatalogConfiguration conf {args};
    
    ifstream in {"/tmp/CreatePhotometryGrid/photometry_grid.dat"};
    auto model_phot_grid = PhzDataModel::phzGridBinaryImport<vector<SourceCatalog::Photometry>>(in);
    in.close();
    
    PhzLikelihood::CatalogHandler handler {conf.getPhotometricCorrectionMap(), move(model_phot_grid)};
    
    auto catalog = conf.getCatalog();
    
    handler.handleSources(catalog.begin(), catalog.end(), conf.getOutputHandler());
    
    return Elements::ExitCode::OK;
  }
  
  string getVersion() override {
    return "0.1";
  }
  
};

MAIN_FOR(CreatePhzCatalog)