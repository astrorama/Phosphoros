/**
 * @file PhzDataModel/PhotometryGrid.cpp
 * @date Sep 23, 2014
 * @author Florian Dubath
 */


#include "PhzDataModel/PhotometryGrid.h"

namespace Euclid {
namespace GridContainer {

std::unique_ptr<PhzDataModel::PhotometryCellManager> GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::factory(size_t size){
  SourceCatalog::Photometry default_photometry {std::make_shared<std::vector<std::string>>(), {}};
  return std::unique_ptr<PhzDataModel::PhotometryCellManager> {
    new PhzDataModel::PhotometryCellManager(size, default_photometry)
    };
}

size_t GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::size(const PhzDataModel::PhotometryCellManager& vector) {
    return vector.size();
}

PhzDataModel::PhotometryCellManager::iterator GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::begin(PhzDataModel::PhotometryCellManager& vector) {
    return vector.begin();
}

PhzDataModel::PhotometryCellManager::iterator GridCellManagerTraits<PhzDataModel::PhotometryCellManager>::end(PhzDataModel::PhotometryCellManager& vector) {
  return vector.end();
}


}
}
