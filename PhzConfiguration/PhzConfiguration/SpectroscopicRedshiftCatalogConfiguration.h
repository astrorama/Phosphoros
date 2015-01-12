/**
 * @file SpectroscopicRedshiftCatalogConfiguration.h
 * @date January 9, 2015
 * @author Florian Dubath
 */

#ifndef PHZCONFIGURATION_SPECTROSCOPICREDSHIFTCATALOGCONFIGURATION_H
#define	PHZCONFIGURATION_SPECTROSCOPICREDSHIFTCATALOGCONFIGURATION_H

#include "PhzConfiguration/CatalogConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class SpectroscopicRedshiftCatalogConfiguration
 *
 * @brief Configuration class to be used by executables which want to have as
 * input Spectrometric redshift catalogs
 *
 * @details
 * The catalog retrieved when using the getCatalog() method will contain sources
 * with the SpectroscopicRedshiftAttribute, populated as described by the parameters (see
 * the getProgramOptions() documentation).
 */
class SpectroscopicRedshiftCatalogConfiguration : virtual public CatalogConfiguration {

public:

  /**
   * @brief Returns the program options required for spectroscopic redshift catalog input
   *
   * @details
   * These options include all the options defined by the CatalogConfiguration
   * class.
   *
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief
   * Constructs a new SpectroscopicRedshiftCatalogConfiguration instance, for the given
   * options
   *
   * @details
   * One has to provide the reference of the column containing the redshift.
   * This can be done either by providing the column name or the column number, but not both.
   *
   * One can provide the reference of the column containing the redshift error.
   * This can be done either by providing the column name or the column number, but not both.
   *
   * @param options
   *    A map with the options and their values
   * @throws ElementsException
   *    if the given options contains both redshift column name and number
   * @throws ElementsException
   *    if the given options contains both redshift error column name and number
   */
  SpectroscopicRedshiftCatalogConfiguration(const std::map<std::string,
                              boost::program_options::variable_value>& options);

};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_SPECTROSCOPICREDSHIFTCATALOGCONFIGURATION_H */

