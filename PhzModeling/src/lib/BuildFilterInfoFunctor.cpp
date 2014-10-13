/**
 * @file PhzModeling/BuildFilterInfoFunctor.cpp
 * @date Oct 10, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "MathUtils/interpolation/interpolation.h"
#include "MathUtils/function/function_tools.h"
#include "MathUtils/function/Piecewise.h"
#include "ElementsKernel/PhysConstants.h"
#include "PhzDataModel/FilterInfo.h"


namespace Euclid {
namespace PhzModeling {



std::pair<double,double> BuildFilterInfoFunctor::getRange(const Euclid::XYDataset::XYDataset& filter_dataset) const{
return std::make_pair(filter_dataset.begin()->first,(--filter_dataset.end())->first);
}


double BuildFilterInfoFunctor::computeNormalization(const Euclid::XYDataset::XYDataset& filter_dataset, std::pair<double,double> range) const {
  std::vector<std::pair<double, double>> normalized_values {};
  for (auto& sed_pair : filter_dataset) {
    normalized_values.push_back(std::make_pair(sed_pair.first,sed_pair.second/(sed_pair.first*sed_pair.first)));
  }
  auto filter_over_lambda_squar=Euclid::MathUtils::interpolate(normalized_values,Euclid::MathUtils::InterpolationType::LINEAR);
  auto integral_value = Euclid::MathUtils::integrate(*filter_over_lambda_squar,range.first,range.second);
  return integral_value * Elements::Units::c_light/ Elements::Units::angstrom;
}

Euclid::PhzDataModel::FilterInfo BuildFilterInfoFunctor::operator()(const Euclid::XYDataset::XYDataset& filter_dataset) const{
   auto range =getRange(filter_dataset);
   double normalization = computeNormalization(filter_dataset, range);
   auto function_ptr=computeFunction(filter_dataset);
   return Euclid::PhzDataModel::FilterInfo(range,*function_ptr,normalization);
}

std::unique_ptr<Euclid::MathUtils::Function> BuildFilterInfoFunctor::computeFunction(const Euclid::XYDataset::XYDataset& filter_dataset) const{
  return Euclid::MathUtils::interpolate(filter_dataset,Euclid::MathUtils::InterpolationType::LINEAR);
}




} // end of namespace PhzModeling
} // end of namespace Euclid

