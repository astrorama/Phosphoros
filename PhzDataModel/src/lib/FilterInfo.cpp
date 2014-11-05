/**
 * @file PhzDataModel/PhzModel.cpp
 * @date Oct 3, 2014
 * @author Florian Dubath
 */

#include "PhzDataModel/FilterInfo.h"
#include "ElementsKernel/PhysConstants.h"
#include "MathUtils/function/function_tools.h"



namespace Euclid {
namespace PhzDataModel {


FilterInfo::FilterInfo(
      const std::pair<double,double>& filter_range,
      const Euclid::MathUtils::Function& filter,
      double normalization
      ):m_range{filter_range}, m_filter{filter.clone()},
  m_normalization{normalization} {
}

const std::pair<double,double>& FilterInfo::getRange() const {
  return m_range;
}

const Euclid::MathUtils::Function& FilterInfo::getFilter() const {
  return *m_filter;
}

double FilterInfo::getNormalization() const {
  return m_normalization;
}

FilterInfo::FilterInfo(const FilterInfo& other)
      : m_range{std::make_pair(other.m_range.first,other.m_range.second)},
        m_filter{(*(other.m_filter)).clone()},
        m_normalization{other.m_normalization} {
}

}
} // end of namespace Euclid


