/** 
 * @file CreatePhzCatalogConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include "PhzOutput/BestModelCatalog.h"
#include "PhzOutput/PdfOutput.h"
#include "PhzConfiguration/CreatePhzCatalogConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description CreatePhzCatalogConfiguration::getProgramOptions() {
  po::options_description options {"Create PHZ Catalog options"};

  options.add_options()
  ("output-catalog-file", po::value<std::string>(),
      "The filename of the file to export the PHZ catalog file")
  ("output-pdf-file", po::value<std::string>(),
        "The filename of the PDF data");

  options.add(PhotometricCorrectionConfiguration::getProgramOptions());
  options.add(PhotometryCatalogConfiguration::getProgramOptions());
  options.add(PhotometryGridConfiguration::getProgramOptions());

  return options;
}

CreatePhzCatalogConfiguration::CreatePhzCatalogConfiguration(const std::map<std::string, po::variable_value>& options)
          : CatalogConfiguration(options), PhotometricCorrectionConfiguration(options),
            PhotometryCatalogConfiguration(options), PhotometryGridConfiguration(options) {
  m_options = options;
}

class MultiOutputHandler : public PhzOutput::OutputHandler {
public:
	virtual ~MultiOutputHandler() = default;
	void addHandler(std::unique_ptr<PhzOutput::OutputHandler> handler) {
	  m_handlers.emplace_back(std::move(handler));
	}
	void handleSourceOutput(const SourceCatalog::Source& source,
	                                  PhzDataModel::PhotometryGrid::const_iterator best_model,
	                                  const PhzDataModel::Pdf1D& pdf) override {
		for (auto& handler : m_handlers) {
			handler->handleSourceOutput(source, best_model, pdf);
		}
	}
private:
	std::vector<std::unique_ptr<PhzOutput::OutputHandler>> m_handlers;
};

std::unique_ptr<PhzOutput::OutputHandler> CreatePhzCatalogConfiguration::getOutputHandler() {
  std::unique_ptr<MultiOutputHandler> result {new MultiOutputHandler{}};
  if (!m_options["output-catalog-file"].empty()) {
    std::string out_file = m_options["output-catalog-file"].as<std::string>();
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler>{new PhzOutput::BestModelCatalog{out_file}});
  }
  if (!m_options["output-pdf-file"].empty()) {
    std::string out_file = m_options["output-pdf-file"].as<std::string>();
    result->addHandler(std::unique_ptr<PhzOutput::OutputHandler>{new PhzOutput::PdfOutput{out_file}});
  }
  return std::unique_ptr<PhzOutput::OutputHandler>{result.release()};
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid
