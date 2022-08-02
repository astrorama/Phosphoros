/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file PHZ_PdfHandling/PdfHandlingConfiguration.h
 * @date 01/12/18
 * @author fdubath
 */

#ifndef _PHZ_PDFHANDLING_PDFHANDLINGCONFIGURATION_H
#define _PHZ_PDFHANDLING_PDFHANDLINGCONFIGURATION_H

#include "Configuration/Configuration.h"
#include <boost/filesystem/operations.hpp>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

namespace Euclid {
namespace PHZ_PdfHandling {

/**
 * @class PdfHandlingConfiguration
 * @brief
 *
 */
class PdfHandlingConfiguration : public Configuration::Configuration {

public:
  /**
   * @brief Destructor
   */
  virtual ~PdfHandlingConfiguration() = default;

  /**
   * @brief constructor
   */
  PdfHandlingConfiguration(long manager_id);

  std::map<std::string, OptionDescriptionList> getProgramOptions() override;

  void initialize(const UserValues& args) override;

  std::string              getInputCatalogName() const;
  std::string              getIdColumnName() const;
  std::string              getPdfColName() const;
  std::string              getOutputCatalogName() const;
  std::string              getOutputColumnsPrefix() const;
  std::vector<std::string> getExcludedOutputColumns() const;
  uint                     getChunkSize() const;
  double                   getMergeRatio() const;

  bool getBiggestAreaMode() const;

private:
  std::string              m_input_cat_name    = "";
  std::string              m_id_col            = "ID";
  std::string              m_pdf_col           = "1D-PDF-Z";
  std::string              m_output_cat_name   = "";
  std::string              m_output_col_prefix = "";
  std::vector<std::string> m_excluded_output_columns;
  double                   m_merge_ratio;
  uint                     m_chunk_size        = 500;
  bool                     m_biggest_area_mode = true;

}; /* End of PdfHandlingConfiguration class */

} /* namespace PHZ_PdfHandling */
}  // namespace Euclid

#endif
