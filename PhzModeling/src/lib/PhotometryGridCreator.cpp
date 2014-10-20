/**
 * @file PhzModeling/PhotometryGridCreator.cpp
 * @date Oct 20 2014
 * @author Florian Dubath
 */


#include "MathUtils/interpolation/interpolation.h"

#include "PhzDataModel/PhzModel.h"
#include "PhzDataModel/PhotometryGrid.h"

#include "PhzModeling/ExtinctionFunctor.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "PhzModeling/ApplyFilterFunctor.h"
#include "PhzModeling/CalculateFluxFunctor.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "PhzModeling/PhotometryAlgorithm.h"

#include "PhzModeling/PhotometryGridCreator.h"


namespace Euclid {
namespace PhzModeling {


  PhotometryGridCreator::PhotometryGridCreator(
      PhzDataModel::ModelAxesTuple parameter_space,
      std::vector<Euclid::XYDataset::QualifiedName> filter_name_list,
      std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider,
      std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_curve_provider,
      std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider):
      m_parameter_space{std::move(parameter_space)},
      m_filter_name_list{std::move(filter_name_list)},
      m_sed_provider{std::move(sed_provider)},
      m_reddening_curve_provider{std::move(reddening_curve_provider)},
      m_filter_provider(std::move(filter_provider))
  {
    buildFilterMap();
    buildSedMap();
    buildReddeningCurveMap();
  }

  PhzDataModel::PhotometryGrid PhotometryGridCreator::operator()(){
    // Define the functions based on the Functors
    auto reddening_function= std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset& ,const Euclid::MathUtils::Function&, double)>(ExtinctionFunctor{});
    auto redshift_function= std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset& , double)>(RedshiftFunctor{});
    auto apply_filter_function=std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,const std::pair<double,double>& , const Euclid::MathUtils::Function&)>(ApplyFilterFunctor{});
    auto flux_function=std::function<double(const Euclid::XYDataset::XYDataset& ,double)>(CalculateFluxFunctor{});
    PhzModeling::ModelFluxAlgorithm flux_model_algo {apply_filter_function,flux_function};
    // Create the model grid
    auto model_grid= PhzModeling::ModelDatasetGrid(m_parameter_space,std::move(m_sed_map),std::move(m_reddening_curve_map),reddening_function,redshift_function);

    // Create the photometry Grid
    auto photometry_grid = PhzDataModel::PhotometryGrid(m_parameter_space);

    // Define the algo

    Euclid::PhzModeling::PhotometryAlgorithm<Euclid::PhzModeling::ModelFluxAlgorithm> photometry_algo(std::move(flux_model_algo),std::move(m_filter_map),std::move(m_filter_name_list));

    // Do the computation
    photometry_algo(model_grid.begin(),model_grid.end(),photometry_grid.begin());

    return std::move(photometry_grid);
  }


  void PhotometryGridCreator::buildFilterMap(){
    for (auto filter_name:m_filter_name_list){

      auto filter_ptr = m_filter_provider->getDataset(filter_name);
      if(!filter_ptr){
        throw  Elements::Exception() << "PhotometryGridCreator:: The filter provider do not contains a filter named :"<<filter_name.qualifiedName();
      }

      m_filter_map.insert(std::make_pair(filter_name,std::move(*filter_ptr)));
    }
  }

  void PhotometryGridCreator::buildSedMap(){
   for(auto sed_name:std::get<Euclid::PhzDataModel::ModelParameter::SED>(m_parameter_space)){
     auto sed_ptr = m_sed_provider->getDataset(sed_name);
     if (!sed_ptr){
       throw  Elements::Exception() << "PhotometryGridCreator:: The SED provider do not contains a SED named :"<<sed_name.qualifiedName();
     }

     m_sed_map.insert(std::make_pair(sed_name,std::move(*m_sed_provider->getDataset(sed_name))));
   }
  }

  void PhotometryGridCreator::buildReddeningCurveMap(){
   for(auto reddening_curve_name:std::get<Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE>(m_parameter_space)){

     auto curve_dataset_ptr = m_reddening_curve_provider->getDataset(reddening_curve_name);
     if (!curve_dataset_ptr){
           throw  Elements::Exception() << "PhotometryGridCreator:: The Reddening curve provider do not contains a curve named :"<<reddening_curve_name.qualifiedName();
         }
     auto function_ptr = Euclid::MathUtils::interpolate(*curve_dataset_ptr, Euclid::MathUtils::InterpolationType::LINEAR);

     m_reddening_curve_map.insert(std::make_pair(reddening_curve_name,std::move(function_ptr)));
   }
  }


}
}

