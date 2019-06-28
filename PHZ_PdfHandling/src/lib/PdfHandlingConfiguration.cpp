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
 * @file src/lib/PdfHandlingConfiguration.cpp
 * @date 01/12/18
 * @author fdubath
 */


#include <cstdlib>
#include <set>
#include <sstream>
#include <vector>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include <boost/program_options.hpp>

#include "PHZ_PdfHandling/PdfHandlingConfiguration.h"
#include "Configuration/Configuration.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Euclid {
namespace PHZ_PdfHandling {

static const std::string INPUT_CATALOG {"input-cat"};
static const std::string ID_COLUMN_NAME {"id-column"};
static const std::string PDF_COLUMN_NAME {"pdf-column"};
static const std::string OUTPUT_CATALOG {"output-cat"};
static const std::string CHUNK_SIZE {"chunk-size"};
static const std::string MERGE_RATIO {"merge-ratio"};
static const std::string MODE_SORTING {"mode_sorting"};
static const std::string OUT_COLUMN_PREFIX {"output-columns-prefix"};
static const std::string EXCLUD_COLUMN {"excluded-output-columns"};


static Elements::Logging logger = Elements::Logging::getLogger("PdfHandlingConfig");

PdfHandlingConfiguration::PdfHandlingConfiguration(long manager_id): Configuration(manager_id) {
}

auto PdfHandlingConfiguration::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"PDF Handling options", {
    {INPUT_CATALOG.c_str(), po::value<std::string>(),
        "Name of the .FITS input catalog"},
    {ID_COLUMN_NAME.c_str(), po::value<std::string>()->default_value("ID"),
        "Name of the Id column in the input catalog"},
    {PDF_COLUMN_NAME.c_str(), po::value<std::string>()->default_value("Z-1D-PDF"),
        "Name of the PDF column in the input catalog"},
    {CHUNK_SIZE.c_str(), po::value<uint>()->default_value(500),
        "Number of sources to be read and processed together"},
    {MERGE_RATIO.c_str(), po::value<double>()->default_value(0.8),
        "Forced in range [0,1] give the ratio of the mode hight for which non-decreassing point are still part of the mode if 0 any non-decressing point stop the capture, if 1 the full pdf is captured"},
    {MODE_SORTING.c_str(), po::value<std::string>()->default_value("AREA"),
    "Methode for sorting the pdf modes: ['AREA', 'HEIGHT'] default: AREA"}

  }},{"Output options",{
      {OUTPUT_CATALOG.c_str(), po::value<std::string>(),
              "Name of the .FITS output catalog"},
      {OUT_COLUMN_PREFIX.c_str(), po::value<std::string>()->default_value(""),
              "Prefix for the output column, by default no prefix is added"},
      {EXCLUD_COLUMN.c_str(), po::value<std::string>()->default_value(""),
      "Comma separated names of the columns to be removed from the output catalog. Available columns are: SOURCE_ID, MEDIAN, MIN_70, MAX_70, MIN_90, MAX_90, MIN_95, MAX_95, MED_CENTER_MIN_70, MED_CENTER_MAX_70, MED_CENTER_MIN_90, MED_CENTER_MAX_90, MED_CENTER_MIN_95, MED_CENTER_MAX_95, PHZ_MODE_1_SAMP, PHZ_MODE_1_MEAN, PHZ_MODE_1_FIT, PHZ_MODE_1_AREA, PHZ_MODE_2_SAMP, PHZ_MODE_2_MEAN, PHZ_MODE_2_FIT, PHZ_MODE_2_AREA.  By default all the columns are displayed."}
  }}};
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(std::move(item));
  }
  return elems;
}


void PdfHandlingConfiguration::initialize(const UserValues& args) {
  m_input_cat_name = args.find(INPUT_CATALOG)->second.as<std::string>();

  m_id_col=args.find(ID_COLUMN_NAME)->second.as<std::string>();
  m_pdf_col=args.find(PDF_COLUMN_NAME)->second.as<std::string>();

  m_output_cat_name = args.find(OUTPUT_CATALOG)->second.as<std::string>();
  m_output_col_prefix = args.find(OUT_COLUMN_PREFIX)->second.as<std::string>();

  m_chunk_size=args.find(CHUNK_SIZE)->second.as<uint>();
  m_merge_ratio=args.find(MERGE_RATIO)->second.as<double>();

  if (m_merge_ratio<0){
    m_merge_ratio=0.;
  } else if (m_merge_ratio>=1){
      m_merge_ratio=1.;
  }

  if ( args.find(MODE_SORTING)->second.as<std::string>()=="HEIGHT"){
    m_biggest_area_mode=false;
  }

  std::string excluded_columns = args.find(EXCLUD_COLUMN)->second.as<std::string>();
  m_excluded_output_columns = split(excluded_columns,',');

}


std::string PdfHandlingConfiguration::getInputCatalogName() const{
  return m_input_cat_name;
}

std::string PdfHandlingConfiguration::getIdColumnName() const{
  return m_id_col;
}

std::string PdfHandlingConfiguration::getPdfColName() const{
  return m_pdf_col;
}

std::string PdfHandlingConfiguration::getOutputCatalogName() const{
  return m_output_cat_name;
}
  
std::string PdfHandlingConfiguration::getOutputColumnsPrefix() const {
  return m_output_col_prefix;
}

uint PdfHandlingConfiguration::getChunkSize() const{
  return m_chunk_size;
}

double PdfHandlingConfiguration::getMergeRatio() const{
  return m_merge_ratio;
}

bool PdfHandlingConfiguration::getBiggestAreaMode() const{
  return m_biggest_area_mode;
}



std::vector<std::string> PdfHandlingConfiguration::getExcludedOutputColumns() const {
  return m_excluded_output_columns;
}

} // PHZ_PdfHandling namespace
}


