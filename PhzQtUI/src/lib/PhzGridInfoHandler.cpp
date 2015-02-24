/*
 * GridAxisHandler.cpp
 *
 *  Created on: Feb 24, 2015
 *      Author: fdubath
 */

#include <vector>
#include "PhzQtUI/PhzGridInfoHandler.h"
#include "PhzQtUI/XYDataSetTreeModel.h"
#include "XYDataset/QualifiedName.h"
#include "PhzQtUI/FileUtils.h"

namespace Euclid {
namespace PhosphorosUiDm {

PhzDataModel::ModelAxesTuple PhzGridInfoHandler::getAxesTuple(const ModelSet& model_set){
  std::vector<double> zs{};
  std::vector<double> ebvs{};
  std::vector<XYDataset::QualifiedName> reddening_curves{};
  std::vector<XYDataset::QualifiedName> seds{};

  auto rules = model_set.getParameterRules();
  for (auto& rule : rules){
    auto z_range = rule.second.getZRange();
    if (z_range.getStep()>0 && z_range.getMax()>z_range.getMin()){
           for (double z=z_range.getMin(); z<= z_range.getMax(); z+=z_range.getStep()){
             zs.push_back(z);
           }
    } else{
         zs.push_back(0.);
    }

    auto red_range = rule.second.getEbvRange();
    if (red_range.getStep()>0 && red_range.getMax()>red_range.getMin()){
           for (double ebv=red_range.getMin(); ebv<= red_range.getMax(); ebv+=red_range.getStep()){
             ebvs.push_back(ebv);
           }
    } else{
      ebvs.push_back(0.);
    }


    XYDataSetTreeModel treeModel_sed;
    treeModel_sed.loadDirectory(Euclid::PhosphorosUiDm::FileUtils::getSedRootPath(false),false,"SEDs");
    treeModel_sed.setState(rule.second.getSedRootObject(),rule.second.getExcludedSeds());
    for(auto sed : treeModel_sed.getSelectedLeaf(rule.second.getSedRootObject())){
      // check if  / is needed on root objects...
      seds.push_back(XYDataset::QualifiedName{sed});
    }

    XYDataSetTreeModel treeModel_red;
    treeModel_red.loadDirectory(Euclid::PhosphorosUiDm::FileUtils::getRedCurveRootPath(false),false,"Reddening Curves");
    treeModel_red.setState(rule.second.getReddeningRootObject(),rule.second.getExcludedReddenings());
    for(auto red : treeModel_red.getSelectedLeaf(rule.second.getReddeningRootObject())){
      // check if  / is needed on root objects...
      reddening_curves.push_back(XYDataset::QualifiedName{red});
    }


    // TODO By now we assume that there is a single parameter rules
    break;
  }


  return PhzDataModel::createAxesTuple(zs, ebvs, reddening_curves, seds);
}

}
}

