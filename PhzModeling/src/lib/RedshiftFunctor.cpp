/**
 * @file PhzModeling/RedshiftFunctor.cpp
 * @date Sep 16, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/RedshiftFunctor.h"

namespace Euclid {
namespace PhzModeling {

Euclid::XYDataset::XYDataset RedshiftFunctor::operator()(const Euclid::XYDataset::XYDataset& sed, double z) const {
	    std::vector<std::pair<double, double>> redshifted_values {};
	    for (auto& sed_pair : sed) {
	    	redshifted_values.push_back(std::make_pair(sed_pair.first*(1+z),sed_pair.second/((1+z)*(1+z))));
	    }
	    return  Euclid::XYDataset::XYDataset {std::move(redshifted_values)};
	  }

} // end of namespace PhzModeling
} // end of namespace Euclid

