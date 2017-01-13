/** 
 * @file ComputeModelSedConfiguration.h
 * @date May 28, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZCLI_COMPUTEMODELSEDCONFIGURATION_H
#define	PHZCLI_COMPUTEMODELSEDCONFIGURATION_H

#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "XYDataset/QualifiedName.h"
#include "XYDataset/XYDataset.h"
#include "MathUtils/function/Function.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzConfiguration/IgmConfiguration.h"

namespace Euclid {
namespace PhzConfiguration {

class ComputeModelSedConfiguration : public IgmConfiguration {

public:

  static boost::program_options::options_description getProgramOptions();

  ComputeModelSedConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);

   /**
   * @brief destructor.
   */
   virtual ~ComputeModelSedConfiguration()=default;
   
   PhzDataModel::ModelAxesTuple getGridAxes();
   
   std::map<XYDataset::QualifiedName, XYDataset::XYDataset> getSedMap();
   
   std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>> getReddeningCurveMap();
   
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  
};

}
}

#endif	/* PHZCLI_COMPUTEMODELSEDCONFIGURATION_H */

