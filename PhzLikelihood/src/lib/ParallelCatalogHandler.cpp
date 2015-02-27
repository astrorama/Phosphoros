/** 
 * @file ParallelCatalogHandler.cpp
 * @date February 27, 2015
 * @author Nikolaos Apostolakos
 */

#include "PhzLikelihood/ParallelCatalogHandler.h"

namespace Euclid {
namespace PhzLikelihood {

ParallelCatalogHandler::ParallelCatalogHandler(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
                                               const PhzDataModel::PhotometryGrid& phot_grid,
                                               MarginalizationFunction marginalization_func)
        : m_catalog_handler{phot_corr_map, phot_grid, marginalization_func} { }

}
}