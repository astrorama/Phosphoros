/** 
 * @file LsGridConfiguration.h
 * @date January 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCONFIGURATION_LPHOTOMETRYGRIDCONFIGURATION_H
#define	PHZCONFIGURATION_LPHOTOMETRYGRIDCONFIGURATION_H

#include <set>
#include <PhzDataModel/PhzModel.h>
#include "PhzConfiguration/PhotometryGridConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class LsPhotometryGridConfiguration : public PhotometryGridConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  LsPhotometryGridConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
   /**
   * @brief destructor.
   */
   virtual ~LsPhotometryGridConfiguration()=default;
  
  bool showGeneric();
  
  bool showSedAxis();
  
  bool showReddeningCurveAxis();
  
  bool showEbvAxis();
  
  bool showRedshiftAxis();
  
  std::unique_ptr<std::tuple<size_t,size_t,size_t,size_t>> getCellPhotCoords();

private:

  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

}
}

#endif	/* PHZCONFIGURATION_LPHOTOMETRYGRIDCONFIGURATION_H */

