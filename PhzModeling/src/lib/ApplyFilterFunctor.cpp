/**
 * @file PhzModeling/ApplyFilterFunctor.cpp
 * @date Oct 2, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/ApplyFilterFunctor.h"

namespace Euclid {
namespace PhzModeling {

Euclid::XYDataset::XYDataset ApplyFilterFunctor::operator()(
  const Euclid::XYDataset::XYDataset& model,
  const std::pair<double,double>& filter_range,
  const Euclid::MathUtils::Function& filter
) const {
    std::vector<std::pair<double, double>> filtered_values {};
    for (auto& model_pair : model) {

      if (filter_range.first>model_pair.first
          || filter_range.second<model_pair.first ){
        filtered_values.push_back(std::make_pair(
            model_pair.first,
            0.
            ));
      }
      else {
        filtered_values.push_back(std::make_pair(
            model_pair.first,
            model_pair.second*filter(model_pair.first)
            ));
      }
    }
    return  Euclid::XYDataset::XYDataset {std::move(filtered_values)};
}

} // end of namespace PhzModeling
} // end of namespace Euclid

