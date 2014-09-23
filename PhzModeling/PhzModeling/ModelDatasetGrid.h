/** 
 * @file ModelDatasetGrid.h
 * @date August 10, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_MODELDATASETGRID_H
#define	PHZMODELING_MODELDATASETGRID_H

#include "XYDataset/XYDataset.h"
#include "XYDataset/XYDatasetProvider.h"
#include "PhzModeling/ModelDatasetGenerator.h"

namespace Euclid {
namespace PhzModeling {
  
struct ModelDatasetCellManager {
  size_t m_size;
};

}
} // end of namespace Euclid

namespace Euclid {
namespace GridContainer {

template<>
struct GridCellManagerTraits<Euclid::PhzModeling::ModelDatasetCellManager> {
  typedef Euclid::XYDataset::XYDataset data_type;
  typedef Euclid::PhzModeling::ModelDatasetGenerator iterator;
  static std::unique_ptr<Euclid::PhzModeling::ModelDatasetCellManager> factory(size_t size) {
    return std::unique_ptr<Euclid::PhzModeling::ModelDatasetCellManager>{new Euclid::PhzModeling::ModelDatasetCellManager {size}};
  }
  static size_t begin(const Euclid::PhzModeling::ModelDatasetCellManager&) {
    return 0;
  }
  static size_t end(const Euclid::PhzModeling::ModelDatasetCellManager& manager) {
    return manager.m_size;
  }
};

}
} // end of namespace Euclid

namespace Euclid {
namespace PhzModeling {

/**
  * @class Euclid::PhzModeling::ModelDatasetGrid
  * @brief The ModelDataGrid is a PhzGrid of ModelDatasetCellManager
  * @details
  * This grid provide the model iterator and host the logic of how going through
  * the parameter space limiting the re-computation by storing and re-using intermediary results.
  *
  */
class ModelDatasetGrid : public Euclid::PhzDataModel::PhzGrid<ModelDatasetCellManager> {
  
public:
  /**
	* @brief Constructor
	* @details
	* Constructor.
	*
	* @param axes_tuple
	* A ModelAxesTuple defining the axes of the the Grid.
	*
	* @param sed_provider
	* A XYDatasetProvider allowing to access the SED templates.
	*
	* @param reddening_curve_provider
	* A XYDatasetProvider allowing to access the reddening curves.
	*
	*/
  ModelDatasetGrid(Euclid::PhzDataModel::ModelAxesTuple axes_tuple, std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
                   std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider);

  /**
    * @brief begin function for the iteration.
    */
  Euclid::PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator begin();

  /**
    * @brief end function for the iteration.
    */
  Euclid::PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator end();

private:

  Euclid::PhzDataModel::ModelAxesTuple m_axes_tuple {getAxesTuple()};
  size_t m_size;
  std::vector<Euclid::XYDataset::XYDataset> m_sed_data_vector;
  std::vector<ExtinctionFunctor> m_reddening_curve_functor_vector;
};
  


} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_MODELDATASETGRID_H */

