/**
 * @file PhzModeling/CalculateFluxFunctor.cpp
 * @date Oct 3, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/CalculateFluxFunctor.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/function/function_tools.h"

namespace Euclid {
namespace PhzModeling {

CalculateFluxFunctor::CalculateFluxFunctor(MathUtils::InterpolationType interp_type)
      : m_interp_type{interp_type} {
}


double CalculateFluxFunctor::operator()(
  const Euclid::XYDataset::XYDataset& model,
  double normalization
) const {
  auto interpolated_model = Euclid::MathUtils::interpolate(model, m_interp_type);
  return Euclid::MathUtils::integrate(*interpolated_model, model.begin()->first, (--model.end())->first)/normalization;
}

} // end of namespace PhzModeling
} // end of namespace Euclid

