/** 
 * @file PhzDataModel/PhotometryGrid.cpp
 * @date Sep 23, 2014
 * @author Florian Dubath
 */


#include "PhzDataModel/PhotometryGrid.h"


  std::unique_ptr<Euclid::PhzDataModel::PhotometryCellManager> Euclid::GridContainer::GridCellManagerTraits<Euclid::PhzDataModel::PhotometryCellManager>::factory(size_t size){
    Euclid::SourceCatalog::Photometry default_photometry {std::make_shared<std::vector<std::string>>(), {}};
    return std::unique_ptr<Euclid::PhzDataModel::PhotometryCellManager> {
      new Euclid::PhzDataModel::PhotometryCellManager(size, default_photometry)
    };
  }

  size_t Euclid::GridContainer::GridCellManagerTraits<Euclid::PhzDataModel::PhotometryCellManager>::size(const Euclid::PhzDataModel::PhotometryCellManager& vector) {
    return vector.size();
  }

  Euclid::PhzDataModel::PhotometryCellManager::iterator Euclid::GridContainer::GridCellManagerTraits<Euclid::PhzDataModel::PhotometryCellManager>::begin(Euclid::PhzDataModel::PhotometryCellManager& vector) {
    return vector.begin();
  }

  Euclid::PhzDataModel::PhotometryCellManager::iterator Euclid::GridContainer::GridCellManagerTraits<Euclid::PhzDataModel::PhotometryCellManager>::end(Euclid::PhzDataModel::PhotometryCellManager& vector) {
    return vector.end();
  }



