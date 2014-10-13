/**
 * @file PhzModeling/ModelFluxAlgorithm.h
 * @date Oct 7, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_MODELFLUXALGORITHM_H
#define PHZMODELING_MODELFLUXALGORITHM_H


#include "XYDataset/XYDataset.h"
#include "MathUtils/function/Function.h"
#include "PhzDataModel/FilterInfo.h"

namespace Euclid {
namespace PhzModeling {

/**
* @class Euclid::PhzModeling::ModelFluxAlgorithm
 * @brief
 * Compute the Fluxes of a model relative to a set of filters.
 * @details
 * This algorithm will iterate on a filter container and compute for each of them
 * the flux of a given photometry model by filtering the model and
 * then calculate the total flux.
 *
 */
class ModelFluxAlgorithm{


public:
  /**
   * @brief constructor
   *
   * @param apply_filter_function
   * A std::function taking as input a XYDataset (the photometry model)
   * a pair of double (the filter range) and a MathUtils::Function (the filter)
   * and returning in a XYDataset the filtered model.
   *
   * @param calculate_flux_function
   * A std::function taking as input a XYDataset (the filtered model)
   * and a double (the filter normalization)
   * and returning as a double the total flux of the model.
   *
   */
  ModelFluxAlgorithm(
    const std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,
        const std::pair<double,double>&, const Euclid::MathUtils::Function&)>& apply_filter_function,
    const std::function<double (const Euclid::XYDataset::XYDataset& ,double )>& calculate_flux_function)
  :m_apply_filter_function(apply_filter_function),m_calculate_flux_function(calculate_flux_function){

  }

  /**
   * @brief Move constructor
   */
  ModelFluxAlgorithm(ModelFluxAlgorithm&&)=default;

  /**
     * @brief Move operator
     */
  ModelFluxAlgorithm& operator=(ModelFluxAlgorithm&&)=default;


/**
 * @brief destructor
 */
  virtual ~ModelFluxAlgorithm()=default;

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
  void operator()(const Euclid::XYDataset::XYDataset& model,
                  FilterIterator filter_iterator_begin,
                  FilterIterator filter_iterator_end,
                  FluxIterator flux_iterator) const{
    while (filter_iterator_begin!=filter_iterator_end){
         auto filter_info= *filter_iterator_begin;
         auto filtered_model= m_apply_filter_function(model,filter_info.getRange(),filter_info.getFilter());

         flux_iterator->flux= m_calculate_flux_function(filtered_model,filter_info.getNormalization());
         flux_iterator->error= 0.;

         ++filter_iterator_begin;
         ++flux_iterator;
       }
  }

private:
  const std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,
      const std::pair<double,double>&,const Euclid::MathUtils::Function&)>& m_apply_filter_function;
  const std::function<double(const Euclid::XYDataset::XYDataset& ,double )>& m_calculate_flux_function;
};


} // end of namespace PhzModeling
} // end of namespace Euclid

#endif    /* PHZMODELING_MODELFLUXALGORITHM_H */
