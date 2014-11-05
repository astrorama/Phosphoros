/**
 * @file PhzModeling/CalculateFluxFunctor.h
 * @date Oct 3, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_CALCULATEFLUXFUNCTOR_H
#define PHZMODELING_CALCULATEFLUXFUNCTOR_H

#include "MathUtils/interpolation/interpolation.h"


namespace Euclid {

namespace XYDataset {
  class XYDataset;
}

namespace PhzModeling {

/**
 * @class Euclid::PhzModeling::CalculateFluxFunctor
 * @brief
 * This functor is in charge of computing the total flux of
 * a filtered Photometry model.
 * @details
 * This functor takes the Photometry model and integrate the model flux over
 * the wavelegth parameter.
 */
class CalculateFluxFunctor {

public:
  
  CalculateFluxFunctor(MathUtils::InterpolationType interp_type);

    /**
        * @brief Function Call Operator
        * @details
        * Perform the integration of the flux over the wavelength
        *
        * @param model
        * A XYDataset representing the filtered Photometry Model
        * to be integrated.
        *
        * @param normalization
        * A double accounting for the filter normalization.
        *
        * @return
        * A double representing the total flux of the Model.
        */
    double operator()(
        const Euclid::XYDataset::XYDataset& model,
        double normalization
        ) const;
    
private:
  
  MathUtils::InterpolationType m_interp_type;

};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif    /* PHZMODELING_CALCULATEFLUXFUNCTOR_H */
