/*
 * CatalogColumnReader.h
 *
 *  Created on: Feb 25, 2015
 *      Author: fdubath
 */

#ifndef CATALOGCOLUMNREADER_H_
#define CATALOGCOLUMNREADER_H_

#include <set>
#include <string>

namespace Euclid {
namespace PhzUITools {
/**
 *  @brief The CatalogColumnReader class
 */
class CatalogColumnReader {

public:
  /**
   * @brief Initialize a CatalogColumnReader with an absolute file name.
   *
   * @param file_name the absolute file path of the catalog.
   */
  CatalogColumnReader(std::string file_name);

  /**
   * @brief opens and reads the columns name in the file.
   *
   * @return the list of the column names.
   */
  std::set<std::string> getColumnNames();

private:
  std::string m_file_name;
};

}  // namespace PhzUITools
}  // namespace Euclid

#endif /* CATALOGCOLUMNREADER_H_ */
