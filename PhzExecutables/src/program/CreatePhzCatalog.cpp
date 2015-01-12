/** 
 * @file CreatePhzCatalog.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <vector>
#include <future>
#include <thread>
#include <tuple>
#include <memory>
#include <atomic>
#include <chrono>
#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Program.h"
#include "PhzLikelihood/CatalogHandler.h"
#include "PhzConfiguration/CreatePhzCatalogConfiguration.h"

#include <fstream>
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/Pdf1D.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("CreatePhzCatalog");

struct ResultType {
  ResultType(const SourceCatalog::Source& source,
             const PhzDataModel::PhotometryGrid::const_iterator best_match,
             PhzDataModel::Pdf1D pdf)
        : source(source), best_match(best_match), pdf(std::move(pdf)) { }
  const SourceCatalog::Source& source;
  const PhzDataModel::PhotometryGrid::const_iterator best_match;
  PhzDataModel::Pdf1D pdf;
};

class ParallelJob {
  
public:
  
  ParallelJob(PhzLikelihood::CatalogHandler& handler, vector<SourceCatalog::Source>::const_iterator begin,
              vector<SourceCatalog::Source>::const_iterator end, atomic<size_t>& progress)
        : handler(handler), begin(begin), end(end), m_progress(progress) { }

  vector<ResultType> operator()() {
    vector<ResultType> result;
    Output output {result, m_progress};
    handler.handleSources(begin, end, output);
    return result;
  }
  
private:
  
  class Output : public PhzOutput::OutputHandler {
  public :
    Output(vector<ResultType>& result_vector, atomic<size_t>& progress) : m_result_vector(result_vector), m_progress(progress) {}
    void handleSourceOutput(const SourceCatalog::Source& source,
                            PhzDataModel::PhotometryGrid::const_iterator best_model,
                            const PhzDataModel::Pdf1D& pdf) override {
      // We need to make a copy of the pdf because the results will be handled
      // by a different thread when the refered object will be already deleted
      PhzDataModel::Pdf1D pdf_copy {pdf.getAxis<0>()};
      for (size_t i=0; i<pdf.size(); ++i) {
        pdf_copy(i) = pdf(i);
      }
      m_result_vector.push_back(ResultType{source, best_model, std::move(pdf_copy)});
      ++m_progress;
    }
  private:
    vector<ResultType>& m_result_vector;  
    atomic<size_t>& m_progress;
  };
  
  PhzLikelihood::CatalogHandler& handler;
  vector<SourceCatalog::Source>::const_iterator begin;
  vector<SourceCatalog::Source>::const_iterator end;
  atomic<size_t>& m_progress;

};

class CreatePhzCatalog : public Elements::Program {
  
  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::CreatePhzCatalogConfiguration::getProgramOptions();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    PhzConfiguration::CreatePhzCatalogConfiguration conf {args};
    
    auto model_phot_grid = conf.getPhotometryGrid();
    
    PhzLikelihood::CatalogHandler handler {conf.getPhotometricCorrectionMap(), move(model_phot_grid)};
    
    auto catalog = conf.getCatalog();
    
    std::vector<std::future<vector<ResultType>>> futures;
    
    atomic<size_t> progress {0};
    size_t total_sources = catalog.size();
    logger.info() << "Processing " << total_sources << " sources";
    uint threads = std::thread::hardware_concurrency();
    logger.info() << "Using " << threads << " threads";
    uint step = (catalog.end()-catalog.begin()) / threads;
    for (uint i=0; i< threads; ++i) {
      auto begin = catalog.begin() + i*step;
      auto end = ((i+1) == threads) ? catalog.end() : catalog.begin() + (i+1)*step;
      futures.push_back(std::async(std::launch::async, ParallelJob{handler, begin, end, progress}));
    }
    auto out_ptr = conf.getOutputHandler();
    
    while (progress < total_sources) {
      this_thread::sleep_for(chrono::seconds(5));
      int percentage_done = 100. * progress / total_sources;
      logger.info() << "Progress: " << percentage_done << " %";
    }
    for (auto& f : futures) {
      for (auto& result : f.get()) {
        out_ptr->handleSourceOutput(result.source, result.best_match, result.pdf);
      }
    }
    
    logger.info("Done!");
    return Elements::ExitCode::OK;
  }
  
  string getVersion() override {
    return "0.1";
  }
  
};

MAIN_FOR(CreatePhzCatalog)