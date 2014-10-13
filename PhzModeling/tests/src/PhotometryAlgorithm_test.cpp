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

struct PhotometryAlgorithm_Fixture {

 class DummyFilterFunction:public Euclid::MathUtils::Function{
   public:
    DummyFilterFunction()=default;

    virtual ~DummyFilterFunction() = default;

    double operator()(const double x) const {
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
          const Euclid::MathUtils::Function& filter
        ) const{
          std::vector<std::pair<double, double>> filtered_values {};
           for (auto& sed_pair : model) {
             if (sed_pair.first>filter_range.first)
             {
               filtered_values.push_back(std::make_pair(sed_pair.first,sed_pair.second));
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
     double operator()(const Euclid::XYDataset::XYDataset& filterd_model,double normalization){
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
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter1"},makeFilter(0.1)));
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter2"},makeFilter(0.2)));
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter3"},makeFilter(0.3)));
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter4"},makeFilter(0.4)));
    filter_map.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filterSet1/filter5"},makeFilter(0.5)));

    filter_name_list.push_back(Euclid::XYDataset::QualifiedName{"filterSet1/filter2"});
    filter_name_list.push_back(Euclid::XYDataset::QualifiedName{"filterSet1/filter5"});
  }

   std::vector<std::pair<double, double>> makeFilter(double vlue){
       return std::vector<std::pair<double, double>>{
         std::make_pair(1.,vlue),
         std::make_pair(2.,vlue),
         std::make_pair(3.,vlue),
         std::make_pair(4.,vlue),
         std::make_pair(5.,vlue)
       };
   }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometryAlgorithm_test)

//-----------------------------------------------------------------------------
// Check that the functor returns a XYDataSet with the same length that the input one
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(outputLength_test, PhotometryAlgorithm_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the length of the returned model");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzModeling::ModelFluxAlgorithm flux_model_algo{m_apply_filter_function,m_flux_function};



// in progress
// auto algo =  Euclid::PhzModeling::PhotometryAlgorithm<Euclid::PhzModeling::ModelFluxAlgorithm>{std::move(flux_model_algo),filter_map,filter_name_list};

}

BOOST_AUTO_TEST_SUITE_END ()
