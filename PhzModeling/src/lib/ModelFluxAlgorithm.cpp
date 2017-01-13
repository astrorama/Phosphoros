/** 
 * @file ModelFluxAlgorithm.cpp
 * @date January 8, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzModeling/ModelFluxAlgorithm.h"

namespace Euclid {
namespace PhzModeling {

ModelFluxAlgorithm::ModelFluxAlgorithm(ApplyFilterFunction apply_filter_function,
                                       IntegrateDatasetFunction integrate_dataset_function)
: m_apply_filter_function {
  std::move(apply_filter_function)
},
m_integrate_dataset_function{std::move(integrate_dataset_function)}
{
}

} // end of namespace PhzModeling
} // end of namespace Euclid