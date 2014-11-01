/**
 * @file PhzModeling/ExtinctionFunctor.cpp
 * @date Sep 26, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/ExtinctionFunctor.h"
#include "XYDataset/XYDataset.h"


Euclid::XYDataset::XYDataset
 Euclid::PhzModeling::ExtinctionFunctor::operator()(const Euclid::XYDataset::XYDataset& sed,
 const Euclid::MathUtils::Function& reddening_curve, double ebv) const {
  std::vector<std::pair<double, double>> reddened_values {};
  for (auto& sed_pair : sed) {
    double exponent = -0.4 * reddening_curve(sed_pair.first) * ebv;
    double reddened_value = sed_pair.second * std::pow(10, exponent);
    reddened_values.emplace_back(std::make_pair(sed_pair.first, reddened_value));
  }
  return std::move(reddened_values);
}
