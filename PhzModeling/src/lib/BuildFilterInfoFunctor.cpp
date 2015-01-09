/**
 * @file PhzModeling/BuildFilterInfoFunctor.cpp
 * @date Oct 10, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/function/function_tools.h"
#include "ElementsKernel/PhysConstants.h"
#include "PhzDataModel/FilterInfo.h"


namespace Euclid {
namespace PhzModeling {

/*
 * return the first and the last of the X axis values. If the Y value starts
 * and/or ends with 0s, the range starts at the last value of Lambda for which
 * the filter has a zero value and ends to the last value of lambda for which
 * the filter value is again 0
 */
std::pair<double,double> getRange(const XYDataset::XYDataset& filter_dataset) {
  size_t min = 0;
  size_t current = 0;
  size_t max = 0;
  bool first_non_zero_found = false;
  for (auto& data_pair : filter_dataset) {
    if (!first_non_zero_found) {
      if (data_pair.second == 0) {
        ++min;
      } else {
        first_non_zero_found = true;
      }
    }
    if (data_pair.second != 0) {
      max = current;
    }
    ++current;
  }
  if (min > 0) {
    --min;
  }
  if (max < filter_dataset.size()-1) {
    ++max;
  }
  return std::make_pair((filter_dataset.begin()+min)->first, (filter_dataset.begin()+max)->first);
}

/*
 * take the sampling, multiply it by 1/lambda², then take a linear interpolation
 * and return c * the integral.
 */
double computeNormalization(const XYDataset::XYDataset& filter_dataset, std::pair<double,double> range)  {
  std::vector<std::pair<double, double>> normalized_values {};
  for (auto& sed_pair : filter_dataset) {
    normalized_values.push_back(std::make_pair(sed_pair.first,sed_pair.second/(sed_pair.first*sed_pair.first)));
  }
  auto filter_over_lambda_squar=MathUtils::interpolate(normalized_values,MathUtils::InterpolationType::LINEAR);
  auto integral_value = MathUtils::integrate(*filter_over_lambda_squar,range.first,range.second);
  return integral_value * Elements::Units::c_light/ Elements::Units::angstrom;
}

// create a function as a linear interpolation of the provided filter sampling
std::unique_ptr<MathUtils::Function> computeFunction(const XYDataset::XYDataset& filter_dataset) {
  return MathUtils::interpolate(filter_dataset,MathUtils::InterpolationType::LINEAR);
}

PhzDataModel::FilterInfo BuildFilterInfoFunctor::operator()(const XYDataset::XYDataset& filter_dataset) const {
   auto range =getRange(filter_dataset);
   double normalization = computeNormalization(filter_dataset, range);
   auto function_ptr=computeFunction(filter_dataset);
   return PhzDataModel::FilterInfo(range,*function_ptr,normalization);
}

} // end of namespace PhzModeling
} // end of namespace Euclid

