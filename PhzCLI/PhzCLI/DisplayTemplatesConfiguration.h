/** 
 * @file DisplayTemplateConfiguration.h
 * @date January 26, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCLI_DISPLAYTEMPLATESCONFIGURATION_H
#define	PHZCLI_DISPLAYTEMPLATESCONFIGURATION_H

#include <set>
#include <PhzDataModel/PhzModel.h>
#include "PhzConfiguration/PhotometryGridConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class DisplayTemplatesConfiguration : public PhotometryGridConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  DisplayTemplatesConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
   /**
   * @brief destructor.
   */
   virtual ~DisplayTemplatesConfiguration()=default;
  
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

#endif	/* PHZCLI_DISPLAYTEMPLATESCONFIGURATION_H */

