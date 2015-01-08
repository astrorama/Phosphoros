/** 
 * @file IntegrateDatasetFunctor.h
 * @date January 7, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_INTEGRATEDATASETFUNCTOR_H
#define	PHZMODELING_INTEGRATEDATASETFUNCTOR_H

#include <utility>
#include "MathUtils/interpolation/interpolation.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

/**
 * @class IntegrateDatasetFunctor
 * 
 * @brief
 * Functor responsible for calculating the integral of a function represented by
 * an XYDataset::XYDataset points
 * 
 * @details
 * This class performs the integration by using interpolation. The type of the
 * interpolation can be controlled with the parameters of the constructor.
 */
class IntegrateDatasetFunctor {
  
public:
  
  /**
   * @brief Constructs a new IntegrateDatasetFunctor instance, which will use the
   * given type of interpolation
   * @param type The type of interpolation to use
   */
  IntegrateDatasetFunctor(MathUtils::InterpolationType type);
  
  /**
   * @brief Returns the integral of the given dataset in the given range
   * @details
   * The function outside the dataset range is assumed to be zero.
   * @param dataset The dataset to integrate
   * @param range The range to integrate
   * @return The integral value
   */
  double operator()(const XYDataset::XYDataset& dataset, std::pair<double, double> range) const;
  
private:
  
  MathUtils::InterpolationType m_type;
  
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_INTEGRATEDATASETFUNCTOR_H */

