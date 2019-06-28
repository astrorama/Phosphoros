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
 * @file src/program/ProcessPDF.cpp
 * @date 01/12/18
 * @author fdubath
 */

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <boost/program_options.hpp>
#include "ElementsKernel/ProgramHeaders.h"
#include "ElementsKernel/Exception.h"
#include "Configuration/ConfigManager.h"
#include "PhzConfiguration/RedshiftConfig.h"
#include "Configuration/Utils.h"
#include "PHZ_PdfHandling/PdfHandlingConfiguration.h"
#include "Table/FitsReader.h"
#include "Table/FitsWriter.h"
#include <cmath>
#include "MathUtils/PDF/PdfModeExtraction.h"
#include "MathUtils/PDF/Cumulative.h"
#include "XYDataset/XYDataset.h"

#include <CCfits/CCfits>
#include <regex>

using namespace Euclid;
using namespace Euclid::Configuration;
using namespace Euclid::PHZ_PdfHandling;
using namespace Euclid::Table;

namespace po = boost::program_options;

using boost::program_options::options_description;
using boost::program_options::variable_value;


static Elements::Logging logger = Elements::Logging::getLogger("ProcessPDF");

static long config_manager_id = getUniqueManagerId();

class ProcessPDF : public Elements::Program {

public:

  po::options_description defineSpecificProgramOptions() override {
      auto& config_manager = Euclid::Configuration::ConfigManager::getInstance(config_manager_id);
      config_manager.registerConfiguration<PdfHandlingConfiguration>();
      return config_manager.closeRegistration();
    }


