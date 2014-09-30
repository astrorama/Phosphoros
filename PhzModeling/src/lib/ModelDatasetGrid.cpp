/*
 * PhzModeling/ModelDatasetGrid.cpp
 *
 *  Created on: Sep 29, 2014
 *      Author: fdubath
 */

#include "PhzModeling/ModelDatasetGrid.h"

Euclid::PhzModeling::ModelDatasetGrid::ModelDatasetGrid(const Euclid::PhzDataModel::ModelAxesTuple& parameter_space,
   std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> sed_map,
   std::map<Euclid::XYDataset::QualifiedName,std::unique_ptr<Euclid::MathUtils::Function> > reddening_curve_map,
   std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,const Euclid::MathUtils::Function&, double)> reddening_function,
   std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&, double)> redshift_function): Euclid::PhzDataModel::PhzGrid<ModelDatasetCellManager>(parameter_space){

  m_sed_map=std::move(sed_map);
  m_reddening_curve_map=std::move(reddening_curve_map);
  m_reddening_function=std::move(reddening_function);
  m_redshift_function=std::move(redshift_function);
  size_t z_size = std::get<Euclid::PhzDataModel::ModelParameter::Z>(getAxesTuple()).size();
  size_t ebv_size = std::get<Euclid::PhzDataModel::ModelParameter::EBV>(getAxesTuple()).size();
  size_t reddening_curve_size = std::get<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(getAxesTuple()).size();
  size_t sed_size = std::get<Euclid::PhzDataModel::ModelParameter::SED>(getAxesTuple()).size();
  m_size = z_size * ebv_size * reddening_curve_size * sed_size;
}


Euclid::PhzDataModel::PhzGrid<Euclid::PhzModeling::ModelDatasetCellManager>::iterator Euclid::PhzModeling::ModelDatasetGrid::begin(){

  return Euclid::PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator(*this, Euclid::PhzModeling::ModelDatasetGenerator(getAxesTuple(), m_sed_map, m_reddening_curve_map, 0, m_reddening_function,m_redshift_function));
}


Euclid::PhzDataModel::PhzGrid<Euclid::PhzModeling::ModelDatasetCellManager>::iterator Euclid::PhzModeling::ModelDatasetGrid::end(){
  return Euclid::PhzDataModel::PhzGrid<ModelDatasetCellManager>::iterator(*this, Euclid::PhzModeling::ModelDatasetGenerator(getAxesTuple(), m_sed_map, m_reddening_curve_map, m_size, m_reddening_function,m_redshift_function));
}
