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

namespace Euclid {
namespace PhzConfiguration {

class CatalogConfiguration {
  
public:
  
  static boost::program_options::options_description getProgramOptions();
  
  CatalogConfiguration(const std::map<std::string, boost::program_options::variable_value>& options);
  
  SourceCatalog::Catalog getCatalog();
  
protected:
  
  virtual void addAttributeHandler(std::shared_ptr<SourceCatalog::AttributeFromRow> handler) final;
  
private:
  
  std::map<std::string, boost::program_options::variable_value> m_options;
  std::vector<std::shared_ptr<SourceCatalog::AttributeFromRow>> m_attribute_handlers;
  
};

} // end of namespace PhzConfiguration
} // end of namespace Euclid

#endif	/* PHZCONFIGURATIONCATALOGCONFIGURATION_H */

