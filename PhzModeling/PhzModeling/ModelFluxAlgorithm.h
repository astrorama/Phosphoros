/**
 * @file PhzModeling/ModelFluxAlgorithm.h
 * @date Oct 7, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_MODELFLUXALGORITHM_H
#define PHZMODELING_MODELFLUXALGORITHM_H

#include "PhzModeling/IntegrateDatasetFunctor.h"

namespace Euclid {
namespace MathUtils {
  class Function;
}

namespace XYDataset {
  class XYDataset;
}

namespace PhzModeling {

/**
* @class PhzModeling::ModelFluxAlgorithm
 * @brief
 * Compute the Fluxes of a model relative to a set of filters.
 * @details
 * This algorithm will iterate on a filter container and compute for each of them
 * the flux of a given photometry model by filtering the model and
 * then calculate the total flux.
 *
 */
class ModelFluxAlgorithm {


public:
  
  /// Signature of the functions which is used for applying the filter
  typedef std::function<XYDataset::XYDataset (const XYDataset::XYDataset& dataset,
                                              const std::pair<double,double>& range,
                                              const MathUtils::Function& filter)
                       > ApplyFilterFunction;
  
  /// Signature of the function which is used for integrating the model dataset
  typedef std::function<double (const XYDataset::XYDataset& dataset,
                                std::pair<double, double> range)
                       > IntegrateDatasetFunction;
  
  /**
   * @brief constructor
   *
   * @param apply_filter_function
   * A std::function taking as input a XYDataset (the photometry model)
   * a pair of double (the filter range) and a MathUtils::Function (the filter)
   * and returning in a XYDataset the filtered model.
   *
   * @param integrate_dataset_function
   * An std::function for performing integration of XYDatasets, which will be
   * used for calculating the integral of the filtered dataset. It defaults on
   * linear interpolation integration.
   *
   */
  ModelFluxAlgorithm(ApplyFilterFunction apply_filter_function,
      IntegrateDatasetFunction integrate_dataset_function=IntegrateDatasetFunctor{MathUtils::InterpolationType::LINEAR});

  /**
   * @brief  Function Call Operator
   * @details
   * Run the computation of the total flux for all the filter values of the input iterator
   * applied on the provided model and store the result in the out iterator.
   *
   * @param model
   * A XYDataset representing the photometric model for wich one want to compute the total fluxes.
   *
   * @param filter_iterator_begin
   * An iterator over a FilterInfo container containg the list of filter to be applied.
   *
   * @param filter_iterator_end
   * End iterator on the FilterInfo container.
   *
   * @param flux_iterator
   * An iterator over a FluxErrorPair on which the computed total fluxes have to be stored.
   *
   */
  template<typename FilterIterator,typename FluxIterator>
  void operator()(const XYDataset::XYDataset& model,
                  FilterIterator filter_iterator_begin,
                  FilterIterator filter_iterator_end,
                  FluxIterator flux_iterator) const;

private:
  ApplyFilterFunction m_apply_filter_function;
  IntegrateDatasetFunction m_integrate_dataset_function;
};


} // end of namespace PhzModeling
} // end of namespace Euclid

#include "PhzModeling/_impl/ModelFluxAlgorithm.icpp"

#endif    /* PHZMODELING_MODELFLUXALGORITHM_H */
