/**
 * @file ComputeModelSed.cpp
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
#include "PhzCLI/ComputeModelSedConfiguration.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("ComputeModelSed");

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

class ComputeModelSed : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::ComputeModelSedConfiguration::getProgramOptions();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    
    PhzConfiguration::ComputeModelSedConfiguration conf {args};
    
    auto grid_axes = conf.getGridAxes();
    auto sed_map = conf.getSedMap();
    auto red_curve_map = conf.getReddeningCurveMap();
    auto igm_function = conf.getIgmAbsorptionFunction();

    PhzModeling::ModelDatasetGrid grid {grid_axes, move(sed_map), move(red_curve_map),
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

MAIN_FOR(ComputeModelSed)
