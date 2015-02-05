/**
 * @file CreatePhzCatalogConfiguration.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_CREATEPHZCATALOGCONFIGURATION_H
#define	PHZCONFIGURATION_CREATEPHZCATALOGCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "PhzOutput/OutputHandler.h"
#include "PhzConfiguration/PhotometricCorrectionConfiguration.h"
#include "PhzConfiguration/PhotometryCatalogConfiguration.h"
#include "PhzConfiguration/PhotometryGridConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class CreatePhzCatalogConfiguration
 *
 * @brief Class defining the input configuration of the CreatePhzCatalog executable
 *
 * @details
 * It defines all the configuration required for the following inputs:
 * - Photometric correction
 * - Photometry catalog
 * - Model photometry grid
 * and it adds the required configuration for the output of the executable.
 */
class CreatePhzCatalogConfiguration : public PhotometricCorrectionConfiguration,
                                      public PhotometryCatalogConfiguration,
                                      public PhotometryGridConfiguration {

public:

  /**
   * @brief
   * Returns the program options for the CreatePhzCatalog executable
   *
   * @details
   * The returned options are the ones defined by the PhotometricCorrectionConfiguration,
   * the PhotometryCatalogConfiguration  and the PhotometryGridConfiguration,
   * plus the parameter \a output-catalog-file, which is the filename of the file
   * to export the PHZ catalog and the \a output-pdf-file parameter which is the filename
   * of the FITS file for the PDFs storage.
   *
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();

  /**
   * @brief
   * Constructs a new CreatePhzCatalogConfiguration instance, for the given options
   *
   * @param options
   *    A map with the options and their values
   */
  CreatePhzCatalogConfiguration(const std::map<std::string,
                              boost::program_options::variable_value>& options);
   /**
   * @brief destructor.
   */
   virtual ~CreatePhzCatalogConfiguration()=default;

  /**
   * @brief
   * Returns the PhzOutput::OutputHandler to be used for producing the output of
   * the CreatePhzCatalog executable
   *
   * @dtails
   * The output handler currently returned is of the type PhzOutput::BestModelCatalog
   * and it stores in an ASCII catalog the best fitted model information.
   *
   * @return
   * The handler to use for storing the output
   */
  std::unique_ptr<PhzOutput::OutputHandler> getOutputHandler();

private:

  std::map<std::string, boost::program_options::variable_value> m_options;

};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CREATEPHZCATALOGCONFIGURATION_H */

