/*
 * PhzModeling/ModelDatasetGrid.cpp
 *
 *  Created on: Sep 29, 2014
 *      Author: fdubath
 */

#include "PhzModeling/ModelDatasetGrid.h"

namespace Euclid {
namespace PhzModeling {

ModelDatasetGrid::ModelDatasetGrid(const PhzDataModel::ModelAxesTuple& parameter_space,
                  std::map<XYDataset::QualifiedName,XYDataset::XYDataset> sed_map,
                  std::map<XYDataset::QualifiedName,std::unique_ptr<MathUtils::Function> > reddening_curve_map,
                  ReddeningFunction reddening_function, RedshiftFunction redshift_function)
  : PhzDataModel::PhzGrid<ModelDatasetCellManager>(parameter_space) {

  m_sed_map=std::move(sed_map);
  m_reddening_curve_map=std::move(reddening_curve_map);
  m_reddening_function=std::move(reddening_function);
  m_redshift_function=std::move(redshift_function);
  size_t z_size = std::get<PhzDataModel::ModelParameter::Z>(parameter_space).size();
  size_t ebv_size = std::get<PhzDataModel::ModelParameter::EBV>(parameter_space).size();
  size_t reddening_curve_size = std::get<PhzDataModel::ModelParameter::REDDENING_CURVE>(parameter_space).size();
  size_t sed_size = std::get<PhzDataModel::ModelParameter::SED>(parameter_space).size();
  m_size = z_size * ebv_size * reddening_curve_size * sed_size;
}


PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator
 ModelDatasetGrid::begin(){

  return PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator(
      *this,
      ModelDatasetGenerator(getAxesTuple(),
      m_sed_map,
      m_reddening_curve_map,
      0,
      m_reddening_function,
      m_redshift_function));
}


PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator
 ModelDatasetGrid::end(){
  return PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator(
      *this,
      ModelDatasetGenerator(getAxesTuple(),
      m_sed_map,
      m_reddening_curve_map,
      m_size,
      m_reddening_function,
      m_redshift_function));
}


}
}