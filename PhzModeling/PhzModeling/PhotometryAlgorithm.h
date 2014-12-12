/**
 * @file PhzModeling/PhotometryAlgorithm.h
 * @date Oct 9, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_PHOTOMETRYALGORITHM_H
#define PHZMODELING_PHOTOMETRYALGORITHM_H

#include <map>

#include "XYDataset/XYDataset.h"
#include "XYDataset/QualifiedName.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/FilterInfo.h"
#include "PhzModeling/BuildFilterInfoFunctor.h"

namespace Euclid {
namespace PhzModeling {

/**
 * @class PhotometryAlgorithm
 * @brief
 * Calculates the Photometry of Models, for a set of filters.
 * @details
 * This algorithm will iterate on a Model container and compute for each of them
 * the flux for each of the provided filters.
 */
template<typename ModelFluxAlgo>
class PhotometryAlgorithm{

public:

  /**
   * @brief constructor
   *
   * @details
   * Instanciate and initialize a PhotometryAlgorithm. The algorithm will then
   * use the filters which names are provides in the filters name list in order
   * to compute the photometry. If the XYDataset representing the requested
   * filter is not available in the filter map the initialization throw an Euclid Exception.
   *
   * @param filter_map
   * A map<QualifiedName,XYDataset> which contains the filters sampling.
   *
   * @param filter_name_list
   * A vector<QualifiedName> storing the list ( and the order) of filters to be
   * used to build the photometry.
   */
  PhotometryAlgorithm(ModelFluxAlgo model_flux_algorithm,
      const std::map<XYDataset::QualifiedName,XYDataset::XYDataset>& filter_map,
      const std::vector<XYDataset::QualifiedName>& filter_name_list);

  /**
   * @brief destructor.
   */
  virtual ~PhotometryAlgorithm()=default;

  /**
   * @brief Function Call Operator
   * @details
   * Iterate over the model and for each of them compute the photometry. Version
   * which can be monitored, to be used for parallel programming.
   *
   * @param model_begin
   * An iterator over a XYDataset container giving access to the models.
   *
   * @param model_end
   * end iterator for the model container
   *
   * @param photometry_begin
   * An iterator over a Photometry container into which the computed photometry will be stored.
   *
   * @param monitor
   * A reference to an object to be notified with the progress of the job. Its
   * prefix ++ operator will be called every time a model is handled.
   */
  template<typename ModelIterator,typename PhotometryIterator, typename Monitor>
  void operator()(ModelIterator model_begin, ModelIterator model_end,
                  PhotometryIterator photometry_begin, Monitor& monitor) const;

  /**
   * @brief Function Call Operator
   * @details
   * Iterate over the model and for each of them compute the photometry
   *
   * @param model_begin
   * An iterator over a XYDataset container giving access to the models.
   *
   * @param model_end
   * end iterator for the model container
   *
   * @param photometry_begin
   * An iterator over a Photometry container into which the computed photometry will be stored.
   */
  template<typename ModelIterator,typename PhotometryIterator>
  void operator()(ModelIterator model_begin, ModelIterator model_end,
                  PhotometryIterator photometry_begin) const;

private:
  ModelFluxAlgo m_model_flux_agorithm;
  std::vector<PhzDataModel::FilterInfo> m_filter_info_vector;
  std::shared_ptr<std::vector<std::string>> m_filter_name_shared_vector;

};

template<typename ModelFluxAlgo>
PhotometryAlgorithm<ModelFluxAlgo> createPhotometryAlgorithm(ModelFluxAlgo model_flux_algorithm,
      const std::map<XYDataset::QualifiedName,XYDataset::XYDataset>& filter_map,
      const std::vector<XYDataset::QualifiedName>& filter_name_list) {
  return PhotometryAlgorithm<ModelFluxAlgo>(model_flux_algorithm, filter_map, filter_name_list);
}

} // end of namespace PhzModeling
} // end of namespace Euclid

#define PHOTOMETRYALGO_IMPL
#include "PhzModeling/_impl/PhotometryAlgorithm.icpp"
#undef PHOTOMETRYALGO_IMPL

#endif    /* PHZMODELING_MODELFLUXALGORITHM_H */
