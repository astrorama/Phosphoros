/**
 * @file PhzDataModel/serialization/QualifiedName.h
 * @date May 20, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_SERIALIZATION_QUALIFIEDNAME_H
#define	PHZDATAMODEL_SERIALIZATION_QUALIFIEDNAME_H

#include <string>
#include <vector>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include "XYDataset/QualifiedName.h"

namespace boost {
namespace serialization {

/**
 * @brief Serialize/deserialize function.
 *
 * @details
 * Everything is done in the constructor. Nothing here.
 *
 */
template<typename Archive>
void serialize(Archive&, Euclid::XYDataset::QualifiedName&, const unsigned int) {
  // Nothing here. Everything is done in the constructor
}

/**
 * @brief Save the data to be used by the constructor.
 *
 * @details
 * Stores the Groups vector then the (unqualified)name.
 *
 */
template<typename Archive>
void save_construct_data(Archive& ar, const Euclid::XYDataset::QualifiedName* t, const unsigned int) {
  std::vector<std::string> groups = t->groups();
  ar << groups;
  std::string name = t->datasetName();
  ar << name;
}

/**
 * @brief Get constructor data and instantiate the object.
 *
 * @details
 * Reads the Groups vector then the (unqualified)name.
 *
 */
template<typename Archive>
void load_construct_data(Archive& ar, Euclid::XYDataset::QualifiedName* t, const unsigned int) {
  std::vector<std::string> groups;
  ar >> groups;
  std::string name;
  ar >> name;
  ::new(t) Euclid::XYDataset::QualifiedName(std::move(groups), std::move(name));
}

} // end of namespace serialization
} // end of namespace boost

#endif	/* PHZDATAMODEL_SERIALIZATION_QUALIFIEDNAME_H */

