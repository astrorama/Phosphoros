/*
 * CatalogColumnReader.cpp
 *
 *  Created on: Feb 25, 2015
 *      Author: fdubath
 */

#include <fstream>
#include <array>
#include "AlexandriaKernel/memory_tools.h"
#include "PhzUITools/CatalogColumnReader.h"
#include "Table/TableReader.h"
#include "Table/AsciiReader.h"
#include "Table/FitsReader.h"

namespace Euclid {
namespace PhzUITools {

CatalogColumnReader::CatalogColumnReader(std::string file_name) : m_file_name(file_name) {
}

std::set<std::string> CatalogColumnReader::getColumnNames() {
  std::set<std::string> column_names;

  try{
    bool is_fit_file = false;
    std::ifstream in { m_file_name };
    std::array<char, 80> first_header_array;
    in.read(first_header_array.data(), 80);
    in.close();
    std::string first_header_str { first_header_array.data() };
    if (first_header_str.compare(0, 9, "SIMPLE  =") == 0) {
      is_fit_file=true;
    }
    
    std::unique_ptr<Table::TableReader> reader {};
    if (is_fit_file) {
      reader = make_unique<Table::FitsReader>(m_file_name, 1);
    } else {
      reader = make_unique<Table::AsciiReader>(m_file_name);
    }
    auto& info = reader->getInfo();

    for(std::size_t i=0; i< info.size();++i){
      column_names.insert(info.getDescription(i).name);
    }
  } catch(...) {} // if the program is not able to read the file return an empty list...

  return column_names;

}

}
}
