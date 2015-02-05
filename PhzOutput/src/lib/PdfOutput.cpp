/*
 * PdfOutput.cpp
 *
 *  Created on: Feb 4, 2015
 *      Author: Nicolas Morisset
 */

#include <fstream>
#include <string>
#include <vector>
#include "Table/Table.h"
#include "Table/FitsWriter.h"
#include "PhzOutput/PdfOutput.h"


namespace Euclid {
namespace PhzOutput {


void PdfOutput::handleSourceOutput(const SourceCatalog::Source& source,
                                         PhzDataModel::PhotometryGrid::const_iterator best_model,
                                   const PhzDataModel::Pdf1D& pdf) {

  // Create the columnInfo object
  std::shared_ptr<Euclid::Table::ColumnInfo> column_info {new Euclid::Table::ColumnInfo {{
	  Euclid::Table::ColumnInfo::info_type("Z", typeid(double)),
	  Euclid::Table::ColumnInfo::info_type("Probability", typeid(double))
  }}};

  // Transfer pdf data to rows
  std::vector<Table::Row> row_list {};
  for (auto iter=pdf.begin(); iter!=pdf.end(); ++iter) {
	  row_list.push_back(Table::Row{{iter.axisValue<0>(), *iter}, column_info});
  }

  Table::Table pdf_table{row_list};

  // Store pdf data into fits dile
//  CCfits::FITS fits_file {m_out_file.string(), CCfits::RWmode::Write};
  Table::FitsWriter fits_writer {Table::FitsWriter::Format::BINARY};

	if (source.getId()%1000 == 0) {
		m_fits_file->destroy();
		m_fits_file.reset( new CCfits::FITS {m_out_file.string(), CCfits::RWmode::Write});
	}
  fits_writer.write(*m_fits_file, std::to_string(source.getId()), pdf_table);

}

} // end of namespace PhzOutput
} // end of namespace Euclid


