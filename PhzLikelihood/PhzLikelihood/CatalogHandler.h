/** 
 * @file CatalogHandler.h
 * @date December 3, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZLIKELIHOOD_CATALOGHANDLER_H
#define	PHZLIKELIHOOD_CATALOGHANDLER_H

#include "PhzDataModel/PhotometricCorrectionMap.h"
#include "PhzDataModel/PhotometryGrid.h"
#include "PhzOutput/OutputHandler.h"
#include "PhzLikelihood/SourcePhzFunctor.h"

namespace Euclid {
namespace PhzLikelihood {

class CatalogHandler {
  
public:
  
  CatalogHandler(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                 PhzDataModel::PhotometryGrid phot_grid);
  
  template<typename SourceIter>
  void handleSources(SourceIter source_begin, SourceIter source_end,
                     std::unique_ptr<PhzOutput::OutputHandler> out_handler_ptr) const;
  
private:
  
  SourcePhzFunctor m_source_phz_func;
  
};

} // end of namespace PhzLikelihood
} // end of namespace Euclid

#include "PhzLikelihood/_impl/CatalogHandler.icpp"

#endif	/* PHZLIKELIHOOD_CATALOGHANDLER_H */

