/**
 * @file LsGrid.cpp
 * @date January 26, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include "ElementsKernel/ProgramHeaders.h"
#include "PhzCLI/DisplayModelGridConfiguration.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosDisplayTemplate");

ostream& operator<<(ostream& stream, const XYDataset::QualifiedName& name) {
  stream << name.qualifiedName();
  return stream;
}

void printGeneric(const PhzDataModel::PhotometryGridInfo& grid_info) {
  auto sed_size = std::get<PhzDataModel::ModelParameter::SED>(grid_info.axes).size();
  auto red_curve_size = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(grid_info.axes).size();
  auto ebv_size = std::get<PhzDataModel::ModelParameter::EBV>(grid_info.axes).size();
  auto z_size = std::get<PhzDataModel::ModelParameter::Z>(grid_info.axes).size();
  cout << "\nParameter Space info\n";
  cout << "--------------------\n";
  cout << "SED axis size: " << sed_size << '\n';
  cout << "Reddening curve axis size: " << red_curve_size << '\n';
  cout << "E(B-V) axis size: " << ebv_size << '\n';
  cout << "Z axis size: " << z_size << '\n';
  cout << "Total grid size : " << sed_size*red_curve_size*ebv_size*z_size << "\n\n";

  cout << "\nPhotometry info\n";
  cout << "---------------\n";
  cout << "IGM absorption method: " << grid_info.igm_method << '\n';
  cout << "Photometry filters:\n";
  for (auto& f : grid_info.filter_names) {
    cout << "    " << f << '\n';
  }
  cout << '\n';
}

template <typename Axis>
void printAxis(const Axis& axis) {
  cout << "\nAxis " << axis.name() << " (" << axis.size() << ")\n";
  cout << "Index\tValue\n";
  int i {0};
  for (auto& value : axis) {
    cout << i++ << '\t' << value << '\n';
  }
  cout << '\n';
}

void printPhotometry(const PhzDataModel::PhotometryGrid& grid,
                     const tuple<size_t,size_t,size_t,size_t>& coords) {
  size_t c1 = get<0>(coords);
  size_t c2 = get<1>(coords);
  size_t c3 = get<2>(coords);
  size_t c4 = get<3>(coords);
  // Note that the photometry grid indices have the opposit order
  auto& phot = grid.at(c4,c3,c2,c1);
  cout << "\nCell (" << c1 << "," << c2 << "," << c3 << "," << c4 << ") axis information:\n";
  cout << "SED      " << grid.getAxis<PhzDataModel::ModelParameter::SED>()[c1] << '\n';
  cout << "REDCURVE " << grid.getAxis<PhzDataModel::ModelParameter::REDDENING_CURVE>()[c2] << '\n';
  cout << "EBV      " << grid.getAxis<PhzDataModel::ModelParameter::EBV>()[c3] << '\n';
  cout << "Z        " << grid.getAxis<PhzDataModel::ModelParameter::Z>()[c4] << '\n';
  cout << "\nCell (" << c1 << "," << c2 << "," << c3 << "," << c4 << ") Photometry:\n";
  for (auto iter=phot.begin(); iter!=phot.end(); ++iter) {
    cout << iter.filterName() << "\t" << (*iter).flux << '\n';
  }
  cout << '\n';
}

class DisplayModelGrid : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::DisplayModelGridConfiguration::getProgramOptions();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    PhzConfiguration::DisplayModelGridConfiguration conf {args};
    auto grid_info = conf.getPhotometryGridInfo();

    if (conf.showGeneric()) {
      printGeneric(grid_info);
    }

    if (conf.showSedAxis()) {
      printAxis(std::get<PhzDataModel::ModelParameter::SED>(grid_info.axes));
    }

    if (conf.showReddeningCurveAxis()) {
      printAxis(std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(grid_info.axes));
    }

    if (conf.showEbvAxis()) {
      printAxis(std::get<PhzDataModel::ModelParameter::EBV>(grid_info.axes));
    }

    if (conf.showRedshiftAxis()) {
      printAxis(std::get<PhzDataModel::ModelParameter::Z>(grid_info.axes));
    }

    auto phot_coords = conf.getCellPhotCoords();
    if (phot_coords) {
      auto grid = conf.getPhotometryGrid();
      printPhotometry(grid, *phot_coords);
    }

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(DisplayModelGrid)
