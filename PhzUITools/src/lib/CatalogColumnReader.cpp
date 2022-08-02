/*
 * CatalogColumnReader.cpp
 *
 *  Created on: Feb 25, 2015
 *      Author: fdubath
 */

#include "PhzUITools/CatalogColumnReader.h"
#include "AlexandriaKernel/memory_tools.h"
#include "Table/AsciiReader.h"
#include "Table/FitsReader.h"
#include "Table/TableReader.h"
#include <array>
#include <fstream>

namespace Euclid {
namespace PhzUITools {

CatalogColumnReader::CatalogColumnReader(std::string file_name) : m_file_name(file_name) {}

Table::ColumnInfo GetInfo(std::string file_name, bool is_fit_file) {
  if (is_fit_file) {
    return Table::FitsReader(file_name, 1).getInfo();
  } else {
    return Table::AsciiReader(file_name).getInfo();
  }
}

std::set<std::string> CatalogColumnReader::getColumnNames() {
  std::set<std::string> column_names;

  try {
    bool                 is_fit_file = false;
    std::ifstream        in{m_file_name};
    std::array<char, 80> first_header_array;
    in.read(first_header_array.data(), 80);
    in.close();
    std::string first_header_str{first_header_array.data()};
    if (first_header_str.compare(0, 9, "SIMPLE  =") == 0) {
      is_fit_file = true;
    }

    Table::ColumnInfo column_info = GetInfo(m_file_name, is_fit_file);

    for (std::size_t i = 0; i < column_info.size(); ++i) {
      column_names.insert(column_info.getDescription(i).name);
    }
  } catch (...) {
  }  // if the program is not able to read the file return an empty list...

  return column_names;
}

}  // namespace PhzUITools
}  // namespace Euclid
