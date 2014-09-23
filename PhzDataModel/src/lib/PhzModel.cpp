/** 
 * @file PhzDataModel/PhzModel.cpp
 * @date sep 17, 2014
 * @author Florian Dubath
 */

#include "PhzDataModel/PhzModel.h"

namespace Euclid {
namespace PhzDataModel {



ModelAxesTuple createAxesTuple(std::vector<double> zs, std::vector<double> ebvs,
                               std::vector<Euclid::XYDataset::QualifiedName> reddening_curves,
                               std::vector<Euclid::XYDataset::QualifiedName> seds) {
  Euclid::GridContainer::GridAxis<double> z_axis {"Z", std::move(zs)};
  Euclid::GridContainer::GridAxis<double> ebv_axis {"E(B-V)", std::move(ebvs)};
  Euclid::GridContainer::GridAxis<Euclid::XYDataset::QualifiedName> reddening_curves_axis {"Reddening Curve", std::move(reddening_curves)};
  Euclid::GridContainer::GridAxis<Euclid::XYDataset::QualifiedName> sed_axis {"SED", std::move(seds)};
  return ModelAxesTuple{std::move(z_axis), std::move(ebv_axis),
                        std::move(reddening_curves_axis), std::move(sed_axis)};
}


}
} // end of namespace Euclid


