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

Euclid::XYDataset::XYDataset ApplyFilterFunctor::operator()(
  const Euclid::XYDataset::XYDataset& model,
  const std::pair<double,double>& filter_range,
  const Euclid::MathUtils::Function& filter
) const {
    std::vector<std::pair<double, double>> filtered_values {};
    bool has_entered_range=false;
    bool is_first=true;

    for (auto model_iterator=model.begin();model_iterator!= model.end();++model_iterator) {
      // before entering the range
      if (filter_range.first>model_iterator->first){
        is_first=false;
        continue;
      }

      // first entrance get the previous point
      if (!has_entered_range){
        has_entered_range=true;
        if (!is_first){
          --model_iterator;
          filtered_values.emplace_back(model_iterator->first, 0.);
          ++model_iterator;
        }
      }

      //after the range get the last point and break
      if (filter_range.second<model_iterator->first){
        filtered_values.emplace_back(model_iterator->first, 0.);
        break;
      }

      // inside the range
      filtered_values.emplace_back(
          model_iterator->first,
          model_iterator->second*filter(model_iterator->first)
          );
    }

    return  Euclid::XYDataset::XYDataset {std::move(filtered_values)};
}

} // end of namespace PhzModeling
} // end of namespace Euclid

