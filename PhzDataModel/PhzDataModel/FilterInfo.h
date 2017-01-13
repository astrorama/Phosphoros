/**
 * @file PhzDataModel/FilterInfo.h
 * @date Oct 3, 2014
 * @author Florian Dubaths
 */

#ifndef PHZDATAMODEL_FILTERINFO_H
#define	PHZDATAMODEL_FILTERINFO_H


#include "XYDataset/XYDataset.h"
#include "MathUtils/function/Function.h"

namespace Euclid {
namespace PhzDataModel {

/**
 * @class Euclid::PhzDataModel::FilterInfo
 * @brief Object allowing to store a filter along with its range and normalization
 *
 * @details
 * This class contains a filter, its range and its normalization.
 * It is a communication object between the different part of the algorithm.
 *
 */
class FilterInfo{

public:

  /**
   * @brief Constructor.
   *
   * @param filter_range
   * A pair of double (lambda_min,lambda_max) containing the support of the filter.
   * i.e. one consider that the filter is null outside of its range
   *
   * @param filter
   * A Function which values range in the [0,1] interval defining the filter.
   *
   * @param normalization
   * A double which value is the integral (on the range) of the filter time c/lambda².
   *
   */
  FilterInfo(
      const std::pair<double,double>& filter_range,
      const Euclid::MathUtils::Function& filter,
      double normalization
      );

  /**
   * @brief Move Constructor
   */
  FilterInfo(FilterInfo&&)=default;

  /**
   * @brief Move Operator
   */
  FilterInfo& operator=(FilterInfo&&) = default;

  /**
   * @brief Copy Constructor
   */
  FilterInfo(const FilterInfo&);

  /**
   * @brief Copy Operator
   */
  FilterInfo& operator=(const FilterInfo&) = default;

  /**
   * @brief destructor
   */
  virtual ~FilterInfo() = default;

  /**
   * @brief Range getter
   *
   * @return
   * A pair<double,double> containing the filter range
   *
   */
  const std::pair<double,double>& getRange() const;

  /**
   * @brief Filter getter
   *
   * @return
   * A MathUtils::Function& which is the filter function
   *
   */
  const Euclid::MathUtils::Function& getFilter() const;

  /**
   * @brief Normalization getter
   *
   * @return
   * A double which is the filter normalization
   *
   */
  double getNormalization() const;

private:
  std::pair<double,double> m_range;
  std::unique_ptr<Euclid::MathUtils::Function> m_filter;
  double m_normalization;
};

}
}
#endif	/* PHZDATAMODEL_FILTERINFO_H */

