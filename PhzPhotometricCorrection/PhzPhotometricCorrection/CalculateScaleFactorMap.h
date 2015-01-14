/**
 * @file PhzPhotometricCorrection/CalculateScaleFactorMap.h
 * @date Jan 14, 2015
 * @author Florian Dubath
 */

#ifndef PHZPHOTOMETRICCORRECTION_CALCULATESCALEFACTORMAP_H
#define PHZPHOTOMETRICCORRECTION_CALCULATESCALEFACTORMAP_H

#include "SourceCatalog/Catalog.h"
#include "PhzLikelihood/ScaleFactorFunctor.h"
#include "PhzPhotometricCorrection/BestFitModelMap.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

typedef std::map<long, double> ScaleFactorMap;

/**
 * @class CalculateScaleFactorMap
 *
 * @brief A functor which compute the  scale factor from all the sources of a
 * given catalog.
 */
class CalculateScaleFactorMap{
public:

  /**
    * @brief Compute the Scale Factor Map
    *
    * @param sourcesBegin
    * A begin iterator over the sources.
    *
    * @param sourcesEnd
    * The end iterator over the sources.
    *
    * @param modelMap
    * A BestFitmodelMap pairing the sources Id with a pointer to the best fit model.
    *
    * @return A map pairing the source Id with the computed scale factor for
    * its best fitted model.
    */
  template<typename SourceIter>
  ScaleFactorMap operator()(SourceIter& sourcesBegin,SourceIter& sourcesEnd,BestFitModelMap& modelMap) const;

};


} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid


#include "PhzPhotometricCorrection/_impl/CalculateScaleFactorMap.icpp"

#endif    /* PHZPHOTOMETRICCORRECTION_CALCULATESCALEFACTORMAP_H */
