/** 
 * @file PhotometryCatalogConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <vector>
#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "SourceCatalog/AttributeFromRow.h"
#include "SourceCatalog/SourceAttributes/PhotometryAttributeFromRow.h"
#include "PhzConfiguration/PhotometryCatalogConfiguration.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("PhotometryCatalogConfiguration");

po::options_description PhotometryCatalogConfiguration::getProgramOptions() {
  po::options_description options = CatalogConfiguration::getProgramOptions();
  options.add_options()
    ("filter-name-mapping", po::value<std::vector<std::string>>(),
        "The mapping of the flux and error columns of a filter FORMAT=\"filter flux_name error_name\"");
//    ("filter-name-index-mapping", po::value<std::vector<std::string>>(),
//        "The mapping of the flux and error columns of a filter FORMAT=\"filter flux_index error_index\"");
  return options;
}

PhotometryCatalogConfiguration::PhotometryCatalogConfiguration(const std::map<std::string, po::variable_value>& options)
        : CatalogConfiguration(options) {
  // First create the vector with the filter name mapping based on the user parameters
  std::vector<std::pair<std::string, std::pair<std::string, std::string>>> filter_name_mapping {};
  auto mapping_iter = options.find("filter-name-mapping");
  if (mapping_iter != options.end()) {
    for (auto& filter_mapping_option : mapping_iter->second.as<std::vector<std::string>>()) {
      smatch match_res;
      regex expr {"\\s*([^\\s]+)\\s+([^\\s]+)\\s+([^\\s]+)\\s*"};
      if (!regex_match(filter_mapping_option, match_res, expr)) {
        logger.error() << "Malformed filter-name-mapping: " << filter_mapping_option;
        throw Elements::Exception() << "Malformed filter-name-mapping: " << filter_mapping_option;
      }
      filter_name_mapping.emplace_back(match_res.str(1), std::make_pair(match_res.str(2), match_res.str(3)));
    }
  }
  if (filter_name_mapping.size() < 1) {
    logger.error() << "Found only " << filter_name_mapping.size() << " filter-name-mapping parameters";
    throw Elements::Exception() << "Need two or more source photometries to operate (check filter-name-mapping option)";
  }
  
  // Add the row handler to parse the photometries
  std::shared_ptr<SourceCatalog::AttributeFromRow> handler_ptr {new SourceCatalog::PhotometryAttributeFromRow{getAsTable().getColumnInfo(), std::move(filter_name_mapping)}};
  addAttributeHandler(std::move(handler_ptr));
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid