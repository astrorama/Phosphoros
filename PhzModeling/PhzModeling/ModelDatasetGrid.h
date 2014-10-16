/*
 * PhzModeling/ModelDatasetGrid.h
 *
 *  Created on: Sep 29, 2014
 *      Author: fdubath
 */

#ifndef PHZMODELING_MODELDATASETGRID_H_
#define PHZMODELING_MODELDATASETGRID_H_

#include "XYDataset/XYDataset.h"
#include "PhzModeling/ModelDatasetGenerator.h"

namespace Euclid {
namespace PhzModeling {
class ModelDatasetGenerator;

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
    return std::unique_ptr<Euclid::PhzModeling::ModelDatasetCellManager>{
      new Euclid::PhzModeling::ModelDatasetCellManager {size}
    };
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
 * @brief Specialization of the Euclid::PhzDataModel::PhzGrid used to store
 * and walk through the parameter space.
 * The grid do not actually store the SED models but compute them dynamically
 * (through the ModelDatsetGenerator).
 */
class ModelDatasetGrid: public Euclid::PhzDataModel::PhzGrid<ModelDatasetCellManager> {

public:
  /**
   * @brief Constructor
   * @details
   * Constructor.
   *
   * @param parameter_space
   * A ModelAxesTuple defining the axes of the the Grid.
   *
   * @param sed_map
   * A map storing the SED template and giving access to them through
   * the QualifiedName.
   * As the XYDataset cannot be copied one must move this argument in.
   *
   * @param reddening_curve_map
   * A map storing the extinction curve as a Function and giving access to them
   * through the QualifiedName.
   * As the unique_ptr cannot be copied one must move this argument in.
   *
   * @param reddening_function
   * A function used to apply the extinction to a SED
   *
   * @param redshift_function
   * A function used to apply the redshit to a SED
   *
   */
  ModelDatasetGrid(const Euclid::PhzDataModel::ModelAxesTuple& parameter_space,
                   std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> sed_map,
                   std::map<Euclid::XYDataset::QualifiedName,
                     std::unique_ptr<Euclid::MathUtils::Function> > reddening_curve_map,
                   std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,
                       const Euclid::MathUtils::Function&,
                       double)> reddening_function,
                   std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,
                       double)> redshift_function);

  /**
  * @brief begin function for the iteration.
  */
  Euclid::PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator begin();

  /**
  * @brief end function for the iteration.
  */
  Euclid::PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator end();

private:
  size_t m_size;

  std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> m_sed_map;
  std::map<Euclid::XYDataset::QualifiedName,std::unique_ptr<Euclid::MathUtils::Function> > m_reddening_curve_map;
  std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,
      const Euclid::MathUtils::Function&,
      double)> m_reddening_function;
  std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,
double)> m_redshift_function;
};

}
}

#endif /* PHZMODELING_MODELDATASETGRID_H_ */
