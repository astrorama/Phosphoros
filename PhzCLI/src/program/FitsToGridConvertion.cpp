/*
 * Copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file program/FitsToGridConvertion.cpp
 *
 * This code is used to convert back a grid stored as a fits file (Created from PhosphorosDisplayModelGrid).
 * You have to feed it with the config file used to create the grid and the fits files containing the photometries.
 *
 * @date 2023/04/03
 * @author dubathf
 */
#include <regex>
#include <map>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <boost/algorithm/string.hpp>

#include <ElementsKernel/ProgramHeaders.h>
#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Exception.h"

#include "Table/Table.h"
#include "Table/ColumnInfo.h"
#include "XYDataset/QualifiedName.h"
#include "PhzCLI/FitsToGridConfig.h"
#include "PhzConfiguration/FilterConfig.h"
#include "PhzConfiguration/ModelGridOutputConfig.h"
#include "PhzConfiguration/IgmConfig.h"
#include "PhzConfiguration/ParameterSpaceConfig.h"
#include "Configuration/CatalogConfig.h"

#include "Configuration/Utils.h"

#include "PhzDataModel/PhotometryGrid.h"
#include "PhzDataModel/PhotometryGridInfo.h"


using namespace Euclid;
using namespace Euclid::PhzConfiguration;
namespace po = boost::program_options;

static Elements::Logging logger = Elements::Logging::getLogger("FitsToGridConvertion");

static long config_manager_id = Euclid::Configuration::getUniqueManagerId();

std::shared_ptr<std::vector<std::string>> buildFilterPointer(const std::vector<Euclid::XYDataset::QualifiedName>& filter_list_in){
    std::vector<std::string> filter_list{};

    for (auto iter = filter_list_in.begin(); iter!=filter_list_in.end(); ++iter){
      	filter_list.push_back(iter->qualifiedName());
    }

    std::shared_ptr<std::vector<std::string>> filters_ptr = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{filter_list});

    return filters_ptr;
}

void checkInputColumns(std::shared_ptr<Table::ColumnInfo> column_info, std::shared_ptr<std::vector<std::string>> filters_ptr ){
	if (filters_ptr->size()+5 != column_info->size()) {
		logger.error()<< "The number of columns in the input file (" << column_info->size() <<
				") do not match the expected number (from the grid config) which is "<< (filters_ptr->size()+5) ;
		throw Elements::Exception() << "The number of columns in the input file (" << column_info->size() <<
				") do not match the expected number (from the grid config) which is "<< (filters_ptr->size()+5) ;
	}

	logger.info() << "Check the columns names";
	std::vector<std::string> expectedColName{"ID","Model_SED","Model_RedCurve","Model_EBV","Model_Z"};
	for (size_t id=0; id <  expectedColName.size(); ++id) {
		 if (column_info->getDescription(id).name!=expectedColName[id]) {
			 logger.error()<< "The column at position " << id <<
							" is expected to be named "<< expectedColName[id] << " but the effective name is "<< column_info->getDescription(id).name;
					throw Elements::Exception() <<"The column at position " << id <<
							" is expected to be named "<< expectedColName[id] << " but the effective name is "<< column_info->getDescription(id).name;
		 }
	}
}

std::map<std::string, size_t> buildCommentMap(std::string comment){
	std::vector<std::string> bits;
	boost::split(bits, comment, boost::is_any_of(","));
	std::map<std::string, size_t> result{};
	for (size_t index=0; index<bits.size(); ++index) {
		std::string key = std::regex_replace( bits[index], std::regex("\\s|\\r\\n|\\r|\\n"), "");
		result.emplace(key,index);
	}
	return result;
}


bool matchingRow(const Euclid::Table::Table& data, size_t row_index, double z, double ebv, size_t red_index, size_t sed_index) {
	 size_t curr_sed = boost::get<std::int32_t>(data[row_index][1]);
	 if (curr_sed == sed_index) {
		size_t curr_red = boost::get<std::int32_t>(data[row_index][2]);
		if (curr_red == red_index) {
			double curr_ebv = boost::get<float>(data[row_index][3]);
			if (std::abs(curr_ebv - ebv)<0.000001) {
				double curr_z = boost::get<float>(data[row_index][4]);
				if (std::abs(curr_z - z)<0.000001) {
					return true;
				}
			}
		}
	 }

	 return false;
}

class FitsToGridConvertion : public Elements::Program {

