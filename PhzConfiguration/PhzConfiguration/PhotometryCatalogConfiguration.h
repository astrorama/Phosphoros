/** 
 * @file PhotometryCatalogConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_PHOTOMETRYCATALOGCONFIGURATION_H
#define	PHZCONFIGURATION_PHOTOMETRYCATALOGCONFIGURATION_H

#include "PhzConfiguration/CatalogConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class PhotometryCatalogConfiguration
 * 
 * @brief Configuration class to be used by executables which want to have as
 * input photometric catalogs
 * 
 * @details
 * The catalog retrieved when using the getCatalog() method will contain sources
 * with the PhotometryAttribute, populated as described by the parameters (see
 * the getProgramOptions() documentation).
 */
class PhotometryCatalogConfiguration : virtual public CatalogConfiguration {
  
public:
  
  /**
   * @brief Returns the program options required for photometric catalog input
   * 
   * @details
   * These options include all the options defined by the CatalogConfiguration
   * class, with the extra option \a filter-name-mapping. This option is a
   * vector of strings with elements that do the mapping between the filter
   * qualified names and the column names with the flux and error values,
   * separated with spaces.
   * 
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();
  
  /**
   * @brief
   * Constructs a new PhotometryCatalogConfiguration instance, for the given
   * options
   * 
   * @details
   * If no filter mapping is provided or if any of the filter mapping does not
   * follow the format of three space separated strings an exception is thrown.
   * 
   * @param options
   *    A map with the options and their values
   * @throws ElementsException
   *    if the given options do not contain any filter mapping
   * @throws ElementsException
   *    if any filter mapping does not follow the format "filter flux_name error_name"
   */
  PhotometryCatalogConfiguration(const std::map<std::string,
                              boost::program_options::variable_value>& options);
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_PHOTOMETRYCATALOGCONFIGURATION_H */

