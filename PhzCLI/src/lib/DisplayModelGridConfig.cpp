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
 * @file src/lib/DisplayModelGridConfig.cpp
 * @date 11/16/15
 * @author nikoapos
 */

#include <boost/regex.hpp>
#include <sstream>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include "ElementsKernel/Exception.h"
#include "PhzCLI/DisplayModelGridConfig.h"
#include "PhzConfiguration/PhotometryGridConfig.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzCLI {

DisplayModelGridConfig::DisplayModelGridConfig(long manager_id) : Configuration(manager_id) {
  declareDependency<PhzConfiguration::PhotometryGridConfig>();
}

auto DisplayModelGridConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return {{"Display Model Grid options",
           {{"export-as-catalog", po::value<std::string>(), "Export the model grid as a FITS catalog"},
            {"all-regions-info", po::bool_switch()->default_value(false),
             "Show an overview of each parameter space region"},
            {"region", po::value<std::string>(), "Specify a region to show information for"},
            {"sed", po::bool_switch()->default_value(false), "Show the SED axis values"},
            {"redcurve", po::bool_switch()->default_value(false), "Show the reddening curve axis values"},
            {"ebv", po::bool_switch()->default_value(false), "Show the E(B-V) axis values"},
            {"z", po::bool_switch()->default_value(false), "Show the redshift axis values"},
            {"phot", po::value<std::string>(),
             "Show the photometry of the cell (SED,REDCURVE,EBV,Z) (zero based indices)"}}}};
}

void DisplayModelGridConfig::initialize(const UserValues& args) {
  m_show_all_regions_info = args.at("all-regions-info").as<bool>();
  m_show_sed_axis         = args.at("sed").as<bool>();
  m_show_red_curve_axis   = args.at("redcurve").as<bool>();
  m_show_ebv_axis         = args.at("ebv").as<bool>();
  m_show_redshift_axis    = args.at("z").as<bool>();

  if (args.count("export-as-catalog") > 0) {
    m_export_as_catalog = true;
    m_output_fits_name  = args.at("export-as-catalog").as<std::string>();
  }

  if (args.count("region") > 0) {
    m_region_name = args.at("region").as<std::string>();
  }

  if (args.count("phot") > 0) {
    std::string coords = args.at("phot").as<std::string>();
    smatch      match_res;
    regex       expr{"\\s*\\(?\\s*"    // Skip any opening parenthesis if exists and any spaces
               "(\\d+)"          // Get the SED index
               "\\s*[ ,]"        // Separate with comma or space. Ignore other spaces
               "(\\d+)"          // Get the REDCURVE index
               "\\s*[ ,]"        // Separate with comma or space. Ignore other spaces
               "(\\d+)"          // Get the EBV index
               "\\s*[ ,]"        // Separate with comma or space. Ignore other spaces
               "(\\d+)"          // Get the Z index
               "\\s*\\)?\\s*"};  // Skip any closing parentesis if exists and any spaces
    if (!regex_match(coords, match_res, expr)) {
      throw Elements::Exception() << "Malformed coordinates: " << coords;
    }
    std::stringstream coords_stream{};
    coords_stream << match_res.str(1) << " " << match_res.str(2) << " " << match_res.str(3) << " " << match_res.str(4)
                  << " ";
    size_t c1, c2, c3, c4;
    coords_stream >> c1 >> c2 >> c3 >> c4;
    m_requested_cell_coords.reset(new std::tuple<size_t, size_t, size_t, size_t>(c1, c2, c3, c4));
  }

  m_show_overall = !m_show_all_regions_info && args.count("region") == 0;

  m_show_generic = !m_show_sed_axis && !m_show_red_curve_axis && !m_show_ebv_axis && !m_show_redshift_axis &&
                   m_requested_cell_coords == nullptr;
}

const std::string& DisplayModelGridConfig::getRegionName() const {
  return m_region_name;
}

std::shared_ptr<std::tuple<size_t, size_t, size_t, size_t>> DisplayModelGridConfig::getRequestedCellCoords() const {
  return m_requested_cell_coords;
}

bool DisplayModelGridConfig::showAllRegionsInfo() const {
  return m_show_all_regions_info;
}

bool DisplayModelGridConfig::showEbvAxis() const {
  return m_show_ebv_axis;
}

bool DisplayModelGridConfig::showGeneric() const {
  return m_show_generic;
}

bool DisplayModelGridConfig::showOverall() const {
  return m_show_overall;
}

bool DisplayModelGridConfig::showReddeningCurveAxis() const {
  return m_show_red_curve_axis;
}

bool DisplayModelGridConfig::showRedshiftAxis() const {
  return m_show_redshift_axis;
}

bool DisplayModelGridConfig::showSedAxis() const {
  return m_show_sed_axis;
}

bool DisplayModelGridConfig::exportAsCatalog() const {
  return m_export_as_catalog;
}

const std::string& DisplayModelGridConfig::getOutputFitsName() const {
  return m_output_fits_name;
}

}  // namespace PhzCLI
}  // namespace Euclid
