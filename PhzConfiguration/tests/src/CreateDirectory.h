/*
 * CreateDirectory.h
 *
 *  Created on: Jan 22, 2015
 *      Author: Nicolas Morisset
 */

#ifndef PHZCONFIGURATION_TESTS_SRC_CREATEDIRECTORY_H_
#define PHZCONFIGURATION_TESTS_SRC_CREATEDIRECTORY_H_

#include <boost/filesystem.hpp>


// Create a directory on disk using boost
inline void makeDirectory(const std::string& directory_name) {
  boost::filesystem::path d {directory_name};
  boost::filesystem::create_directories(d);
}

#endif /* PHZCONFIGURATION_TESTS_SRC_CREATEDIRECTORY_H_ */
