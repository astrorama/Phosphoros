/** 
 * @file ReddeningConfiguration.cpp
 * @date October 6, 2014
 * @author Nikolaos Apostolakos
 */

#include <string>
#include <vector>

#include "ElementsKernel/Exception.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzConfiguration/ReddeningConfiguration.h"
#include "CheckString.h"

using boost::regex;
using boost::regex_match;

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {


po::options_description ReddeningConfiguration::getProgramOptions() {
  po::options_description options {"Photometric reddening options"};
  options.add_options()
    ("reddening-curve-root-path", po::value<std::string>(),
        "The directory containing the reddening curves")
    ("reddening-curve-group", po::value<std::vector<std::string>>(),
        "Use all the reddening curves in the given group and subgroups")
    ("reddening-curve-exclude", po::value<std::vector<std::string>>(),
        "a single name of the reddening curve to be excluded")
    ("reddening-curve-name", po::value<std::vector<std::string>>(),
        "A single reddening curve name")
    ("ebv-range", po::value<std::vector<std::string>>(),
        "E(B_V) range: min, max, step")
    ("ebv-value", po::value<std::vector<std::string>>(),
        "A single E(B_V) value");
  return options;
}

std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> ReddeningConfiguration::getReddeningDatasetProvider() {
  if (!m_options["reddening-curve-root-path"].empty()) {
    std::string path = m_options["reddening-curve-root-path"].as<std::string>();
    std::unique_ptr<Euclid::XYDataset::FileParser> file_parser {new Euclid::XYDataset::AsciiParser{}};
    return std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> {
      new Euclid::XYDataset::FileSystemProvider{path, std::move(file_parser)}
    };
  }
  throw Elements::Exception {"Missing or unknown reddening dataset provider options : <reddening-curve-root-path>"};
}

std::vector<Euclid::XYDataset::QualifiedName> ReddeningConfiguration::getReddeningCurveList() {
  // We use a set to avoid duplicate entries
  std::set<Euclid::XYDataset::QualifiedName> selected {};
  if (!m_options["reddening-curve-group"].empty()) {
    auto provider = getReddeningDatasetProvider();
    auto group_list = m_options["reddening-curve-group"].as<std::vector<std::string>>();
    for (auto& group : group_list) {
      for (auto& name : provider->listContents(group)) {
        selected.insert(name);
      }
    }
  }
  // Add reddening-name if any
  if (!m_options["reddening-curve-name"].empty()) {
    auto name_list    = m_options["reddening-curve-name"].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.insert(Euclid::XYDataset::QualifiedName{name});
    }
  }
  // Remove reddening-exclude if any
  if (!m_options["reddening-curve-exclude"].empty()) {
    auto name_list = m_options["reddening-curve-exclude"].as<std::vector<std::string>>();
    for (auto& name : name_list) {
      selected.erase(Euclid::XYDataset::QualifiedName{name});
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty reddening curve list";
  }
  return std::vector<Euclid::XYDataset::QualifiedName> {selected.begin(), selected.end()};
}

std::vector<double> ReddeningConfiguration::getEbvList() {

  // Regex for the ebv-range option
  const std::string ebv_range_regex{"(((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)){3}"};
  // regex for the ebv-value option
  const std::string ebv_value_regex{"((\\d+(\\.\\d*)?)|(\\.\\d+))($|\\s+)"};

  // A set is used to avoid duplicates and to order the different entries
  std::set<double> selected {};
  if (!m_options["ebv-range"].empty()) {
    auto ranges_list = m_options["ebv-range"].as<std::vector<std::string>>();
    for (auto& range_string : ranges_list) {
      checkRangeString({"ebv-range"}, range_string);
      std::stringstream range_stream {range_string};
      double min {};
      double max {};
      double step {};
      std::string dummy{};
      range_stream >> min >> max >> step >> dummy;
      if (!dummy.empty()) {
        throw Elements::Exception() <<"Invalid character(s) for the ebv-range "
                                    << "option from here : " << dummy;
      }
      // Check the range is allowed before inserting
      if (!selected.empty() && ( (max < min) || (*--selected.end() > min)) ) {
       throw Elements::Exception{"Invalid range(s) for ebv-range option"};
      }
      // Insert value in the set
      for (double value=min; value<=max; value+=step) {
        selected.insert(value);
      }
    }
  }
  // Add the ebv-value option
  if (!m_options["ebv-value"].empty()) {
    auto values_list = m_options["ebv-value"].as<std::vector<std::string>>();
    for (auto& values_string : values_list) {
      checkValueString({"ebv-value"}, values_string);
      std::stringstream values_stream {values_string};
      while (values_stream.good()) {
        double value {};
        std::string dummy{};
        values_stream >> value >> dummy;
        if (!dummy.empty()) {
          throw Elements::Exception() <<"Invalid character(s) for the ebv-value "
                                      << "option from here : " << dummy;
        }
        selected.insert(value);
      }
    }
  }
  if (selected.empty()) {
    throw Elements::Exception() << "Empty ebv list (check the options ebv-range and ebv-value)";
  }
  return std::vector<double> {selected.begin(), selected.end()};
}

}
}
