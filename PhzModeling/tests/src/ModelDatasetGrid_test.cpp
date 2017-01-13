/**
 * @file tests/src/ModelDatasetGenerator_test.cpp
 * @date Sep 30, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzModeling/RedshiftFunctor.h"
#include "XYDataset/XYDataset.h"
#include "PhzModeling/ModelDatasetGrid.h"
#include "MathUtils/function/Function.h"
#include "PhzModeling/ExtinctionFunctor.h"

struct ModelDatasetGrid_Fixture {
  class DummyRedshift{
  public:

    virtual ~DummyRedshift() = default;
    Euclid::XYDataset::XYDataset operator()(const Euclid::XYDataset::XYDataset& sed, double z) const{
      std::vector<std::pair<double, double>> redshifted_values {};
      for (auto& sed_pair : sed) {
       redshifted_values.push_back(std::make_pair(sed_pair.first*(1+z),sed_pair.second));
      }
      return  Euclid::XYDataset::XYDataset {std::move(redshifted_values)};
    }
  };



  class DummyReddening{
   public:

     virtual ~DummyReddening() = default;
     Euclid::XYDataset::XYDataset operator()(const Euclid::XYDataset::XYDataset& sed,const Euclid::MathUtils::Function& reddening_curve, double ebv) const{
       std::vector<std::pair<double, double>> redshifted_values {};
       for (auto& sed_pair : sed) {
          redshifted_values.push_back(std::make_pair(sed_pair.first,sed_pair.second*(1+reddening_curve(sed_pair.first) * ebv)));
       }
        return  Euclid::XYDataset::XYDataset {std::move(redshifted_values)};
     }

   };

  class DummyExtinctionFunction:public Euclid::MathUtils::Function{
   public:
    DummyExtinctionFunction()=default;

    DummyExtinctionFunction(double factor):m_factor{factor}{}

    virtual ~DummyExtinctionFunction() = default;

    double operator()(const double x) const {
      if (x<1100.){
        return 2.*m_factor;
      }

      if (x<1300.){
        return 1.5*m_factor;
      }
      return 1.*m_factor;
    }

    std::unique_ptr<Function> clone() const{
       return std::unique_ptr<Function>{new DummyExtinctionFunction()};
    }
   private:
    double m_factor=1.0;
  };



  std::vector<double> zs{0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
  std::vector<double> ebvs{0.0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009,0.01};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"},{"reddeningCurves/Curve2"},{"reddeningCurves/Curve3"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve_1"},{"sed/Curve_2"},{"sed/Curve_3"}};

  Euclid::PhzDataModel::ModelAxesTuple parameter_space= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);

  std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> m_sed_map;
  std::map<Euclid::XYDataset::QualifiedName,std::unique_ptr<Euclid::MathUtils::Function> > m_reddening_curve_map;

  std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset& ,const Euclid::MathUtils::Function&, double)> m_reddening_function =  std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset& ,const Euclid::MathUtils::Function&, double)>(DummyReddening{});
  std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset& , double)> m_redshift_function=std::function<Euclid::XYDataset::XYDataset(const Euclid::XYDataset::XYDataset& , double)>(DummyRedshift{});

  std::vector<std::pair<double, double>> sed1 {std::make_pair(10000.,0.004),std::make_pair(12000.,0.002),std::make_pair(14000.,0.001)};
  std::vector<std::pair<double, double>> sed2 {std::make_pair(10000.,0.005),std::make_pair(12000.,0.003),std::make_pair(14000.,0.002)};
  std::vector<std::pair<double, double>> sed3 {std::make_pair(10000.,0.006),std::make_pair(12000.,0.004),std::make_pair(14000.,0.003)};

  DummyExtinctionFunction red1={1.};
  DummyExtinctionFunction red2={2.};
  DummyExtinctionFunction red3={4.};

  ModelDatasetGrid_Fixture(){
    m_sed_map.emplace(Euclid::XYDataset::QualifiedName("sed/Curve_1"),sed1);
    m_sed_map.emplace(Euclid::XYDataset::QualifiedName("sed/Curve_2"),sed2);
    m_sed_map.emplace(Euclid::XYDataset::QualifiedName("sed/Curve_3"),sed3);

    m_reddening_curve_map.emplace(Euclid::XYDataset::QualifiedName("reddeningCurves/Curve1"),std::unique_ptr<Euclid::MathUtils::Function>(new DummyExtinctionFunction(1.)));
    m_reddening_curve_map.emplace(Euclid::XYDataset::QualifiedName("reddeningCurves/Curve2"),std::unique_ptr<Euclid::MathUtils::Function>(new DummyExtinctionFunction(2.)));
    m_reddening_curve_map.emplace(Euclid::XYDataset::QualifiedName("reddeningCurves/Curve3"),std::unique_ptr<Euclid::MathUtils::Function>(new DummyExtinctionFunction(4.)));
  }
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ModelDatasetGrid_test)
//----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(constructor_test, ModelDatasetGrid_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing constructor");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzModeling::ModelDatasetGrid model_grid(parameter_space,std::move(m_sed_map),std::move(m_reddening_curve_map),m_reddening_function,m_redshift_function);

  BOOST_CHECK_EQUAL(4,model_grid.axisNumber());
  BOOST_CHECK_EQUAL(990,model_grid.size()); //3*3*11*10
}

BOOST_FIXTURE_TEST_CASE(iterator_test, ModelDatasetGrid_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing iterator");
  BOOST_TEST_MESSAGE(" ");

  Euclid::PhzModeling::ModelDatasetGrid model_grid(parameter_space,std::move(m_sed_map),std::move(m_reddening_curve_map),m_reddening_function,m_redshift_function);

  auto iterator = model_grid.begin();

  //check the 0th element
  auto expected_iterator=sed1.begin();
  for(auto actual_pair:(*iterator)){
    BOOST_CHECK(Elements::isEqual(expected_iterator->first,actual_pair.first));
    BOOST_CHECK(Elements::isEqual(expected_iterator->second,actual_pair.second));
    ++expected_iterator;
  }

  // check the 1th element (which is redshifted only by z=0.1 using the dummy redshift functor)
  ++iterator;
  expected_iterator=sed1.begin();
  for(auto actual_pair:(*iterator)){
    BOOST_CHECK(Elements::isEqual((1.1)*expected_iterator->first,actual_pair.first));
    BOOST_CHECK(Elements::isEqual(expected_iterator->second,actual_pair.second));
    ++expected_iterator;
  }
}


BOOST_AUTO_TEST_SUITE_END ()
