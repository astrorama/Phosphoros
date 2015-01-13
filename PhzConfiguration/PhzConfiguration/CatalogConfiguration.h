/**
 * @file CatalogConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATIONCATALOGCONFIGURATION_H
#define	PHZCONFIGURATIONCATALOGCONFIGURATION_H

#include <map>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include "SourceCatalog/AttributeFromRow.h"
#include "SourceCatalog/Catalog.h"
#include "Table/Table.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class CatalogConfiguration
 *
 * @brief Superclass of all configuration classes related with catalog input
 *
 * @details
 * The CatalogConfiguration class provides the basis for reading input catalogs.
 * It provides all the functionality for reading an input catalog as an object
 * of type Table::Table and for converting it to an object of type
 * SourceCatalog::Catalog.
 *
 * Because this is a generic class, it has no knowledge of the specific catalog
 * attributes. By default it does not use any attribute handlers and it only
 * parses the ID of the catalog entries. To create more specific catalog
 * configurations (for example coordinates catalog), one can inherit from the
 * CatalogConfiguration class and use the addAttributeHandler() method for
 * instructing how to create the different source attributes. The method
 * getAsTable() can be used by the subclasses if they need extra information
 * for constructing the attribute handlers (for example if they need to get the
 * column names).
 *
 * Note that when creating subclasses of the CatalogConfiguration only virtual
 * inheritance must be used. This enables the usage of multiple inheritance of
 * the subclasses for creating more complex catalog configurations (for example
 * a catalog configuration for a catalog with both coordinates and photometries).
 *
 * Note that the subclasses are aware only of the Table::Table object and that
 * the actual format the catalog is stored is abstracted by the CatalogConfiguration.
 */
class CatalogConfiguration {

public:

  /**
   * @brief
   * Returns the program options defined by the CatalogConfiguration
   *
   * @details
   * These options are:
   * - input-catalog-file     : The file containing the input catalog
   * - input-catalog-format   : The format of the input catalog (FITS or ASCII)
   * - source-id-column-name  : The name of the column representing the source ID
   * - source-id-column-index : The index (1-based) of the column representing the source ID
   *
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief
   * Constructs a new CatalogConfiguration instance, for the given options
   * @details
   * The option input-catalog-file is obligatory and it must be an existing file.
   * The option input-catalog-format is optional and defaults to automatically
   * detect the format of the file. The parameters source-id-column-name and
   * source-id-column-index are mutually exclusive and if are both missing they
   * default to the column with name "ID".
   *
   * @param options
   *    A map with the options and their values
   * @throws ElementsException
   *    if the input-catalog-file parameter is not given
   * @throws ElementsException
   *    if the input-catalog-file is not a file
   * @throws ElementsException
   *    if both source-id-column-name and source-id-column-index are given
   */
  CatalogConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);

  /// Returns an instance of SourceCatalog::Catalog representing the catalog
  /// described by the options passed to the constructor.
  SourceCatalog::Catalog getCatalog();

protected:

  /**
   * @brief
   * Adds an attribute handler to be used when converting the catalog from
   * Table::Table to SourceCatalog::Catalog, when the getCatalog() method is called
   * @details
   * This method should be used by the subclasses in their constructors, to
   * register their specialized SourceCatalog::AttributeFromRow instances.
   *
   * @param handler The handler to add
   */
  virtual void addAttributeHandler(std::shared_ptr<SourceCatalog::AttributeFromRow> handler) final;

  /**
   * @brief
   * Returns the catalog as a Table::Table
   * @details
   * This method can be used by the subclasses if they need extra information
   * about the table (for example column names) to create their attribute
   * handlers.
   * @return
   *    The catalog as a Table::Table
   */
  virtual const Table::Table& getAsTable() final;

private:

  std::map<std::string, boost::program_options::variable_value> m_options;
  std::vector<std::shared_ptr<SourceCatalog::AttributeFromRow>> m_attribute_handlers;
  std::unique_ptr<Table::Table> m_table_ptr;

};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATIONCATALOGCONFIGURATION_H */

