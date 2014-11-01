/**
 * @file tests/src/PhotometryAlgorithm_test.cpp
 * @date Oct 13, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <map>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzModeling/PhotometryAlgorithm.h"
#include "PhzModeling/ModelFluxAlgorithm.h"
#include "XYDataset/XYDataset.h"
#include "XYDataset/QualifiedName.h"
#include "ElementsKernel/PhysConstants.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

struct PhotometryAlgorithm_Fixture {

 class DummyFilterFunction:public Euclid::MathUtils::Function{
   public:
    DummyFilterFunction()=default;

    virtual ~DummyFilterFunction() = default;

    double operator()(double) const {
      return 1.;
    }

    std::unique_ptr<Function> clone() const{
       return std::unique_ptr<Function>{new DummyFilterFunction()};
    }
  };


  class DummyApplyFilter{
  public:
    DummyApplyFilter() = default;
    DummyApplyFilter(DummyApplyFilter&&) = default;
    DummyApplyFilter(const DummyApplyFilter&) = default;
    DummyApplyFilter& operator=(DummyApplyFilter&&) = default;
    DummyApplyFilter& operator=(const DummyApplyFilter&) = default;
    virtual ~DummyApplyFilter() = default;
    Euclid::XYDataset::XYDataset operator()(
          const Euclid::XYDataset::XYDataset& model,
          const std::pair<double,double>& filter_range,
          const Euclid::MathUtils::Function&
        ) const{
          std::vector<std::pair<double, double>> filtered_values {};
           for (auto& sed_pair : model) {
             if (sed_pair.first>filter_range.first){
               filtered_values.push_back(std::make_pair(sed_pair.first,sed_pair.first));
             }
           }
           return  Euclid::XYDataset::XYDataset {std::move(filtered_values)};

    }
  };

  class DummyFluxCalculator{
   public:
    DummyFluxCalculator() = default;
    DummyFluxCalculator(DummyFluxCalculator&&) = default;
    DummyFluxCalculator(const DummyFluxCalculator&) = default;
    DummyFluxCalculator& operator=(DummyFluxCalculator&&) = default;
    DummyFluxCalculator& operator=(const DummyFluxCalculator&) = default;
     virtual ~DummyFluxCalculator() = default;
     double operator()(const Euclid::XYDataset::XYDataset& filterd_model,double){

            return  filterd_model.size();
     }
   };


  std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,const std::pair<double,double>& , const Euclid::MathUtils::Function&)> m_apply_filter_function
      =std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset&,const std::pair<double,double>& , const Euclid::MathUtils::Function&)>(DummyApplyFilter{});

  std::function<double(const Euclid::XYDataset::XYDataset& ,double)> m_flux_function
     =std::function<double(const Euclid::XYDataset::XYDataset& ,double)>(DummyFluxCalculator{});



  std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> filter_map;
  std::vector<Euclid::XYDataset::QualifiedName> filter_name_list;

  PhotometryAlgorithm_Fixture(){
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter1"},makeFilter(9000.,0.1)));
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter2"},makeFilter(9000.,0.2)));
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter3"},makeFilter(9000.,0.3)));
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter4"},makeFilter(9000.,0.4)));
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter5"},makeFilter(11000.,0.5)));

    filter_name_list.push_back(Euclid::XYDataset::QualifiedName{"filterSet1/filter2"});
    filter_name_list.push_back(Euclid::XYDataset::QualifiedName{"filterSet1/filter5"});
  }

   std::vector<std::pair<double, double>> makeFilter(double first, double vlue){
       return std::vector<std::pair<double, double>>{
         std::make_pair(first,vlue),
         std::make_pair(12000.,vlue),
         std::make_pair(17000.,vlue),
         std::make_pair(18000.,vlue),
         std::make_pair(20000.,vlue)
       };
   }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometryAlgorithm_test)

BOOST_FIXTURE_TEST_CASE(Constructor_test, PhotometryAlgorithm_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzModeling::ModelFluxAlgorithm flux_model_algo{m_apply_filter_function,m_flux_function};
  Euclid::PhzModeling::PhotometryAlgorithm<Euclid::PhzModeling::ModelFluxAlgorithm> algo(std::move(flux_model_algo),std::move(filter_map),std::move(filter_name_list));
}

BOOST_FIXTURE_TEST_CASE(Exception_test, PhotometryAlgorithm_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the exception");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzModeling::ModelFluxAlgorithm flux_model_algo{m_apply_filter_function,m_flux_function};

  std::vector<Euclid::XYDataset::QualifiedName> local_filter_name_list;
  local_filter_name_list.push_back(Euclid::XYDataset::QualifiedName{"filterSet1/filter2"});
  local_filter_name_list.push_back(Euclid::XYDataset::QualifiedName{"filterSet1/filter-1"});


  BOOST_CHECK_THROW(
      Euclid::PhzModeling::PhotometryAlgorithm<Euclid::PhzModeling::ModelFluxAlgorithm> algo(std::move(flux_model_algo),std::move(filter_map),std::move(local_filter_name_list))
     ,Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(execution_test, PhotometryAlgorithm_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the execution");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzModeling::ModelFluxAlgorithm flux_model_algo{m_apply_filter_function,m_flux_function};
  Euclid::PhzModeling::PhotometryAlgorithm<Euclid::PhzModeling::ModelFluxAlgorithm> algo(std::move(flux_model_algo),std::move(filter_map),std::move(filter_name_list));

  auto model_1 = Euclid::XYDataset::XYDataset{std::vector<std::pair<double, double>>{
    std::make_pair(10000.,0.001),
    std::make_pair(12000.,0.001),
    std::make_pair(14000.,0.001),
    std::make_pair(16000.,0.001),
    std::make_pair(17000.,0.001)
  }};

  auto model_2 = Euclid::XYDataset::XYDataset{std::vector<std::pair<double, double>>{
    std::make_pair(10000.,0.002),
    std::make_pair(12000.,0.002),
    std::make_pair(14000.,0.002),
    std::make_pair(16000.,0.002),
    std::make_pair(17000.,0.002),
    std::make_pair(18000.,0.002)
  }};

  std::vector<Euclid::XYDataset::XYDataset> model_vector;
  model_vector.push_back(std::move(model_1));
  model_vector.push_back(std::move(model_2));

  auto ptr = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>());
  ptr->push_back("test");


  std::vector<Euclid::SourceCatalog::Photometry> photometry_vector;
  photometry_vector.push_back(Euclid::SourceCatalog::Photometry(ptr,std::vector<Euclid::SourceCatalog::FluxErrorPair>{{1.,0.}}));
  photometry_vector.push_back(Euclid::SourceCatalog::Photometry(ptr,std::vector<Euclid::SourceCatalog::FluxErrorPair>{{1.,0.}}));

  algo(model_vector.begin(),model_vector.end(),photometry_vector.begin());

  auto model_vector_iterator = model_vector.begin();
  for(auto& photometry:photometry_vector){
    // check that there is the right filters
   auto filter1 =photometry.find("filterSet1/filter1");
   BOOST_CHECK(!filter1);
   auto filter2 =photometry.find("filterSet1/filter2");
   BOOST_CHECK(filter2);
   auto filter5 =photometry.find("filterSet1/filter5");
   BOOST_CHECK(filter5);

   BOOST_CHECK_EQUAL(model_vector_iterator->size(),filter2->flux);
   BOOST_CHECK_EQUAL(model_vector_iterator->size()-1,filter5->flux);
   ++model_vector_iterator;
  }

}

BOOST_AUTO_TEST_SUITE_END ()
