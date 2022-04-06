/**
 * @file LsGrid.cpp
 * @date January 26, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include <set>
#include <chrono>
#include <sstream>
#include "ElementsKernel/ProgramHeaders.h"
#include "Configuration/ConfigManager.h"
#include "XYDataset/QualifiedName.h"
#include "Table/FitsWriter.h"

#include "PhzDataModel/PhotometryGridInfo.h"
#include "PhzConfiguration/PhotometryGridConfig.h"
#include "PhzCLI/DisplayModelGridConfig.h"
#include "Configuration/Utils.h"

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("PhosphorosDisplayModelGrid");

static long config_manager_id = Euclid::Configuration::getUniqueManagerId();

void printPhotometryInfo(const PhzDataModel::PhotometryGridInfo& grid_info) {
  cout << "Photometry info\n";
  cout << "---------------\n";
  cout << "IGM absorption method: " << grid_info.igm_method << '\n';
  cout << "Photometry filters:\n";
  for (auto& f : grid_info.filter_names) {
    cout << "    " << f << '\n';
  }
  cout << '\n';
}

void printOverall(const PhzDataModel::PhotometryGridInfo& grid_info) {
  printPhotometryInfo(grid_info);
  std::string region_names {};
  std::set<XYDataset::QualifiedName> overall_seds {};
  std::set<XYDataset::QualifiedName> overall_reddening_curves {};
  std::set<double> overall_ebv {};
  std::set<double> overall_z {};
  std::size_t total_models_no = 0;
  for (auto& pair : grid_info.region_axes_map) {
    region_names += " \"" + pair.first + '\"';
    auto& sed_axis = std::get<PhzDataModel::ModelParameter::SED>(pair.second);
    overall_seds.insert(sed_axis.begin(), sed_axis.end());
    auto& redcurve_axis = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(pair.second);
    overall_reddening_curves.insert(redcurve_axis.begin(), redcurve_axis.end());
    auto& ebv_axis = std::get<PhzDataModel::ModelParameter::EBV>(pair.second);
    overall_ebv.insert(ebv_axis.begin(), ebv_axis.end());
    auto& z_axis = std::get<PhzDataModel::ModelParameter::Z>(pair.second);
    overall_z.insert(z_axis.begin(), z_axis.end());
    total_models_no += sed_axis.size()*redcurve_axis.size()*ebv_axis.size()*z_axis.size();
  }
  cout << "Parameter Space info\n";
  cout << "--------------------\n";
  cout << "Number of regions: " << grid_info.region_axes_map.size() << '\n';
  cout << "Regions names: " << region_names << '\n';
  cout << "Total number of SED templates: " << overall_seds.size() << '\n';
  cout << "Total range of E(B-V): [" << *overall_ebv.begin() << ", " << *overall_ebv.rbegin() << "]\n";
  cout << "Total range of Redshift Z: [" << *overall_z.begin() << ", " << *overall_z.rbegin() << "]\n";
  cout << "Total number of models: " << total_models_no << '\n';
  cout << '\n';
}

void printAllRegionsInfo(const PhzDataModel::PhotometryGridInfo& grid_info) {
  printPhotometryInfo(grid_info);
  cout << "Parameter Space info (" << grid_info.region_axes_map.size() << " regions)\n";
  cout << "---------------------------------\n";
  for (auto& pair : grid_info.region_axes_map) {
    cout << "Region: " << pair.first << '\n';
    auto& sed_axis = std::get<PhzDataModel::ModelParameter::SED>(pair.second);
    cout << "    Number of SED templates: " <<sed_axis.size() << '\n';
    auto& redcurve_axis = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(pair.second);
    cout << "    Number of Reddeining Curves: " << redcurve_axis.size() << '\n';
    auto& ebv_axis = std::get<PhzDataModel::ModelParameter::EBV>(pair.second);
    cout << "    E(B-V) range: [" << ebv_axis[0] << ", " << ebv_axis[ebv_axis.size()-1] << "] ("
         << ebv_axis.size() << " values)\n";
    auto& z_axis = std::get<PhzDataModel::ModelParameter::Z>(pair.second);
    cout << "    Redshift Z range: [" << z_axis[0] << ", " << z_axis[z_axis.size()-1] << "] ("
         << z_axis.size() << " values)\n";
    cout << "    Number of models: " << (sed_axis.size()*redcurve_axis.size()*ebv_axis.size()*z_axis.size()) << '\n';
  }
  cout << '\n';
}

void printGeneric(const PhzDataModel::PhotometryGridInfo& grid_info, const std::string& region_name) {
  printPhotometryInfo(grid_info);
  auto sed_size = std::get<PhzDataModel::ModelParameter::SED>(grid_info.region_axes_map.at(region_name)).size();
  auto red_curve_size = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(grid_info.region_axes_map.at(region_name)).size();
  auto ebv_size = std::get<PhzDataModel::ModelParameter::EBV>(grid_info.region_axes_map.at(region_name)).size();
  auto z_size = std::get<PhzDataModel::ModelParameter::Z>(grid_info.region_axes_map.at(region_name)).size();
  cout << "\nParameter Space info\n";
  cout << "--------------------\n";
  cout << "SED axis size: " << sed_size << '\n';
  cout << "Reddening curve axis size: " << red_curve_size << '\n';
  cout << "E(B-V) axis size: " << ebv_size << '\n';
  cout << "Z axis size: " << z_size << '\n';
  cout << "Total grid size : " << sed_size*red_curve_size*ebv_size*z_size << "\n";
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

template <int Axis>
std::map<PhzDataModel::PhotometryGrid::axis_type<Axis>, int> axisOverallIndices(
                    const std::map<std::string, PhzDataModel::PhotometryGrid>& grid_map) {
  std::map<PhzDataModel::PhotometryGrid::axis_type<Axis>, int> result;
  int i = 0;
  for (auto& pair : grid_map) {
    auto& axis = pair.second.getAxis<Axis>();
    for (auto& key : axis) {
      if (result.count(key) == 0) {
        result[key] = i++;
      }
    }
  }
  
  return result;
}

template <typename T>
std::string axisIndicesToString(const std::map<T, int>& indices) {
  std::stringstream result {};
  result << '[';
  std::vector<T> ordered (indices.size(), {"temp"});
  for (auto& pair : indices) {
    ordered[pair.second] = pair.first;
  }
  for (auto& key : ordered) {
    result << key << ',';
  }
  result.seekp(-1, std::ios_base::end);
  result << ']';
  return result.str();
}

void exportAsCatalog(const PhzDataModel::PhotometryGridInfo& grid_info,
                     const std::map<std::string, PhzDataModel::PhotometryGrid>& grid_map,
                     const std::string& output_name) {
  
  // Create the Table writer
  Table::FitsWriter writer {output_name, true};
  
  // Get the overall indices for the non numerical axes
  auto sed_indices = axisOverallIndices<PhzDataModel::ModelParameter::SED>(grid_map);
  writer.addComment("SEDs : " + axisIndicesToString(sed_indices));
  auto redcurve_indices = axisOverallIndices<PhzDataModel::ModelParameter::REDDENING_CURVE>(grid_map);
  writer.addComment("RedCurves : " + axisIndicesToString(redcurve_indices));
  
  // Create the column info of the table
  std::vector<Table::ColumnInfo::info_type> info_list {
    {"ID", typeid(int)},
    {"Model_SED", typeid(int)},
    {"Model_RedCurve", typeid(int)},
    {"Model_EBV", typeid(float)},
    {"Model_Z", typeid(float)}
  };
  for (auto& filter : grid_info.filter_names) {
    info_list.emplace_back(filter.datasetName(), typeid(double));
  }
  auto column_info = make_shared<Table::ColumnInfo>(move(info_list));
  
  // Now populate the catalog
  int id = 0;
  vector<Table::Row> row_list {};
  for (auto& pair : grid_map) {
    for (auto it = pair.second.begin(); it != pair.second.end(); ++it) {
      ++id;
      int sed = sed_indices.at(it.axisValue<PhzDataModel::ModelParameter::SED>());
      int redcurve = redcurve_indices.at(it.axisValue<PhzDataModel::ModelParameter::REDDENING_CURVE>());
      float ebv = it.axisValue<PhzDataModel::ModelParameter::EBV>();
      float z = it.axisValue<PhzDataModel::ModelParameter::Z>();
      std::vector<Table::Row::cell_type> cells {id, sed, redcurve, ebv, z};
      for (auto& filter : grid_info.filter_names) {
        cells.emplace_back(it->find(filter.qualifiedName())->flux);
      }
      row_list.push_back(Table::Row{cells, column_info});
      if (row_list.size() > 500) {
        writer.addData(Table::Table{row_list});
        row_list.clear();
      }
    }
  }
  
  writer.addData(Table::Table{row_list});

  logger.info() << "Exported model grid in file " << output_name;
}

class DisplayModelGrid : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<PhzCLI::DisplayModelGridConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(map<string, po::variable_value>& args) override {

    auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);
    
    auto& conf = config_manager.getConfiguration<PhzCLI::DisplayModelGridConfig>();
    auto& grid_info = config_manager.getConfiguration<PhzConfiguration::PhotometryGridConfig>().getPhotometryGridInfo();
    
    cout << '\n';
    if (conf.exportAsCatalog()) {
      auto& grid_map = config_manager.getConfiguration<PhzConfiguration::PhotometryGridConfig>().getPhotometryGrid();
      auto& filename = config_manager.getConfiguration<PhzCLI::DisplayModelGridConfig>().getOutputFitsName();
      exportAsCatalog(grid_info, grid_map, filename);
    } else if (conf.showOverall()) {
      printOverall(grid_info);
    } else if (conf.showAllRegionsInfo()) {
      printAllRegionsInfo(grid_info);
    } else {
      auto region_name = conf.getRegionName();
      if (grid_info.region_axes_map.count(region_name) == 0) {
        cout << "Unknown region name: " << region_name << '\n';
        throw Elements::Exception() << "Unknown region name: " << region_name;
      }
      cout << "Info for parameter space region \"" << region_name << "\"\n";
      cout << "----------------------------------------\n\n";
      if (conf.showGeneric()) {
        printGeneric(grid_info, region_name);
      }
      if (conf.showSedAxis()) {
        printAxis(std::get<PhzDataModel::ModelParameter::SED>(grid_info.region_axes_map.at(region_name)));
      }

      if (conf.showReddeningCurveAxis()) {
        printAxis(std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(grid_info.region_axes_map.at(region_name)));
      }

      if (conf.showEbvAxis()) {
        printAxis(std::get<PhzDataModel::ModelParameter::EBV>(grid_info.region_axes_map.at(region_name)));
      }

      if (conf.showRedshiftAxis()) {
        printAxis(std::get<PhzDataModel::ModelParameter::Z>(grid_info.region_axes_map.at(region_name)));
      }

      auto phot_coords = conf.getRequestedCellCoords();
      if (phot_coords) {
        auto& grid_map = config_manager.getConfiguration<PhzConfiguration::PhotometryGridConfig>().getPhotometryGrid();
        printPhotometry(grid_map.at(region_name), *phot_coords);
      }
    }


    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(DisplayModelGrid)
