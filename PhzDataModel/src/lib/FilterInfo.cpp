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
      ):m_range{filter_range}{
  m_filter=filter.clone();
  m_normalization=normalization;
}

const std::pair<double,double>& FilterInfo::getRange(){
  return m_range;
}

const Euclid::MathUtils::Function& FilterInfo::getFilter(){
  return *m_filter;
}

double FilterInfo::getNormalization(){
  return m_normalization;
}

}
} // end of namespace Euclid


