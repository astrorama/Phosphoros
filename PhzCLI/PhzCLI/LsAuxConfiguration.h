/** 
 * @file LsAuxConfiguration.h
 * @date May 28, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCLI_LSAUXCONFIGURATION_H
#define	PHZCLI_LSAUXCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "XYDataset/XYDatasetProvider.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class LsAuxConfiguration : public virtual PhosphorosPathConfiguration {

public:

  static boost::program_options::options_description getProgramOptions();

  LsAuxConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);

   /**
   * @brief destructor.
   */
   virtual ~LsAuxConfiguration()=default;
   
   std::unique_ptr<XYDataset::XYDatasetProvider> getDatasetProvider();
   
   std::string getGroup();
   
   std::string getDatasetToShow();
   
   bool showData();
   
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

}
}


#endif	/* PHZCLI_LSAUXCONFIGURATION_H */

