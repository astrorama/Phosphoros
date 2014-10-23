/*
 * RedshiftConfiguration.cpp
 *
 *  Created on: Oct 20, 2014
 *      Author: Nicolas Morisset
 */

#include <string>
#include <vector>
#include <algorithm>
#include <boost/regex.hpp>

#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/RedshiftConfiguration.h"

using boost::regex;
using boost::regex_match;

namespace Euclid {
namespace PhzConfiguration {

// This function applies a provided regex (regex_string) to a string_to_parse()
// in order to detect any not  wanted character otherwise throws an exception
void check_string(const std::string& regex_string,
                  const std::string& option,
                  const std::string& string_to_parse)
{
  boost::regex expression(regex_string);
  if (!boost::regex_match(string_to_parse, expression)) {
    throw Elements::Exception() <<"Invalid character(s) found for the option: "
                                << option << " = " << string_to_parse;
  }
}


po::options_description RedshiftConfiguration::getProgramOptions() {
  po::options_description options {"Photometric redshift option"};
  options.add_options()
   ("z-range", po::value<std::vector<std::string>>(),
        "Redshift range: min, max, step")
    ("z-value", po::value<std::vector<std::string>>(),
        "A single z value");
  return options;
}


std::vector<double> RedshiftConfiguration::getZList() {

  // Regex for the z-range option
  const std::string z_range_regex{"(((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)){3}"};
  // regex for the z-value option
  const std::string z_value_regex{"((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)"};

  // A set is used to avoid duplicates and to order the different entries
  std::set<double> selected {};
  if (!m_options["z-range"].empty()) {
    auto ranges_list = m_options["z-range"].as<std::vector<std::string>>();
    for (auto& range_string : ranges_list) {
      check_string(z_range_regex, {"z-range"}, range_string);
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
      if (!selected.empty() && ( (max < min) || (*--selected.end() > min)) ) {
       throw Elements::Exception{"Invalid range(s) for z-range option"};
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
      check_string(z_value_regex, {"z-value"}, values_string);
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


