/**
 * @file PhzModeling/ApplyFilterFunctor.cpp
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/ApplyFilterFunctor.h"
#include "MathUtils/function/Function.h"
#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

XYDataset::XYDataset ApplyFilterFunctor::operator()(
                              const XYDataset::XYDataset& model,
                              const std::pair<double,double>& filter_range,
                              const MathUtils::Function& filter) const {
  // The data points of the filtered model
  std::vector<std::pair<double, double>> filtered_values {};
  
  // Add the minimum of the range as a zero point, if the first model point is
  // outside of the range
  if (model.front().first <= filter_range.first) {
    filtered_values.emplace_back(filter_range.first, 0.);
  }
  
  // Go through the dataset points and add the ones that are inside the range
  // multiplied by the filter
  for (auto& model_point : model) {
    // Skip values before the min of the range
    if (model_point.first <= filter_range.first) {
      continue;
    }
    // Skip values after the max of the range
    if (model_point.first >= filter_range.second) {
      break;
    }
    filtered_values.emplace_back(model_point.first, model_point.second * filter(model_point.first));
  }
  
  // Add the maximum of the range as a zero point, if the last model point is
  // outside of the range
  if (model.back().first >= filter_range.second) {
    filtered_values.emplace_back(filter_range.second, 0.);
  }
  
  return  XYDataset::XYDataset {std::move(filtered_values)};
}

} // end of namespace PhzModeling
} // end of namespace Euclid

