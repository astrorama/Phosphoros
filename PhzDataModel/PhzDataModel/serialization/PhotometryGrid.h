/** 
 * @file PhotometryGrid.h
 * @date August 21, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRID_H
#define	PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRID_H

#include <boost/serialization/split_free.hpp>
#include "ElementsKernel/Exception.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "GridContainer/serialization/GridContainer.h"
#include "PhzDataModel/PhotometryGrid.h"

namespace boost {
namespace serialization {

template<typename Archive>
void save(Archive& ar, const Euclid::PhzDataModel::PhotometryGrid& grid, const unsigned int) {
  size_t size = grid.size();
  if (size == 0) {
    throw Elements::Exception() << "Serialization of empty PhotometryGrid is not supported";
  }
  // We store the filter names only once. We require that all photometries have
  // the same filters
  std::vector<std::string> filter_names {};
  for (auto iter=(*grid.begin()).begin(); iter!=(*grid.begin()).end(); ++iter) {
    filter_names.push_back(iter.filterName());
  }
  ar << filter_names;
  // We store the flux and error values for each photometry and we check if the
  // filters are matching the common ones
  for (auto& photometry : grid) {
    if (photometry.size() != filter_names.size()) {
      throw Elements::Exception() << "Serialization of grids of Photometries with "
                                << "different filters is not supported";
    }
    auto filt_iter = filter_names.begin();
    for (auto phot_iter=photometry.begin(); phot_iter!=photometry.end(); ++phot_iter, ++ filt_iter) {
      if (*filt_iter != phot_iter.filterName()) {
        throw Elements::Exception() << "Serialization of grids of Photometries with "
                                  << "different filters is not supported";
      }
      ar << (*phot_iter).flux;
      ar << (*phot_iter).error;
    }
  }
}

template<typename Archive>
void load(Archive& ar, Euclid::PhzDataModel::PhotometryGrid& grid, const unsigned int) {
  std::vector<std::string> filter_names;
  ar >> filter_names;
  auto filter_names_ptr = std::make_shared<std::vector<std::string>>(std::move(filter_names));
  for (auto& cell : grid) {
    std::vector<Euclid::SourceCatalog::FluxErrorPair> phot_values;
    for (int i=0; i< filter_names_ptr->size(); ++i) {
      double flux;
      double error;
      ar >> flux >> error;
      phot_values.push_back({flux, error});
    }
    cell = Euclid::SourceCatalog::Photometry {filter_names_ptr, std::move(phot_values)};
  }
}

/// This method is specialized for the PhotometryGrid to avoid storing multiple
/// times the filter names of the photometries.
template<typename Archive>
void serialize(Archive& ar, Euclid::PhzDataModel::PhotometryGrid& t, const unsigned int version) {
  split_free(ar, t, version);
}

} /* end of namespace serialization */
} /* end of namespace boost */

#endif	/* PHZDATAMODEL_SERIALIZATION_PHOTOMETRYGRID_H */

