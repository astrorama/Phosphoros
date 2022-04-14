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
#include "NdArray/NdArray.h"
#include <pybind11/pybind11.h>

using namespace std;
using namespace Euclid;
namespace po = boost::program_options;




static Elements::Logging logger = Elements::Logging::getLogger("_DisplayModelGrid");
static long config_manager_id = Euclid::Configuration::getUniqueManagerId();
namespace py = pybind11;



void readModelGrid(const std::string& catalog_type, const std::string& intermediate_file_dir, const std::string& grid_name, py::buffer& out_array) {
	map<string, po::variable_value> options_map {};
	options_map["catalog-type"].value() = boost::any(catalog_type);
	if (intermediate_file_dir.length() > 0) {
		options_map["intermediate-products-dir"].value() = boost::any(intermediate_file_dir);
	}
    options_map["model-grid-file"].value() = boost::any(grid_name);

	auto& config_manager = Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<PhzConfiguration::PhotometryGridConfig>();
    config_manager.closeRegistration();
    config_manager.initialize(options_map);
    auto& grid_map = config_manager.getConfiguration<PhzConfiguration::PhotometryGridConfig>().getPhotometryGrid();

    long total_size=0;
    int photometry_band_number = 0;
    for (auto& pair : grid_map) {
        photometry_band_number = (*(pair.second.begin())).size();

    	auto& grid = pair.second;

    	total_size+=grid.getAxis<PhzDataModel::ModelParameter::SED>().size()  *
    			grid.getAxis<PhzDataModel::ModelParameter::REDDENING_CURVE>().size()*
				grid.getAxis<PhzDataModel::ModelParameter::EBV>().size()*
				grid.getAxis<PhzDataModel::ModelParameter::Z>().size();
    }

    // Create the NdArray
    auto data = Euclid::NdArray::NdArray<double>({5+photometry_band_number, total_size});
    size_t grid_index = 0;
    size_t row = 0;
    for (auto& pair : grid_map) {
    	 for (auto it = (pair.second).begin(); it != (pair.second).end(); ++it) {
    		 data.at(std::vector<size_t>{0,row}) = grid_index;
    		 data.at(std::vector<size_t>{1,row}) = it.axisIndex<PhzDataModel::ModelParameter::SED>();
    		 data.at(std::vector<size_t>{2,row}) = it.axisIndex<PhzDataModel::ModelParameter::REDDENING_CURVE>();
    		 data.at(std::vector<size_t>{3,row}) = it.axisIndex<PhzDataModel::ModelParameter::EBV>();
    		 data.at(std::vector<size_t>{4,row}) = it.axisIndex<PhzDataModel::ModelParameter::Z>();
    		 int band_index = 5;
    		 for (auto it_band = (*it).begin(); it_band!=(*it).end(); ++it_band) {
    			 data.at({band_index,row})=(*it_band).flux;
    			 ++band_index;
    		 }
    		 ++row;
    	 }
    	 ++grid_index;
    }
    out_array(data);
}


PYBIND11_MODULE(_DisplayModelGrid, m) {
    m.doc() = "Function reading a modelgrid and exposing the models as a catalog"; // optional module docstring

    m.def("readModelGrid", &readModelGrid, "A function that read a Model Grid into a py::buffer");
}

