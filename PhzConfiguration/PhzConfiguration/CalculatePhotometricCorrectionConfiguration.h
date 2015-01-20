/** 
 * @file CalculatePhotometricCorrectionConfiguration.h
 * @date January 19, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_CALCULATEPHOTOMETRICCORRECTIONCONFIGURATION_H
#define	PHZCONFIGURATION_CALCULATEPHOTOMETRICCORRECTIONCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzConfiguration/PhotometryCatalogConfiguration.h"
#include "PhzConfiguration/SpectroscopicRedshiftCatalogConfiguration.h"
#include "PhzConfiguration/PhotometryGridConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

/**
 * @class CalculatePhotometricCorrectionConfiguration
 * 
 * @brief 
 * Configuration class to be used by the CalculatePhotometricCorrection executable
 * 
 * @details
 * This class defines as input a catalog which contains photometric and
 * spectroscopic information (by inheriting the PhotometryCatalogConfiguration
 * and SpectroscopicRedshiftCatalogConfiguration classes) and a grid with the
 * photometries of the models (by inheriting the PhotometryGridConfiguration).
 * In addition of the parameters defined by these classes, it addes the parameter
 * \b output-phot-corr-file, which is the file where the photometric corrections
 * will be stored, and it provides a handler function for performing the output.
 */
class CalculatePhotometricCorrectionConfiguration : virtual public PhotometryCatalogConfiguration,
                                                    virtual public SpectroscopicRedshiftCatalogConfiguration,
                                                    public PhotometryGridConfiguration {
  
public:
  
  /// The type of the function which handles the output of the photometric
  /// correction map
  typedef std::function<void(const PhzDataModel::PhotometricCorrectionMap&)> OutputFunction;
  
  /**
   * @brief
   * Returns the program options required by the CalculatePhotometricCorrection
   * executable
   * 
   * @details
   * These options are the ones required by the PhotometryCatalogConfiguration,
   * SpectroscopicRedshiftCatalogConfiguration and PhotometryGridConfiguration
   * classes, with the folowing extra option:
   * - output-phot-corr-file : The file to export the calculated photometric correction
   * 
   * @return A boost::program_options::options_description object describing
   * the program options
   */
  static boost::program_options::options_description getProgramOptions();
  
  /**
   * @brief
   * Constructs a new CalculatePhotometricCorrectionConfiguration instance
   * @details
   * The option output-phot-corr-file is obligatory and it must be a path where
   * the executable can create the photometric correction file.
   * 
   * @param options
   *    A map with the options and their values
   * @throws ElementsException
   *    of the output-phot-corr-file is missing
   * @throws ElementsException
   *    if the output-phot-corr-file cannot be created
   */
  CalculatePhotometricCorrectionConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
  /**
   * @brief
   * Returns a function which can be used for storing the output photometric
   * corrections.
   * @return The output function
   */
  OutputFunction getOutputFunction();
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATION_CALCULATEPHOTOMETRICCORRECTIONCONFIGURATION_H */
