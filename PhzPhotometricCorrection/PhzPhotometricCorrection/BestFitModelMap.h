/**
 * @file PhzPhotometricCorrection/BestFitModelMap.h
 * @date Jan 14, 2015
 * @author Florian Dubath
 */

#ifndef PHZPHOTOMETRICCORRECTION_BESTFITMODELMAP_H
#define PHZPHOTOMETRICCORRECTION_BESTFITMODELMAP_H

#include <map>
#include "SourceCatalog/SourceAttributes/Photometry.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

/**
 * @class BestFitModelMap
 *
 * @brief A map storing the best fit model for a source catalog
 *
 * @details
 * This map has as key the source ID and contains a pointer on the best fit model.
 */
typedef std::map<long, std::unique_ptr<SourceCatalog::Photometry>> BestFitModelMap;

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid

#endif    /* PHZPHOTOMETRICCORRECTION_BESTFITMODELMAP_H */
