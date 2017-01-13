/** 
 * @file LxAuxConfiguration.cpp
 * @date May 28, 2015
 * @author Nikolaos Apostolakos
 */

#include <memory>
#include "XYDataset/FileParser.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzCLI/LsAuxConfiguration.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"

namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description LsAuxConfiguration::getProgramOptions() {
  po::options_description options {};
    options.add_options()
    ("type", po::value<std::string>(),
        "The type of the contents to list (one of SEDs, Filters or ReddeningCurves")
    ("group", po::value<std::string>(),
        "List the contents of the given group")
    ("data", po::value<std::string>(),
        "Print the data of the given dataset");
    return merge(options)
                (PhosphorosPathConfiguration::getProgramOptions());
}

LsAuxConfiguration::LsAuxConfiguration(const std::map<std::string, po::variable_value>& options)
                : PhosphorosPathConfiguration(options) {
  m_options = options;
}

std::unique_ptr<XYDataset::XYDatasetProvider> LsAuxConfiguration::getDatasetProvider() {
  // Get the dataset provider to use. If the user didn't gave the data-root-path
  // use the current directory
  std::string path = getAuxDataDir().string();
  if (!m_options["type"].empty() && m_options["type"].as<std::string>().size() != 0) {
    path += "/" + m_options["type"].as<std::string>();
  }
  std::unique_ptr<XYDataset::FileParser> file_parser {new XYDataset::AsciiParser{}};
  return std::unique_ptr<XYDataset::XYDatasetProvider> {
    new XYDataset::FileSystemProvider{path, std::move(file_parser)}
  };
}

std::string LsAuxConfiguration::getDatasetToShow() {
  return m_options.count("data") > 0 ? m_options.at("data").as<std::string>() : "";
}

std::string LsAuxConfiguration::getGroup() {
  return m_options.count("group") > 0 ? m_options.at("group").as<std::string>() : "";
}

bool LsAuxConfiguration::showData() {
  return m_options.count("data") > 0;
}

}
}