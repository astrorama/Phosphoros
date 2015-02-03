/** 
 * @file PrintModelDataset.cpp
 * @date January 27, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include "ElementsKernel/ProgramHeaders.h"
#include "MathUtils/interpolation/interpolation.h"
#include "PhzConfiguration/ParameterSpaceConfiguration.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/RedshiftFunctor.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PrintModelDataset");

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

class PrintModelDataset : public Elements::Program {
  
  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::ParameterSpaceConfiguration::getProgramOptions();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    
    PhzConfiguration::ParameterSpaceConfiguration conf {args};
    auto sed_prov = conf.getSedDatasetProvider();
    auto red_curve_prov = conf.getReddeningDatasetProvider();
    auto sed_list = conf.getSedList();
    auto red_curve_list = conf.getReddeningCurveList();
    auto ebv_list = conf.getEbvList();
    auto z_list = conf.getZList();
    
    auto param_space = PhzDataModel::createAxesTuple(z_list, ebv_list, red_curve_list, sed_list);
    auto sed_map = buildMap(*sed_prov, sed_list.begin(), sed_list.end());
    auto red_curve_map = convertToFunction(buildMap(*red_curve_prov,
                                            red_curve_list.begin(), red_curve_list.end()));
    
    PhzModeling::ModelDatasetGrid grid {param_space, move(sed_map), move(red_curve_map),
                               PhzModeling::ExtinctionFunctor{}, PhzModeling::RedshiftFunctor{}};
                               
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

MAIN_FOR(PrintModelDataset)