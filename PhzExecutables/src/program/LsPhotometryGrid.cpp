/** 
 * @file LsGrid.cpp
 * @date January 26, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include "ElementsKernel/ProgramHeaders.h"
#include "PhzConfiguration/LsPhotometryGridConfiguration.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("LsPhotometryGrid");

void printGeneric(const PhzDataModel::PhotometryGrid& grid) {
  cout << "\nSED axis size: " << grid.getAxis<PhzDataModel::ModelParameter::SED>().size() << '\n';
  cout << "Reddening curve axis size: " << grid.getAxis<PhzDataModel::ModelParameter::REDDENING_CURVE>().size() << '\n';
  cout << "E(B-V) axis size: " << grid.getAxis<PhzDataModel::ModelParameter::EBV>().size() << '\n';
  cout << "Z axis size: " << grid.getAxis<PhzDataModel::ModelParameter::Z>().size() << '\n';
  cout << "Total grid size : " << grid.size() << "\n\n";
}

ostream& operator<<(ostream& stream, const XYDataset::QualifiedName& name) {
  stream << name.qualifiedName();
  return stream;
}

template <int I>
void printAxis(const PhzDataModel::PhotometryGrid& grid) {
  auto& axis = grid.getAxis<I>();
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

class LsPhotometryGrid : public Elements::Program {
  
  po::options_description defineSpecificProgramOptions() override {
    return PhzConfiguration::LsPhotometryGridConfiguration::getProgramOptions();
  }
  
  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {
    
    PhzConfiguration::LsPhotometryGridConfiguration conf {args};
    auto grid = conf.getPhotometryGrid();
    
    if (conf.showGeneric()) {
      printGeneric(grid);
    }
    
    if (conf.showSedAxis()) {
      printAxis<PhzDataModel::ModelParameter::SED>(grid);
    }
    
    if (conf.showReddeningCurveAxis()) {
      printAxis<PhzDataModel::ModelParameter::REDDENING_CURVE>(grid);
    }
    
    if (conf.showEbvAxis()) {
      printAxis<PhzDataModel::ModelParameter::EBV>(grid);
    }
    
    if (conf.showRedshiftAxis()) {
      printAxis<PhzDataModel::ModelParameter::Z>(grid);
    }
    
    auto phot_coords = conf.getCellPhotCoords();
    if (phot_coords) {
      printPhotometry(grid, *phot_coords);
    }
    
    return Elements::ExitCode::OK;
  }
  
};

MAIN_FOR(LsPhotometryGrid)