  po::options_description defineSpecificProgramOptions() override {
    auto& config_manager = Euclid::Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.registerConfiguration<PhzCLI::FitsToGridConfig>();
    return config_manager.closeRegistration();
  }

  Elements::ExitCode mainMethod(std::map<std::string, po::variable_value>& args) override {

    auto& config_manager = Euclid::Configuration::ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);

    logger.info() << "Build the empty grid";
    const std::map<std::string, PhzDataModel::ModelAxesTuple>& parameter_space = config_manager.getConfiguration<ParameterSpaceConfig>().getParameterSpaceRegions();
    std::shared_ptr<std::vector<std::string>> filters_ptr = buildFilterPointer(config_manager.getConfiguration<FilterConfig>().getFilterList());
  	std::map<std::string, PhzDataModel::PhotometryGrid>	grids{};


    logger.info() << "Check the input table";
    checkInputColumns(config_manager.getConfiguration<Euclid::Configuration::CatalogConfig>().getColumnInfo(), filters_ptr);


    logger.info() << "Get the SED and Reddening curve info";
    auto table_reader = config_manager.getConfiguration<Euclid::Configuration::CatalogConfig>().getTableReader();
    auto comments = table_reader->getComment();
	// Create dictionary to convert axis values to index
	std::vector<std::string> result;
	boost::split(result, comments, boost::is_any_of("[]"));
	auto sed_list = result[1];
	std::map<std::string, size_t> sed_dict = buildCommentMap(sed_list);

	auto red_list = result[3];
	std::map<std::string, size_t> red_dict = buildCommentMap(red_list);

	logger.info() << "Read the input file";
    auto data_table = table_reader->read();

	logger.info() << "Fill the grid";
    for (auto map_iter = parameter_space.cbegin(); map_iter != parameter_space.cend(); ++map_iter) {
    	const std::string& name = map_iter->first;

    	PhzDataModel::PhotometryGrid grid{map_iter->second, *filters_ptr};
   	    logger.info() << "Processing grid region " << name ;

    	size_t current_row=0;
    	for (size_t sed_index=0; sed_index<std::get<3>(map_iter->second).size(); ++sed_index) {
    	  auto sed_value = sed_dict[std::get<3>(map_iter->second)[sed_index].qualifiedName()];
    	  for (size_t red_index=0; red_index<std::get<2>(map_iter->second).size(); ++red_index) {
        	auto red_value = red_dict[std::get<2>(map_iter->second)[red_index].qualifiedName()];
    	    for (size_t ebv_index=0; ebv_index<std::get<1>(map_iter->second).size(); ++ebv_index) {
    	      double ebv_val = std::get<1>(map_iter->second)[ebv_index];
    		  for (size_t z_index=0; z_index<std::get<0>(map_iter->second).size(); ++z_index) {
        	      double z_val = std::get<0>(map_iter->second)[z_index];
    			  double start_row = current_row;
    			  bool found = false;
    			  while (!found) {
    				  if (matchingRow(data_table,current_row, z_val, ebv_val, red_value, sed_value)){
						  found=true;
						  std::vector<Euclid::SourceCatalog::FluxErrorPair> values{};
						  for (size_t filter_index=0; filter_index<filters_ptr->size(); ++filter_index){
							  double phot_val = boost::get<double>(data_table[current_row][5 + filter_index]);
							  Euclid::SourceCatalog::FluxErrorPair pair{phot_val,0};
							  values.push_back(pair);
						  }

						  grid(z_index, ebv_index, red_index, sed_index) = Euclid::SourceCatalog::Photometry{filters_ptr, values};
    				  }
    				  ++current_row;
    				  if (current_row == data_table.size()){
    					  current_row=0;
    				  }

    				  if (current_row==start_row) {
    					logger.error()<< "Photometry for (z="<<z_val<<", ebv="<<ebv_val<<", red_index="<<red_index<<", sed_index="<<sed_index<<") not found in the input file!";
    					throw Elements::Exception() << "Photometry for (z="<<z_val<<", ebv="<<ebv_val<<", red_index="<<red_index<<", sed_index="<<sed_index<<") not found in the input file!";
    				  }
    			  }
    		  }
    	    }
          }
    	}

    	grids.emplace(name, std::move(grid));
    }

    logger.info() << "Outputing the result grid";
    auto outputFunction =  config_manager.getConfiguration<ModelGridOutputConfig>().getOutputFunction();
    outputFunction(grids);

    return Elements::ExitCode::OK;
  }
};

MAIN_FOR(FitsToGridConvertion)