  Elements::ExitCode mainMethod(std::map<std::string, variable_value>& args) override {
    auto& config_manager = ConfigManager::getInstance(config_manager_id);
    config_manager.initialize(args);


    Elements::Logging logger = Elements::Logging::getLogger("ProcessPDF");

    logger.info("#");
    logger.info("# Entering mainMethod()");

    std::string pdf_col_name = config_manager.getConfiguration<PdfHandlingConfiguration>().getPdfColName();

    std::regex likelihood_regex("LIKELIHOOD");

    logger.info("# Get the redshift sampling");
    CCfits::FITS fits_file(config_manager.getConfiguration<PdfHandlingConfiguration>().getInputCatalogName());
    CCfits::ExtHDU& extension = fits_file.extension(1);
    std::string comments = extension.getComments();

    std::regex range_regex("LIKELIHOOD-Z-BINS : \\{[0-9., \\r\\n]+\\}");
    if (regex_search(pdf_col_name, likelihood_regex)) {
      // Likelihood Case

    } else {
      // Posterior case
      range_regex = std::regex("[^-]*Z-BINS : \\{[0-9., \\r\\n]+\\}");
    }

    std::smatch range_match;
    std::vector<double> pdf_sampling{};
    if (std::regex_search(comments, range_match, range_regex)) {
        std::string ranges = range_match[0];
        ranges.erase(std::remove(ranges.begin(), ranges.end(), '\n'), ranges.end());
        ranges.erase(std::remove(ranges.begin(), ranges.end(), '\r'), ranges.end());
        ranges.erase(std::remove(ranges.begin(), ranges.end(), ' '), ranges.end());
        std::regex number_regex = std::regex("[0-9.]+");
        std::smatch res;
        string::const_iterator searchStart(ranges.cbegin());
        while (regex_search(searchStart, ranges.cend(), res, number_regex)) {
          pdf_sampling.push_back(atof(res[0].str().c_str()));
          searchStart = res.suffix().first;
        }
    } else {
      //=> no range found
      logger.error("The Pdf sampling was not found in the comments");
      throw Elements::Exception() << "The Pdf sampling was not found in the comments";
    }

    uint chunk_size = config_manager.getConfiguration<PdfHandlingConfiguration>().getChunkSize();
    std::string id_col_name = config_manager.getConfiguration<PdfHandlingConfiguration>().getIdColumnName();

    logger.info("# Open The input file");
    auto reader = FitsReader(config_manager.getConfiguration<PdfHandlingConfiguration>().getInputCatalogName());
    auto input_col = reader.getInfo();


    logger.info("# Create The output file");
    auto writer = FitsWriter(config_manager.getConfiguration<PdfHandlingConfiguration>().getOutputCatalogName(),true);


    logger.info("# Process the data");


    std::string prefix = config_manager.getConfiguration<PdfHandlingConfiguration>().getOutputColumnsPrefix();

    std::vector<ColumnInfo::info_type> info_full_list {
        ColumnInfo::info_type(prefix + "SOURCE_ID", typeid(std::string), "", "Unique ID"),
        ColumnInfo::info_type(prefix + "MEDIAN", typeid(double), "", "median"),
        ColumnInfo::info_type(prefix + "MIN_70", typeid(double), "", "bottom bound of the smallest 70% interval"),
        ColumnInfo::info_type(prefix + "MAX_70", typeid(double), "", "top bound of the smallest 70% interval"),
        ColumnInfo::info_type(prefix + "MIN_90", typeid(double), "", "bottom bound of the smallest 90% interval"),
        ColumnInfo::info_type(prefix + "MAX_90", typeid(double), "", "top bound of the smallest 90% interval"),
        ColumnInfo::info_type(prefix + "MIN_95", typeid(double), "", "bottom bound of the smallest 95% interval"),
        ColumnInfo::info_type(prefix + "MAX_95", typeid(double), "", "top bound of the smallest 95% interval"),
        ColumnInfo::info_type(prefix + "MED_CENTER_MIN_70", typeid(double), "", "bottom bound of the median centered 70% interval"),
        ColumnInfo::info_type(prefix + "MED_CENTER_MAX_70", typeid(double), "", "top bound of the median centered 70% interval"),
        ColumnInfo::info_type(prefix + "MED_CENTER_MIN_90", typeid(double), "", "bottom bound of the median centered 90% interval"),
        ColumnInfo::info_type(prefix + "MED_CENTER_MAX_90", typeid(double), "", "top bound of the median centered 90% interval"),
        ColumnInfo::info_type(prefix + "MED_CENTER_MIN_95", typeid(double), "", "bottom bound of the median centered 95% interval"),
        ColumnInfo::info_type(prefix + "MED_CENTER_MAX_95", typeid(double), "", "top bound of the median centered 95% interval"),
        ColumnInfo::info_type(prefix + "PHZ_MODE_1_SAMP", typeid(double), "", "Position of the highest sample of the first PDF mode"),
        ColumnInfo::info_type(prefix + "PHZ_MODE_1_MEAN", typeid(double), "", "Mean of the first PDF mode"),
        ColumnInfo::info_type(prefix + "PHZ_MODE_1_FIT", typeid(double), "", " Position of the interpolated max of the first PDF mode"),
        ColumnInfo::info_type(prefix + "PHZ_MODE_1_AREA", typeid(double), "", "Area of the first PDF mode"),
        ColumnInfo::info_type(prefix + "PHZ_MODE_2_SAMP", typeid(double), "", "Position of the highest sample of the second PDF mode"),
        ColumnInfo::info_type(prefix + "PHZ_MODE_2_MEAN", typeid(double), "", "Mean of the second PDF mode"),
        ColumnInfo::info_type(prefix + "PHZ_MODE_2_FIT", typeid(double), "", " Position of the interpolated max of the second PDF mode"),
        ColumnInfo::info_type(prefix + "PHZ_MODE_2_AREA", typeid(double), "", "Area of the second PDF mode")
    };

    std::vector<ColumnInfo::info_type> info_list{};
    std::map<int,bool> display_column_map{};

    auto excluded_list = config_manager.getConfiguration<PdfHandlingConfiguration>().getExcludedOutputColumns();
    for(size_t col_index=0; col_index< info_full_list.size();++col_index){
      auto col_name = info_full_list[col_index].name;
      col_name = col_name.substr(prefix.length());
      if (std::find(excluded_list.begin(), excluded_list.end(), col_name) == excluded_list.end()){
        // column is not excluded
        info_list.push_back(info_full_list[col_index]);
        display_column_map[col_index]=true;
      } else {
        display_column_map[col_index]=false;
      }
    }


    logger.info("# Outputting "+std::to_string(info_list.size()) +" columns out of "+ std::to_string(info_full_list.size())+" available." );

    std::shared_ptr<ColumnInfo> column_info {new ColumnInfo {info_list}};

    long total_row=0;
    while(reader.hasMoreRows()){

       logger.info("# Process "+std::to_string(chunk_size) +" rows from row "+std::to_string(total_row));
       total_row+=chunk_size;
       auto rows = reader.read(chunk_size);
       std::vector<Row> row_list {};

       for(auto& row : rows ){
         long id = boost::get<int64_t>(row[id_col_name]);
         try {
           std::vector<double> pdf=boost::get<std::vector<double>>(row[pdf_col_name]);

           auto cumul = MathUtils::Cumulative::fromPdf(pdf_sampling, pdf);
           double med = cumul.findValue(0.5);
           if (isnan(med)){
             throw  Elements::Exception("Source with ID" +std::to_string(id) + "has undefined MEDIAN");
           }

           std::pair<double,double> range_70 = cumul.findMinInterval(0.7);
           std::pair<double,double> range_90 = cumul.findMinInterval(0.9);
           std::pair<double,double> range_95 = cumul.findMinInterval(0.95);

           std::pair<double,double> med_c_range_70 = cumul.findCenteredInterval(0.7);
           std::pair<double,double> med_c_range_90 = cumul.findCenteredInterval(0.9);
           std::pair<double,double> med_c_range_95 = cumul.findCenteredInterval(0.95);
           
           
           std::vector<MathUtils::ModeInfo> modes{};
           auto full_pdf = Euclid::XYDataset::XYDataset::factory(pdf_sampling, pdf);
           
           if (config_manager.getConfiguration<PdfHandlingConfiguration>().getBiggestAreaMode()){
             modes = MathUtils::extractNBigestModes(full_pdf, config_manager.getConfiguration<PdfHandlingConfiguration>().getMergeRatio(), 2);
           }else{
             modes = MathUtils::extractNHighestModes(full_pdf, config_manager.getConfiguration<PdfHandlingConfiguration>().getMergeRatio(), 2);
           }

           std::vector<Row::cell_type> full_values0 {std::to_string(id), med,
                                                range_70.first, range_70.second,
                                                range_90.first,range_90.second,
                                                range_95.first,range_95.second,
                                                med_c_range_70.first, med_c_range_70.second,
                                                med_c_range_90.first,med_c_range_90.second,
                                                med_c_range_95.first,med_c_range_95.second,
                                                modes[0].getHighestSamplePosition(),
                                                modes[0].getMeanPosition(),
                                                modes[0].getInterpolatedMaxPosition(),
                                                modes[0].getModeArea(),
                                                modes[1].getHighestSamplePosition(),
                                                modes[1].getMeanPosition(),
                                                modes[1].getInterpolatedMaxPosition(),
                                                modes[1].getModeArea()
           };

           std::vector<Row::cell_type> values0{};
           for(size_t col_index=0; col_index< full_values0.size();++col_index){
             if (display_column_map[col_index]){
               values0.push_back(full_values0[col_index]);
             }
           }

           Row row0 {values0, column_info};
           row_list.push_back(row0);

         } catch (Elements::Exception e){
           logger.warn("The processing of the PDF of the source with ID "  +std::to_string(id) + " has failed :"+e.what());
         }

       }
       Euclid::Table::Table table {row_list};

       writer.addData(table);

    }



    logger.info("#");
    logger.info("# Exiting mainMethod()");
    logger.info("#");

    return Elements::ExitCode::OK;
  }

};

MAIN_FOR(ProcessPDF)



