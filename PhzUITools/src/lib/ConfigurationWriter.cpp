/*
 * ConfigurationWriter.cpp
 *
 *  Created on: Feb 25, 2015
 *      Author: fdubath
 */
#include <fstream>
#include <iostream>
#include "PhzUITools/ConfigurationWriter.h"

namespace Euclid {
namespace PhzUITools {


bool ConfigurationWriter::writeConfiguration(
    const std::map<std::string, po::variable_value> & config,
    std::string file_name){

  std::ofstream config_file ( file_name );


  for(auto& conf_item : config){


    try{
      config_file<<conf_item.first<<"="<< boost::any_cast<std::string>(conf_item.second.value())<<"\n";
    }
    catch(...){
      for (auto& item : boost::any_cast<std::vector<std::string>>(conf_item.second.value()) ){
            config_file<<conf_item.first<<"="<<item<<"\n";
          }
    }
    config_file<<"\n";

  }

  return true;
}

}
}


