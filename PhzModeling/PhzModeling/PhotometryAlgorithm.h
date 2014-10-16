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
#include "MathUtils/function/Function.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/FilterInfo.h"
#include "PhzModeling/BuildFilterInfoFunctor.h"
#include "PhzModeling/ModelFluxAlgorithm.h"

namespace Euclid {
namespace PhzModeling {

template<typename ModelFluxAlgo>
class PhotometryAlgorithm{
public:

  PhotometryAlgorithm(ModelFluxAlgo model_flux_algorithm,
      std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> filter_map,
      std::vector<Euclid::XYDataset::QualifiedName> filter_name_list);



  virtual ~PhotometryAlgorithm()=default;

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
