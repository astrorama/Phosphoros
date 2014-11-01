/**
 * @file tests/src/PhotometryGridCreator_test.cpp
 * @date Oct 17, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <map>
#include <boost/test/unit_test.hpp>


#include "PhzModeling/PhotometryGridCreator.h"

struct PhotometryGridCreator_Fixture {

  class DatasetProvider: public Euclid::XYDataset::XYDatasetProvider{
  public:
    DatasetProvider(DatasetProvider&&) = default;

    DatasetProvider& operator=(DatasetProvider&&) = default;

    DatasetProvider() {

    }

    DatasetProvider(std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset>&& storage):m_storage(std::move(storage)){}

    std::unique_ptr<Euclid::XYDataset::XYDataset> getDataset(const Euclid::XYDataset::QualifiedName& qualified_name){
      std::vector<std::pair<double, double>> copied_values {};
      try {
        for (auto& sed_pair : m_storage.at(qualified_name)) {
          copied_values.push_back( std::make_pair(sed_pair.first,sed_pair.second));
        }
      } catch (std::out_of_range&){
        return nullptr;
      }
      return std::unique_ptr<Euclid::XYDataset::XYDataset>{new Euclid::XYDataset::XYDataset(std::move(copied_values))};
    }

    std::vector<Euclid::XYDataset::QualifiedName> listContents(const std::string&){
      std::vector<Euclid::XYDataset::QualifiedName> content{};
      for (auto& pair : m_storage) {
        content.push_back(pair.first);
      }

      return std::move(content);
    }

  private:
    std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> m_storage{};
  };

  std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> sed_provider;
  std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> reddening_provider;
  std::unique_ptr<Euclid::XYDataset::XYDatasetProvider> filter_provider;


  PhotometryGridCreator_Fixture(){
    std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> seds{};

    std::vector<std::pair<double, double>> sed_1{};
    sed_1.push_back(std::make_pair(10000.,0.1));
    sed_1.push_back(std::make_pair(12000.,0.2));
    sed_1.push_back(std::make_pair(14000.,0.3));
    sed_1.push_back(std::make_pair(16000.,0.2));
    sed_1.push_back(std::make_pair(18000.,0.1));
    seds.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"sed/sed_1"},sed_1));

    std::vector<std::pair<double, double>> sed_2{};
    sed_2.push_back(std::make_pair(10000.,0.3));
    sed_2.push_back(std::make_pair(12000.,0.2));
    sed_2.push_back(std::make_pair(14000.,0.1));
    sed_2.push_back(std::make_pair(16000.,0.2));
    sed_2.push_back(std::make_pair(18000.,0.3));
    seds.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"sed/sed_2"},sed_2));

    std::vector<std::pair<double, double>> sed_3{};
    sed_3.push_back(std::make_pair(10000.,0.2));
    sed_3.push_back(std::make_pair(12000.,0.2));
    sed_3.push_back(std::make_pair(14000.,0.2));
    sed_3.push_back(std::make_pair(16000.,0.2));
    sed_3.push_back(std::make_pair(18000.,0.2));
    seds.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"sed/sed_3"},sed_3));

    sed_provider = std::unique_ptr<Euclid::XYDataset::XYDatasetProvider>{new DatasetProvider{std::move(seds)}};

    std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> reddenings{};
    std::vector<std::pair<double, double>> ext_1{};
    ext_1.push_back(std::make_pair(10000.,0.1));
    ext_1.push_back(std::make_pair(12000.,0.2));
    ext_1.push_back(std::make_pair(14000.,0.3));
    ext_1.push_back(std::make_pair(16000.,0.4));
    ext_1.push_back(std::make_pair(18000.,0.5));
    reddenings.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"extinction/curve_1"},ext_1));

    std::vector<std::pair<double, double>> ext_2{};
    ext_2.push_back(std::make_pair(10000.,0.5));
    ext_2.push_back(std::make_pair(12000.,0.4));
    ext_2.push_back(std::make_pair(14000.,0.3));
    ext_2.push_back(std::make_pair(16000.,0.2));
    ext_2.push_back(std::make_pair(18000.,0.1));
    reddenings.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"extinction/curve_2"},ext_2));

    reddening_provider = std::unique_ptr<Euclid::XYDataset::XYDatasetProvider>{new DatasetProvider{std::move(reddenings)}};

    std::map<Euclid::XYDataset::QualifiedName,Euclid::XYDataset::XYDataset> filters{};
    std::vector<std::pair<double, double>> filter_1{};
    filter_1.push_back(std::make_pair(10000.,1.));
    filter_1.push_back(std::make_pair(12000.,1.));
    filter_1.push_back(std::make_pair(14000.,0.5));
    filters.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filter/filter_1"},filter_1));

    std::vector<std::pair<double, double>> filter_2{};
    filter_2.push_back(std::make_pair(14000.,0.5));
    filter_2.push_back(std::make_pair(16000.,1.));
    filter_2.push_back(std::make_pair(18000.,1.));
    filters.insert(std::make_pair(Euclid::XYDataset::QualifiedName{"filter/filter_2"},filter_2));

    filter_provider = std::unique_ptr<Euclid::XYDataset::XYDatasetProvider>{new DatasetProvider{std::move(filters)}};

  }

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometryGridCreator_test)

BOOST_FIXTURE_TEST_CASE(Constructor_test, PhotometryGridCreator_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the constructor");
  BOOST_TEST_MESSAGE(" ");

   std::vector<double> zs{0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
   std::vector<double> ebvs{0.0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009,0.01};
   std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"extinction/curve_1"},{"extinction/curve_1"}};
   std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/sed_1"},{"sed/sed_2"},{"sed/sed_3"}};
  auto axes= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  std::vector<Euclid::XYDataset::QualifiedName> filter_name_list{Euclid::XYDataset::QualifiedName{"filter/filter_1"},Euclid::XYDataset::QualifiedName{"filter/filter_2"}};

  Euclid::PhzModeling::PhotometryGridCreator gridCreator {
    std::move(sed_provider),
    std::move(reddening_provider),
    std::move(filter_provider)
  };
}

BOOST_FIXTURE_TEST_CASE(throw_SED_test, PhotometryGridCreator_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the SED exception");
  BOOST_TEST_MESSAGE(" ");

   std::vector<double> zs{0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
   std::vector<double> ebvs{0.0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009,0.01};
   std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"extinction/curve_1"},{"extinction/curve_1"}};
   std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/sed_5"},{"sed/sed_2"},{"sed/sed_3"}};

  auto axes= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);

  std::vector<Euclid::XYDataset::QualifiedName> filter_name_list{Euclid::XYDataset::QualifiedName{"filter/filter_1"},Euclid::XYDataset::QualifiedName{"filter/filter_2"}};

  // throw because the SED is not into the provider
  BOOST_CHECK_THROW(Euclid::PhzModeling::PhotometryGridCreator(
    std::move(sed_provider),
    std::move(reddening_provider),
    std::move(filter_provider)
  ).createGrid(axes, filter_name_list), Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(throw_curve_test, PhotometryGridCreator_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the reddening curve exception");
  BOOST_TEST_MESSAGE(" ");

   std::vector<double> zs{0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
   std::vector<double> ebvs{0.0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009,0.01};
   std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"extinction/curve_3"},{"extinction/curve_1"}};
   std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/sed_1"},{"sed/sed_2"},{"sed/sed_3"}};

  auto axes= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);

  std::vector<Euclid::XYDataset::QualifiedName> filter_name_list{Euclid::XYDataset::QualifiedName{"filter/filter_1"},Euclid::XYDataset::QualifiedName{"filter/filter_2"}};

  // throw because the reddening curve is not into the provider
  BOOST_CHECK_THROW(Euclid::PhzModeling::PhotometryGridCreator(
    std::move(sed_provider),
    std::move(reddening_provider),
    std::move(filter_provider)
  ).createGrid(axes, filter_name_list), Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(throw_filter_test, PhotometryGridCreator_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the Filter exception");
  BOOST_TEST_MESSAGE(" ");

   std::vector<double> zs{0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
   std::vector<double> ebvs{0.0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009,0.01};
   std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"extinction/curve_1"},{"extinction/curve_1"}};
   std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/sed_1"},{"sed/sed_2"},{"sed/sed_3"}};

  auto axes= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);

  std::vector<Euclid::XYDataset::QualifiedName> filter_name_list{Euclid::XYDataset::QualifiedName{"filter/filter_5"},Euclid::XYDataset::QualifiedName{"filter/filter_2"}};

  // throw because the filter is not into the provider
  BOOST_CHECK_THROW(Euclid::PhzModeling::PhotometryGridCreator(
    std::move(sed_provider),
    std::move(reddening_provider),
    std::move(filter_provider)
  ).createGrid(axes, filter_name_list), Elements::Exception);
}


BOOST_FIXTURE_TEST_CASE(execution_test, PhotometryGridCreator_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the execution");
  BOOST_TEST_MESSAGE(" ");

   std::vector<double> zs{0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
   std::vector<double> ebvs{0.0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009,0.01};
   std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"extinction/curve_1"},{"extinction/curve_1"}};
   std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/sed_1"},{"sed/sed_2"},{"sed/sed_3"}};
  auto axes= Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  std::vector<Euclid::XYDataset::QualifiedName> filter_name_list{Euclid::XYDataset::QualifiedName{"filter/filter_1"},Euclid::XYDataset::QualifiedName{"filter/filter_2"}};

  Euclid::PhzModeling::PhotometryGridCreator gridCreator{
    std::move(sed_provider),
    std::move(reddening_provider),
    std::move(filter_provider)
  };
  double sum_filter_1=0.;
  auto photometry_grid = gridCreator.createGrid(axes, filter_name_list);
  for (auto photometry:photometry_grid){
    auto filter_value = photometry.find("filter/filter_1");
    BOOST_CHECK(filter_value->flux>=0.0);
    sum_filter_1+=filter_value->flux;
    filter_value = photometry.find("filter/filter_2");
    BOOST_CHECK(filter_value->flux>=0.0);
   }
  BOOST_CHECK(sum_filter_1>0.);
  }


BOOST_AUTO_TEST_SUITE_END ()
