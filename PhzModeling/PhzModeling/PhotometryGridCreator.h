/**
 * @file PhzModeling/PhotometryGridCreator.h
 * @date Oct 17, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_PHOTOMETRYGRIDCREATOR_H
#define PHZMODELING_PHOTOMETRYGRIDCREATOR_H


#include "MathUtils/function/Function.h"
#include "XYDataset/XYDatasetProvider.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/PhotometryGrid.h"


namespace Euclid {

namespace PhzModeling {

/**
 * @class Euclid::PhzModeling::PhotometryGridCreator
 * @brief
 * this class is the algorithm unit which is responsible for calculating the
 * Photometries of the models.
 *
 */
class PhotometryGridCreator{
public:
  /**
     * @brief constructor
     *
     * @details
     * Instanciate and initialize a PhotometryGridCreator. If the parameter space requires
     * a SED or a reddening curve that the corresponding provider are unable to returns
     * an exception will be throw an Euclid Exception. An exception is also throw if the
     * filter provider cannot provide the requested filters.
     *
     * #param parameter_space
     * A ModelAxesTuple defining the SEDs, the redshifts, the reddening curves and the EVB values
     * for which the photometry will be computed.
     *
     * #param filter_name_list
     * A vector<QualifiedName> containing the list (and order) of filter for building the photometry.
     *
     * #param sed_provider
     * A unique_ptr<XYDatasetProvider> allowing to access to the SEDs
     *
     * #param reddening_curve_provider
     * A unique_ptr<XYDatasetProvider> allowing to access to the Reddening curves
     *
     * #param filter_provider
     * A unique_ptr<XYDatasetProvider> allowing to access to the Filters
     *
     */
  PhotometryGridCreator(
      std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
      std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
      std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider);
  /**
   * #brief destructor.
   */
  virtual ~PhotometryGridCreator()=default;

  /**
   * @brief Creates a photometry grid
   * @details
   * Build a photometry grid.
   *
   */
  PhzDataModel::PhotometryGrid createGrid(const PhzDataModel::ModelAxesTuple& parameter_space,
              const std::vector<Euclid::XYDataset::QualifiedName>& filter_name_list);

private:
  
  std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> m_sed_provider;
  std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> m_reddening_curve_provider;
  std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> m_filter_provider;
  
};

}
}

#endif    /* PHZMODELING_PHOTOMETRYGRIDCREATOR_H */
