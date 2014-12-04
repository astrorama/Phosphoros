/**
 * @file PhzModeling/BuildFilterInfoFunctor.h
 * @date Oct 10, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_BULDFILTERINFOFUNCTOR_H
#define PHZMODELING_BULDFILTERINFOFUNCTOR_H

#include <utility>
#include <memory>

namespace Euclid {
namespace MathUtils {
  class Function;
}

namespace XYDataset {
  class XYDataset;
}

namespace PhzDataModel{
  class FilterInfo;
}

namespace PhzModeling {
/**
 * @class Euclid::PhzModeling::BuildFilterInfoFunctor
 * @brief
 * This functor is in charge of building the filter object.
 * @details
 * This functor takes the sampling of the filter. It extracts the range of the filter,
 * compute the filter normalization (integration of c*filter(lambda)/lambda² )
 * and build an interpolated Function representing the filter.
 */
class BuildFilterInfoFunctor {

public:
  /**
   * @brief Default Constructor
   */
  BuildFilterInfoFunctor() = default;

  /**
   * @brief Move Constructor
   */
  BuildFilterInfoFunctor(BuildFilterInfoFunctor&&) = default;

  /**
   * @brief Move operator
   */
  BuildFilterInfoFunctor& operator=(BuildFilterInfoFunctor&&) = default;

  /**
   * @brief Copy Constructor
   */
  BuildFilterInfoFunctor(const BuildFilterInfoFunctor&) = default;

  /**
    * @brief Copy operator
    */
  BuildFilterInfoFunctor& operator=(const BuildFilterInfoFunctor&) = default;

  /**
   * @brief Destructor
   */
  virtual ~BuildFilterInfoFunctor() = default;

  /**
   * @brief Work out the filter info from the filter data set
   *
   * @param filter_dataset
   * A XYDataset sampling the filter.
   *
   * @return
   * The FilterInfo build from the input filter sampling.
   */
  PhzDataModel::FilterInfo operator()(const XYDataset::XYDataset& filter_dataset) const;


private:
  /*
   * return the first and the last of the X axis values
   */
  std::pair<double,double> getRange(const XYDataset::XYDataset& filter_dataset) const;
  /*
   * create a function as a linear interpolation of the provided filter sampling
   */
  std::unique_ptr<MathUtils::Function> computeFunction(const XYDataset::XYDataset& filter_dataset) const;

  /*
   * take the sampling, multiply it by 1/lambda², then take a linear interpolation
   * and return c * the integral.
   */
  double computeNormalization(const XYDataset::XYDataset& filter_dataset, std::pair<double,double> range) const;

};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif    /* PHZMODELING_BULDFILTERINFOFUNCTOR_H */
