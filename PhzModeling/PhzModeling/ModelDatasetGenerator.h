/*
 * PhzModeling/ModelDatasetGenerator.h
 *
 *  Created on: Sep 29, 2014
 *      Author: fdubath
 */

#ifndef PHZMODELING_MODELDATASETGENERATOR_H_
#define PHZMODELING_MODELDATASETGENERATOR_H_


#include <map>
#include "PhzDataModel/PhzModel.h"
#include "MathUtils/function/Function.h"

namespace Euclid {
namespace XYDataset{
  class XYDataset;
  class QualifiedName;
}

namespace PhzModeling {

/**
 * @class ModelDatasetGenerator
 * @brief Provides the SED Model for the current index of the Model
 * parameter space iterator
 */
class ModelDatasetGenerator {

public:
  
  typedef std::function<XYDataset::XYDataset(const XYDataset::XYDataset&,
                            const MathUtils::Function&, double )> ReddeningFunction;
  
  typedef std::function<XYDataset::XYDataset(const XYDataset::XYDataset& ,
                            double)> RedshiftFunction;
  
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
  *
  * @param reddening_curve_map
  * A map storing the extinction curve as a Function and giving access to them
  * through the QualifiedName.
  *
  * @param current_index
  * The current index.
  *
  * @param reddening_function
  * A function used to apply the extinction to a SED
  *
  * @param redshift_function
  * A function used to apply the redshit to a SED
  *
  */
  ModelDatasetGenerator(const PhzDataModel::ModelAxesTuple& parameter_space,
                        const std::map<XYDataset::QualifiedName,
                                       XYDataset::XYDataset>& sed_map,
                        const std::map<XYDataset::QualifiedName,
                          std::unique_ptr<MathUtils::Function> >& reddening_curve_map,
                        size_t current_index,
                        const ReddeningFunction& reddening_function,
                        const RedshiftFunction& redshift_function);

  /**
    * @brief Copy constructor.
    */
  ModelDatasetGenerator(const ModelDatasetGenerator& other);

  /**
    * @brief Assignment operator. Set the current index to the one of
    * the other instance.
    *
    * @param other
    * A ModelDatasetGenerator: the index has to be copied from.
    *
    */
  ModelDatasetGenerator& operator=(const ModelDatasetGenerator& other);

  /**
    * @brief Assignment operator. Set the current index to the provided one
    * and reset the current reddened SED.
    *
    * @param other
    * A size_t: the new index.
    *
    */
  ModelDatasetGenerator& operator=(size_t other);

  /**
    * @brief Increment operator. Increase the current index and reset
    * the current model.
    */
  ModelDatasetGenerator& operator++();

  /**
    * @brief Increment by addition operator. Increase the current index by
    * the provided value and reset the current reddened SED.
    *
    * @param n
    * An int: the index increment.
    */
  ModelDatasetGenerator& operator+=(int n) ;

  /**
    * @brief Subtraction operator. Decrease the current index by the provided
    * value and reset the current reddened SED.
    *
    * @param n
    * A size_t: the index decrement.
    */
  int operator-(size_t other) const;

  /**
    * @brief Subtraction operator. Decrease the current index by the index of
    * the provided ModelDatasetGenerator and reset the current reddened SED.
    *
    * @param other
    * A ModelDatasetGenerator from which index value the current index will
    * be decremented.
    */
  int operator-(const ModelDatasetGenerator& other) const;

  /**
    * @brief Equality operator. True if the provided value match the
    * current index.
    *
    * @param other
    * A size_t the current index will be compared to.
    */
  bool operator==(size_t other) const;

  /**
    * @brief Equality operator. Test the equality of the current indexes.
    *
    * @param other
    * A ModelDatasetGenerator from which the the current index will be
    * compared to.
    */
  bool operator==(const ModelDatasetGenerator& other) const;

  /**
    * @brief Inequality operator. False if the provided value match the
    * current index.
    *
    * @param other
    * An size_t the current index will be compared to.
    */
  bool operator!=(size_t other) const;

  /**
    * @brief Inequality operator. Test the inequality of the current indexes.
    *
    * @param other
    * A ModelDatasetGenerator from which the the current index will be
    * compared to.
    */
  bool operator!=(const ModelDatasetGenerator& other) const;

  /**
    * @brief Greater than operator. True if the current index is bigger than
    * the provided value.
    *
    * @param other
    * A size_t the current index will be compared to.
    */
  bool operator>(size_t other) const;

  /**
    * @brief Greater than operator. True if the current index is bigger than
    * the current index of the provided Generator.
    *
    * @param other
    * A ModelDatasetGenerator the current index will be compared to.
    */
  bool operator>(const ModelDatasetGenerator& other) const;

  /**
    * @brief Lower than operator. True if the current index is smaller than
    * the provided value.
    *
    * @param other
    * A size_t the current index will be compared to.
    */
  bool operator<(size_t other) const;

  /**
    * @brief Lower than operator. True if the current index is smaller than
    * the current index of the provided Generator.
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
  XYDataset::XYDataset& operator*();

private:

  // An object to convert the parameter space coordinates to a long index and vice versa
  decltype(GridContainer::makeGridIndexHelper(std::declval<PhzDataModel::ModelAxesTuple>())) m_index_helper;
  const PhzDataModel::ModelAxesTuple& m_parameter_space;

  // The current long 1D index
  size_t m_current_index;
  size_t m_size;

  // The indices of the parameters last calculated
  size_t m_current_sed_index {0};
  size_t m_current_reddening_curve_index {0};
  size_t m_current_ebv_index {0};
  size_t m_current_z_index {0};

  // The latest calculated reddened and redshifted SEDs
  std::unique_ptr<XYDataset::XYDataset> m_current_reddened_sed;
  std::unique_ptr<XYDataset::XYDataset> m_current_redshifted_sed;

  // map with the SED datasets the generator uses
  const std::map<XYDataset::QualifiedName,XYDataset::XYDataset>& m_sed_map;

  // vector with the reddening curves the generator uses
  const std::map<XYDataset::QualifiedName,
    std::unique_ptr<MathUtils::Function> >& m_reddening_curve_map;

  ReddeningFunction m_reddening_function;
  RedshiftFunction m_redshift_function;


}; // End of ModelDatasetGenerator class

} // end of namespace PhzModeling
} // end of namespace Euclid



#endif /* PHZMODELING_MODELDATASETGENERATOR_H_ */
