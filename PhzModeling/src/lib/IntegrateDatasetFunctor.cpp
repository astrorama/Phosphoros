/** 
 * @file IntegrateDatasetFunctor.cpp
 * @date January 7, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzModeling/IntegrateDatasetFunctor.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/function/function_tools.h"

namespace Euclid {
namespace PhzModeling {

IntegrateDatasetFunctor::IntegrateDatasetFunctor(MathUtils::InterpolationType type)
                              : m_type {type} { }
                              
double IntegrateDatasetFunctor::operator ()(const XYDataset::XYDataset& dataset,
                                            std::pair<double,double> range) const {
  auto dataset_func = MathUtils::interpolate(dataset, m_type);
  return MathUtils::integrate(*dataset_func, range.first, range.second);
}

} // end of namespace PhzModeling
} // end of namespace Euclid