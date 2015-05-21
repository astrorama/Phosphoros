/**
 * @file ComputeModelSet.cpp
 * @date January 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include "ElementsKernel/ProgramHeaders.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzConfiguration/IgmConfiguration.h"
#include "PhzConfiguration/ParameterSpaceConfiguration.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "PhzModeling/PhotometryGridCreator.h"
#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "PhzModeling/MadauIgmFunctor.h"
#include "PhzModeling/NoIgmFunctor.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("ComputeModelSet");

template<typename NameIter>
std::map<XYDataset::QualifiedName, XYDataset::XYDataset> buildMap(
          XYDataset::XYDatasetProvider& provider, NameIter begin, NameIter end) {
  std::map<XYDataset::QualifiedName, XYDataset::XYDataset> result {};
  while (begin != end) {
    auto dataset_ptr = provider.getDataset(*begin);
    if (!dataset_ptr) {
      throw Elements::Exception() << "Failed to find dataset: " << begin->qualifiedName();
    }
    result.insert(std::make_pair(*begin, std::move(*dataset_ptr)));
    ++begin;
  }
  return result;
}

std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>>
    convertToFunction(const std::map<XYDataset::QualifiedName, XYDataset::XYDataset>& dataset_map) {
  std::map<XYDataset::QualifiedName, std::unique_ptr<Euclid::MathUtils::Function>> result {};
  for (auto& pair : dataset_map) {
    auto function_ptr = MathUtils::interpolate(pair.second, MathUtils::InterpolationType::LINEAR);
    result.emplace(pair.first, std::move(function_ptr));
  }
  return result;
}

class ComputeModelSet : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    po::options_description options {"Display Dataset options"};

    options.add_options()
      ("sed-root-path", po::value<std::string>(),
        "The directory containing the sed datasets, organized in folders")
      ("reddening-curve-root-path", po::value<std::string>(),
          "The directory containing the reddening curves")
      ("sed-name", po::value<std::vector<std::string>>(),
          "The SED name")
      ("reddening-curve-name", po::value<std::vector<std::string>>(),
          "The reddening curve name")
      ("ebv-value", po::value<std::vector<std::string>>(),
          "The E(B-V) value")
      ("z-value", po::value<std::vector<std::string>>(),
          "The redshift value");

    auto igm_options = PhzConfiguration::IgmConfiguration::getProgramOptions().options();
    for (auto o : igm_options) {
      options.add(o);
    }

    return options;
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    PhzConfiguration::ParameterSpaceConfiguration ps_conf {args};
    auto sed_prov = ps_conf.getSedDatasetProvider();
    auto red_curve_prov = ps_conf.getReddeningDatasetProvider();
    auto sed_list = ps_conf.getSedList();
    auto red_curve_list = ps_conf.getReddeningCurveList();
    auto ebv_list = ps_conf.getEbvList();
    auto z_list = ps_conf.getZList();

    auto param_space = PhzDataModel::createAxesTuple(z_list, ebv_list, red_curve_list, sed_list);
    auto sed_map = buildMap(*sed_prov, sed_list.begin(), sed_list.end());
    auto red_curve_map = convertToFunction(buildMap(*red_curve_prov,
                                            red_curve_list.begin(), red_curve_list.end()));

    PhzConfiguration::IgmConfiguration igm_conf {args};
    auto igm_function = igm_conf.getIgmAbsorptionFunction();

    PhzModeling::ModelDatasetGrid grid {param_space, move(sed_map), move(red_curve_map),
                               PhzModeling::ExtinctionFunctor{}, PhzModeling::RedshiftFunctor{},
                               std::move(igm_function)};

    for (auto iter=grid.begin(); iter!=grid.end(); ++iter) {
      cout << "\nDataset for model with:\n";
      cout << "SED      " << iter.axisValue<PhzDataModel::ModelParameter::SED>().qualifiedName() << '\n';
      cout << "REDCURVE " << iter.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>().qualifiedName() << '\n';
      cout << "EBV      " << iter.axisValue<PhzDataModel::ModelParameter::EBV>() << '\n';
      cout << "Z        " << iter.axisValue<PhzDataModel::ModelParameter::Z>() << '\n';
      cout << "Data:\n";
      for (auto& pair : *iter) {
        cout << pair.first << '\t' << pair.second << '\n';
      }
    }
    cout << '\n';

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ComputeModelSet)
