/*
 * CatalogColumnReader.cpp
 *
 *  Created on: Feb 25, 2015
 *      Author: fdubath
 */

#include <fstream>
#include <array>
#include "PhzUITools/CatalogColumnReader.h"
#include <CCfits/CCfits>
#include "Table/AsciiReader.h"
#include "Table/FitsReader.h"
#include "Table/Table.h"

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

    std::shared_ptr<Table::ColumnInfo> column_info_ptr;

    if (is_fit_file){
      CCfits::FITS fits {m_file_name};
      column_info_ptr = Table::FitsReader().read(fits.extension(1)).getColumnInfo();
    } else{
      std::ifstream in {m_file_name};
      column_info_ptr =Table::AsciiReader().read(in).getColumnInfo();

    }

    for(std::size_t i=0; i< column_info_ptr->size();++i){
      column_names.insert(column_info_ptr->getDescription(i).name);
    }
  } catch(...) {} // if the program is not able to read the file return an empty list...

  return column_names;

}

}
}
