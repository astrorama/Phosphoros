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
 * @class Euclid::PhzModeling::PhotometryAlgorithm
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
   * #param filter_map
   * A map<QualifiedName,XYDataset> which contains the filters sampling.
   *
   * #param filter_name_list
   * A vector<QualifiedName> storing the list ( and the order) of filters to be
   * used to build the photometry.
   */
  PhotometryAlgorithm(ModelFluxAlgo model_flux_algorithm,
      std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> filter_map,
      std::vector<Euclid::XYDataset::QualifiedName> filter_name_list);

  /**
   * #brief destructor.
   */
  virtual ~PhotometryAlgorithm()=default;

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
  void operator()(ModelIterator model_begin,ModelIterator model_end,PhotometryIterator photometry_begin) const;

private:
  ModelFluxAlgo m_model_flux_agorithm;
  std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> m_filter_map;
  std::vector<Euclid::XYDataset::QualifiedName> m_filter_name_list;
  std::vector<Euclid::PhzDataModel::FilterInfo> m_filter_info_vector;
  std::shared_ptr<std::vector<std::string>> m_filter_name_shared_vector;

  BuildFilterInfoFunctor filter_info_functor{};

  std::vector<Euclid::PhzDataModel::FilterInfo> manageFilter();

  std::shared_ptr<std::vector<std::string>> createSharedPointers();

};

#define PHOTOMETRYALGO_IMPL
#include "PhzModeling/_impl/PhotometryAlgorithm.icpp"
#undef PHOTOMETRYALGO_IMPL




} // end of namespace PhzModeling
} // end of namespace Euclid

#endif    /* PHZMODELING_MODELFLUXALGORITHM_H */
