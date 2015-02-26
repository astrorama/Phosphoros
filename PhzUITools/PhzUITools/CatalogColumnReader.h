/*
 * CatalogColumnReader.h
 *
 *  Created on: Feb 25, 2015
 *      Author: fdubath
 */

#ifndef CATALOGCOLUMNREADER_H_
#define CATALOGCOLUMNREADER_H_

#include<list>
#include<string>

namespace Euclid {
namespace PhzUITools {

class CatalogColumnReader{

public:
  CatalogColumnReader(std::string file_name);

  std::list<std::string> getColumnNames();

private:

  std::string m_file_name;

};

}
}

#endif /* CATALOGCOLUMNREADER_H_ */
