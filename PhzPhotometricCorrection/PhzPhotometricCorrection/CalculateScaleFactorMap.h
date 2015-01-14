/**
 * @file PhzPhotometricCorrection/CalculateScaleFactorMap.h
 * @date Jan 14, 2015
 * @author Florian Dubath
 */

#ifndef PHZPHOTOMETRICCORRECTION_CALCULATESCALEFACTORMAP_H
#define PHZPHOTOMETRICCORRECTION_CALCULATESCALEFACTORMAP_H

#include "SourceCatalog/Catalog.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

typedef std::map<int64_t, double> ScaleFactorMap;



/**
 * @class CalculateScaleFactorMap
 *
 * @brief A functor which compute the  scale factor from all the sources of a
 * given catalog.
 */
class CalculateScaleFactorMap{
public:

  /**
   * Definition of the function for calculating the factor with which a model
   * photometry must be scaled to best fit the source. It is a function which
   * gets the iterators over the source and the model photometries and it returns
   * the scale factor as a double. It makes the assumption that the iterators
   * will iterate over the photometries of the same filters and in the same
   * order.
   */
  typedef std::function<double(SourceCatalog::Photometry::const_iterator source_begin,
                               SourceCatalog::Photometry::const_iterator source_end,
                               SourceCatalog::Photometry::const_iterator model_begin)
                       > ScaleFactorCalc;


  CalculateScaleFactorMap(ScaleFactorCalc scaleFactorFunction);

  /**
    * @brief Compute the Scale Factor Map
    *
    * @tparam SourceIter The type of the iterator over the source objects
    *
    * @tparam ModelPhotPtr A type which can be dereferenced to a Photometry object
    *    representing the model photometry
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
  template<typename SourceIter,typename ModelPhotPtr>
  ScaleFactorMap operator()(
      SourceIter source_begin,
      SourceIter source_end,
      const std::map<int64_t, ModelPhotPtr>& model_phot_map) const;
private:
  ScaleFactorCalc m_scale_factor_function;
};


} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid


#include "PhzPhotometricCorrection/_impl/CalculateScaleFactorMap.icpp"

#endif    /* PHZPHOTOMETRICCORRECTION_CALCULATESCALEFACTORMAP_H */
