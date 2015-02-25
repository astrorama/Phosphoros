/*
 * ConfigurationWriter.h
 *
 *  Created on: Feb 25, 2015
 *      Author: fdubath
 */

#ifndef CONFIGURATIONWRITER_H_
#define CONFIGURATIONWRITER_H_

#include<map>
#include<string>
#include <boost/program_options.hpp>
namespace po = boost::program_options;


namespace Euclid {
namespace PhzUITools {

class ConfigurationWriter{
public:
static bool writeConfiguration(const std::map<std::string, po::variable_value>& config, std::string file_name);

};
}
}
#endif /* CONFIGURATIONWRITER_H_ */
