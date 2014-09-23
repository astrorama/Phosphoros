/** 
 * @file ModelDatasetGenerator.h
 * @date August 10, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_MODELDATASETGENERATOR_H
#define	PHZMODELING_MODELDATASETGENERATOR_H

#include "XYDataset/XYDataset.h"
#include "PhzDataModel/PhzModel.h"
#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/RedshiftFunctor.h"

namespace Euclid {
namespace PhzModeling {

/**
 * @class Euclid::PhzModeling::ModelDatasetGenerator
 * @brief Provides the Model for the current index of the Model parameter space iterator
 */
class ModelDatasetGenerator {
  
public:
  /**
	* @brief Constructor
	* @details
	* Constructor.
	*
	* @param axes_tuple
	* A ModelAxesTuple defining the axes of the the Grid.
	*
	* @param sed_data_vector
	* A vector<XYDataset> containing the SED templates.
	*
	* @param reddening_curve_functor_vector
	* A vector<ExtinctionFunctor> containing the extinction functors.
	*
	* @param current_index
	* The current index.
	*/
  ModelDatasetGenerator(const Euclid::PhzDataModel::ModelAxesTuple& axes_tuple,
                        const std::vector<Euclid::XYDataset::XYDataset>& sed_data_vector,
                        const std::vector<ExtinctionFunctor>& reddening_curve_functor_vector,
                        size_t current_index);
  /**
    * @brief Copy constructor.
    */
  ModelDatasetGenerator(const ModelDatasetGenerator& other);
  
  /**
    * @brief Assignment operator. Set the current index to the one of the other instance.
    *
    * @param other
    * A ModelDatasetGenerator: the index has to be copied from.
    *
    */
  ModelDatasetGenerator& operator=(const ModelDatasetGenerator& other);
  
  /**
    * @brief Assignment operator. Set the current index to the provided one and reset the current reddened SED.
    *
    * @param other
    * A size_t: the new index.
    *
    */
  ModelDatasetGenerator& operator=(size_t other);
  
  /**
    * @brief Increment operator. Increase the current index and reset the current model.
    */
  ModelDatasetGenerator& operator++();
  
  /**
    * @brief Increment by addition operator. Increase the current index by the provided value and reset the current reddened SED.
    *
    * @param n
    * An int: the index increment.
    */
  ModelDatasetGenerator& operator+=(int n) ;
  
  /**
    * @brief Subtraction operator. Decrease the current index by the provided value and reset the current reddened SED.
    *
    * @param n
    * A size_t: the index decrement.
    */
  int operator-(size_t other) const;
  
  /**
    * @brief Subtraction operator. Decrease the current index by the index of the provided ModelDatasetGenerator and reset the current reddened SED.
    *
    * @param other
    * A ModelDatasetGenerator from which index value the current index will be decremented.
    */
  int operator-(const ModelDatasetGenerator& other) const;
  
  /**
    * @brief Equality operator. True if the provided value match the current index.
    *
    * @param other
    * A size_t the current index will be compared to.
    */
  bool operator==(size_t other) const;
  
  /**
    * @brief Equality operator. Test the equality of the current indexes.
    *
    * @param other
    * A ModelDatasetGenerator from which the the current index will be compared to.
    */
  bool operator==(const ModelDatasetGenerator& other) const;
  
  /**
    * @brief Inequality operator. False if the provided value match the current index.
    *
    * @param other
    * An size_t the current index will be compared to.
    */
  bool operator!=(size_t other) const;
  
  /**
    * @brief Inequality operator. Test the inequality of the current indexes.
    *
    * @param other
    * A ModelDatasetGenerator from which the the current index will be compared to.
    */
  bool operator!=(const ModelDatasetGenerator& other) const;
  
  /**
    * @brief Greater than operator. True if the current index is bigger than the provided value.
    *
    * @param other
    * A size_t the current index will be compared to.
    */
  bool operator>(size_t other) const;
  
  /**
    * @brief Greater than operator. True if the current index is bigger than the current index of the provided Generator.
    *
    * @param other
    * A ModelDatasetGenerator the current index will be compared to.
    */
  bool operator>(const ModelDatasetGenerator& other) const;
  
  /**
    * @brief Lower than operator. True if the current index is smaller than the provided value.
    *
    * @param other
    * A size_t the current index will be compared to.
    */
  bool operator<(size_t other) const;
  
  /**
    * @brief Lower than operator. True if the current index is smaller than the current index of the provided Generator.
    *
    * @param other
    * A ModelDatasetGenerator the current index will be compared to.
    */
  bool operator<(const ModelDatasetGenerator& other) const;
  
  /**
    * @brief Indirection operator.
    * @details
    * Compute the Model SED for the current index and return a reference on it
    *
    * @return
    * A XYDataset representing the Model for the current index
    */
  Euclid::XYDataset::XYDataset& operator*();
  
private:
  
  // An object to convert the parameter space coordinates to a long index and vice versa
  decltype(Euclid::GridContainer::makeGridIndexHelper(std::declval<Euclid::PhzDataModel::ModelAxesTuple>())) m_index_helper;
  const Euclid::PhzDataModel::ModelAxesTuple& m_axes_tuple;
  
  // The current long 1D index
  size_t m_current_index;
  size_t m_size;
  
  // The indices of the parameters last calculated
  size_t m_current_sed_index {0};
  size_t m_current_reddening_curve_index {0};
  size_t m_current_ebv_index {0};
  size_t m_current_z_index {0};
  
  // The latest calculated reddened and redshifted SEDs
  std::unique_ptr<Euclid::XYDataset::XYDataset> m_current_reddened_sed;
  std::unique_ptr<Euclid::XYDataset::XYDataset> m_current_redshifted_sed;
  
  // vector with the SED datasets the generator uses
  const std::vector<Euclid::XYDataset::XYDataset>& m_sed_data_vector;
  
  // vector with the reddening curves the generator uses
  const std::vector<ExtinctionFunctor>& m_reddening_curve_functor_vector;
  
}; // End of ModelDatasetGenerator class

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_MODELDATASETGENERATOR_H */

