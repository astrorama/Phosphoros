/** 
 * @file ComputeModelSedConfiguration.cpp
 * @date May 28, 2015
 * @author Nikolaos Apostolakos
 */

#include "XYDataset/QualifiedName.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzConfiguration/ProgramOptionsHelper.h"
#include "PhzConfiguration/PhosphorosPathConfiguration.h"
#include "PhzConfiguration/ParameterSpaceConfiguration.h"
#include "PhzCLI/ComputeModelSedConfiguration.h"

#include <iostream>


namespace po = boost::program_options;

namespace Euclid {
namespace PhzConfiguration {

po::options_description ComputeModelSedConfiguration::getProgramOptions() {
  po::options_description options {"Compute Model SED options"};

  options.add_options()
    ("sed-name", po::value<std::string>(),
        "The SED name")
    ("reddening-curve-name", po::value<std::string>(),
        "The reddening curve name")
    ("ebv-value", po::value<double>(),
        "The E(B-V) value")
    ("z-value", po::value<double>(),
        "The redshift value");

  return merge(options)
         (PhosphorosPathConfiguration::getProgramOptions())
         (IgmConfiguration::getProgramOptions());
}

ComputeModelSedConfiguration::ComputeModelSedConfiguration(const std::map<std::string, po::variable_value>& options)
              : IgmConfiguration(options) {
  m_options = options;
  if (m_options.count("sed-name") == 0) {
    throw Elements::Exception() << "Missing option sed-name";
  }
  if (m_options.count("reddening-curve-name") == 0) {
    throw Elements::Exception() << "Missing option reddening-curve-name";
  }
  if (m_options.count("ebv-value") == 0) {
    throw Elements::Exception() << "Missing option ebv-value";
  }
  if (m_options.count("z-value") == 0) {
    throw Elements::Exception() << "Missing option z-value";
  }
}


PhzDataModel::ModelAxesTuple ComputeModelSedConfiguration::getGridAxes() {
  return PhzDataModel::createAxesTuple({m_options.at("z-value").as<double>()},
                                       {m_options.at("ebv-value").as<double>()},
                                       {{m_options.at("reddening-curve-name").as<std::string>()}},
                                       {{m_options.at("sed-name").as<std::string>()}});
}


std::map<XYDataset::QualifiedName, XYDataset::XYDataset> ComputeModelSedConfiguration::getSedMap() {
  // ParameterSpaceConfiguration expects vectors of strings instead of strings
  auto ps_options = m_options;
  ps_options["sed-name"].value() = boost::any{std::vector<std::string>{ps_options["sed-name"].as<std::string>()}};
  ps_options["reddening-curve-name"].value() = boost::any{std::vector<std::string>{ps_options["reddening-curve-name"].as<std::string>()}};
  ps_options["ebv-value"].value() = boost::any{std::vector<double>{ps_options["ebv-value"].as<double>()}};
  ps_options["z-value"].value() = boost::any{std::vector<double>{ps_options["z-value"].as<double>()}};
  ParameterSpaceConfiguration ps_conf {ps_options};
  
  auto sed_name = ps_conf.getSedList()[0];
  auto provider = ps_conf.getSedDatasetProvider();
  auto dataset = provider->getDataset(sed_name);
  
  std::map<XYDataset::QualifiedName, XYDataset::XYDataset> result {};
  result.insert(std::make_pair(sed_name, std::move(*dataset)));
  return result;
}


std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>> ComputeModelSedConfiguration::getReddeningCurveMap() {
  // ParameterSpaceConfiguration expects vectors of strings instead of strings
  auto ps_options = m_options;
  ps_options["sed-name"].value() = boost::any{std::vector<std::string>{ps_options["sed-name"].as<std::string>()}};
  ps_options["reddening-curve-name"].value() = boost::any{std::vector<std::string>{ps_options["reddening-curve-name"].as<std::string>()}};
  ps_options["ebv-value"].value() = boost::any{std::vector<double>{ps_options["ebv-value"].as<double>()}};
  ps_options["z-value"].value() = boost::any{std::vector<double>{ps_options["z-value"].as<double>()}};
  ParameterSpaceConfiguration ps_conf {ps_options};
  
  auto red_curve_name = ps_conf.getReddeningCurveList()[0];
  auto provider = ps_conf.getReddeningDatasetProvider();
  auto dataset = provider->getDataset(red_curve_name);
  auto function = MathUtils::interpolate(*dataset, MathUtils::InterpolationType::LINEAR);
  
  std::map<XYDataset::QualifiedName, std::unique_ptr<MathUtils::Function>> result {};
  result.emplace(red_curve_name, std::move(function));
  return result;
}


}
}