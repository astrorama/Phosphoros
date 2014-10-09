/**
 * @file PhzModeling/ApplyFilterFunctor.h
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_APPLYFILTERFUNCTOR_H
#define    PHZMODELING_APPLYFILTERFUNCTOR_H


#include "XYDataset/XYDataset.h"
#include "MathUtils/function/Function.h"

namespace Euclid {
namespace PhzModeling {

/**
 * @class Euclid::PhzModeling::ApplyFilterFunctor
 * @brief
 * This functor is in charge of applying a filter to the Photometry model.
 * @details
 * This functor takes the Photometry model and multiply its components by
 * the provided filter
 */
class ApplyFilterFunctor {

public:
    /**
        * @brief Default Constructor
        */
  ApplyFilterFunctor() = default;

    /**
        * @brief Move Constructor
        */
  ApplyFilterFunctor(ApplyFilterFunctor&&) = default;

    /**
        * @brief Move operator
        */
  ApplyFilterFunctor& operator=(ApplyFilterFunctor&&) = default;

  /**
      * @brief Copy Constructor
    */
  ApplyFilterFunctor(const ApplyFilterFunctor&) = default;

  /**
    * @brief Copy operator
    */
  ApplyFilterFunctor& operator=(const ApplyFilterFunctor&) = default;

    /**
        * @brief Destructor
        */
    virtual ~ApplyFilterFunctor() = default;

    /**
        * @brief Function Call Operator
        * @details
        * Apply the filter to the Photometry Model.
        * Points out of the range are discarded
        * (we keep the first point before and after the range
        * for interpolation purpose)
        *
        * @param model
        * A XYDataset representing the Photometry Model to be filtered.
        *
        * @param filter_range
        * A pair of double defining in which interval
        * the filter assumes non zero values.
        *
        * @param filter
        * A Function which takes a wavelength in input and returns
        * the filter transmission (defined in interval [0,1])
        *
        * @return
        * A XYDataset representing the filtered Photometry Model.
        */
    Euclid::XYDataset::XYDataset operator()(
      const Euclid::XYDataset::XYDataset& model,
      const std::pair<double,double>& filter_range,
      const Euclid::MathUtils::Function& filter
    ) const;

};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif    /* PHZMODELING_APPLYFILTERFUNCTOR_H */
