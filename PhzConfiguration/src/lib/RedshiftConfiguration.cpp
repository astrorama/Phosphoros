/*
 * RedshiftConfiguration.cpp
 *
 *  Created on: Oct 20, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <vector>
#include <algorithm>

#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/RedshiftConfiguration.h"
#include "CheckString.h"

using boost::regex;
using boost::regex_match;

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description RedshiftConfiguration::getProgramOptions() {
  po::options_description options {"Photometric redshift option"};
  options.add_options()
   ("z-range", po::value<std::vector<std::string>>(),
        "Redshift range: minimum maximum step")
    ("z-value", po::value<std::vector<std::string>>(),
        "A single z value");
  return options;
}


std::vector<double> RedshiftConfiguration::getZList() {

  // A set is used to avoid duplicates and to order the different entries
  std::set<double> selected {};
  if (!m_options["z-range"].empty()) {
    auto ranges_list = m_options["z-range"].as<std::vector<std::string>>();
    for (auto& range_string : ranges_list) {
      checkRangeString({"z-range"}, range_string);
      std::stringstream range_stream {range_string};
      double min {};
      double max {};
      double step {};
      std::string dummy{};
      range_stream >> min >> max >> step >> dummy;
      if (!dummy.empty()) {
        throw Elements::Exception() <<"Invalid character(s) for the z-range "
                                    << "option from here : " << dummy;
      }
      // Check the range is allowed before inserting
      if ( (max < min) || (!selected.empty() && (*--selected.end() > min))) {
        throw Elements::Exception()<< "Invalid range(s) for z-range option : \""
                                  <<range_stream.str()<<"\"";
      }
      // Insert value in the set
      for (double value=min; value<=max; value+=step) {
        selected.insert(value);
      }
    }
  }
  // Add the z-value option
  if (!m_options["z-value"].empty()) {
    auto values_list = m_options["z-value"].as<std::vector<std::string>>();
    for (auto& values_string : values_list) {
      checkValueString({"z-value"}, values_string);
      std::stringstream values_stream {values_string};
      while (values_stream.good()) {
        double value {};
        std::string dummy{};
        values_stream >> value >> dummy;
        if (!dummy.empty()) {
          throw Elements::Exception() <<"Invalid character(s) for the z-value "
                                      << "option from here : " << dummy;
        }
        selected.insert(value);
      }
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty Z list (check the options z-range and z-value)";
  }
  return std::vector<double> {selected.begin(), selected.end()};
}


}
}


