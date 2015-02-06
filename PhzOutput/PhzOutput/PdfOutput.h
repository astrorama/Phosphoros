/*
 * PdfOutput.h
 *
 *  Created on: Feb 4, 2015
 *      Author: Nicolas Morisset
 */

#ifndef PHZOUTPUT_PHZOUTPUT_PDFOUTPUT_H_
#define PHZOUTPUT_PHZOUTPUT_PDFOUTPUT_H_


#include <boost/filesystem.hpp>
#include <CCfits/CCfits>
#include "Table/Row.h"
#include "PhzOutput/OutputHandler.h"

namespace fs = boost::filesystem;

namespace Euclid {
namespace PhzOutput {

class PdfOutput : public OutputHandler {

public:

	PdfOutput(fs::path out_file) :m_out_file{out_file},
	                              m_fits_file{new CCfits::FITS{"!"+out_file.string(), CCfits::RWmode::Write}},
	                              m_counter(0) {}

  virtual ~PdfOutput() = default;

  void handleSourceOutput(const SourceCatalog::Source& source,
                          PhzDataModel::PhotometryGrid::const_iterator best_model,
                          const PhzDataModel::Pdf1D& pdf);

private:

  fs::path                      m_out_file;  // Filename of the output fits file
  std::unique_ptr<CCfits::FITS> m_fits_file; // Unique pointer to the FITS file object
  int64_t                       m_counter;   // Counting the number of sources

};

} // end of namespace PhzOutput
} // end of namespace Euclid



#endif /* PHZOUTPUT_PHZOUTPUT_PDFOUTPUT_H_ */
