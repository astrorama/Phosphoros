/** 
 * @file CatalogConfiguration.cpp
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#include <fstream>
#include <boost/filesystem.hpp>
#include <CCfits/CCfits>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "Table/Table.h"
#include "Table/AsciiReader.h"
#include "Table/FitsReader.h"
#include "SourceCatalog/CatalogFromTable.h"
#include "PhzConfiguration/CatalogConfiguration.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzConfiguration {

static Elements::Logging logger = Elements::Logging::getLogger("CatalogConfiguration");

po::options_description CatalogConfiguration::getProgramOptions() {
  po::options_description options {"Catalog options"};
  options.add_options()
    ("input-catalog-file", po::value<std::string>(),
        "The file containing the input catalog")
    ("input-catalog-format", po::value<std::string>(),
        "The format of the input catalog (FITS or ASCII)")
    ("source-id-column-name", po::value<std::string>(),
        "The name of the column representing the source ID")
    ("source-id-column-index", po::value<int>(),
        "The index of the column representing the source ID");
  return options;
}

CatalogConfiguration::CatalogConfiguration(const std::map<std::string, po::variable_value>& options) : m_options{options} {
  if (m_options["input-catalog-file"].empty()) {
    logger.error("Missing option input-catalog-file");
    throw Elements::Exception("Missing mandatory option input-catalog-file");
  }
  auto catalog_file = m_options["input-catalog-file"].as<std::string>();
  if (!fs::exists(catalog_file)) {
    logger.error() << "File " << catalog_file << " not found";
    throw Elements::Exception() << "Input catalog file " << catalog_file << " does not exist";
  }
  if (fs::is_directory(catalog_file)) {
    logger.error() <<  catalog_file << " is a directory";
    throw Elements::Exception() << "Input catalog file " << catalog_file << " is not a file";
  }
  if (!m_options["source-id-column-name"].empty() && !m_options["source-id-column-index"].empty()) {
    logger.error("Found both source-id-column-name and source-id-column-index options");
    throw Elements::Exception("Options source-id-column-name and source-id-column-index are mutually exclusive");
  }
}

void CatalogConfiguration::addAttributeHandler(std::shared_ptr<SourceCatalog::AttributeFromRow> handler) {
  m_attribute_handlers.push_back(handler);
}

enum class FormatType {
  FITS, ASCII
};

FormatType autoDetectFormatType(std::string file) {
  logger.info() << "Auto-detecting format of file " << file;
  FormatType result = FormatType::ASCII;
  {
    std::ifstream in {file};
    std::array<char, 80> first_header_array;
    in.read(first_header_array.data(), 80);
    in.close();
    std::string first_header_str {first_header_array.data()};
    if (first_header_str.compare(0, 9, "SIMPLE  =") == 0) {
      result = FormatType::FITS;
    }
  }
  logger.info() << "Detected " << (result == FormatType::FITS ? "FITS" : "ASCII") << " format";
  return result;
}

Table::Table readFitsTable(std::string file) {
  CCfits::FITS fits {file};
  return Table::FitsReader().read(fits.extension(1));
}

Table::Table readAsciiTable(std::string file) {
  std::ifstream in {file};
  return Table::AsciiReader().read(in);
}

SourceCatalog::Catalog CatalogConfiguration::getCatalog() {
  std::string catalog_file = m_options["input-catalog-file"].as<std::string>();
  
  // Get the format of the file
  FormatType format;
  if (m_options["input-catalog-format"].empty()) {
    format = autoDetectFormatType(catalog_file);
  } else if (m_options["input-catalog-format"].as<std::string>().compare("FITS") == 0) {
    format = FormatType::FITS;
  } else {
    format = FormatType::ASCII;
  }
  
  // Read the catalog from the file as a Table object
  logger.info() << "Reading table from file " << catalog_file;
  Table::Table table = (format == FormatType::FITS)
                       ? readFitsTable(catalog_file)
                       : readAsciiTable(catalog_file);
  
  // Get the name of the ID column
  std::string id_column_name = "ID";
  if (!m_options["source-id-column-name"].empty()) {
    id_column_name = m_options["source-id-column-name"].as<std::string>();
  }
  if (!m_options["source-id-column-index"].empty()) {
    int index = m_options["source-id-column-index"].as<int>();
    id_column_name = table.getColumnInfo()->getName(index-1);
  }
  logger.info() << "Using ID column \"" << id_column_name << '"';
  
  // Convert the table to a catalog object
  logger.info() << "Converting the table to a catalog";
  SourceCatalog::CatalogFromTable converter {table.getColumnInfo(), id_column_name, m_attribute_handlers};
  return converter.createCatalog(table);
}

} // end of namespace PhzConfiguration
} // end of namespace Euclid