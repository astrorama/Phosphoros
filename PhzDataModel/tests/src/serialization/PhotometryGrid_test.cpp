/**
 * @file tests/src/serialization/PhotometryGrid_test.cpp
 * @date Sep 24, 2014
 * @author Florian Dubath
 */

#include <string>
#include <set>
#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "PhzDataModel/serialization/PhotometryGrid.h"

struct PhzPhotometryGridName_Fixture {

  std::vector<double> zs{0.0,0.1};
  std::vector<double> ebvs{0.0,0.001};
  std::vector<Euclid::XYDataset::QualifiedName> reddeing_curves{{"reddeningCurves/Curve1"}};
  std::vector<Euclid::XYDataset::QualifiedName> seds{{"sed/Curve1"}};

  std::shared_ptr<std::vector<std::string>> filter_1 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2"});
  std::shared_ptr<std::vector<std::string>> filter_2 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter2","filter3"});
  std::shared_ptr<std::vector<std::string>> filter_3 = std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filtre1","filter3"});
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_1{{1.1,2.1},{3.1,4.1}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_2{{1.2,2.2},{3.2,4.2}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_3{{1.3,2.3},{3.3,4.3}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_4{{1.4,2.4},{3.4,4.4}};
  std::vector<Euclid::SourceCatalog::FluxErrorPair> values_5{{1.5,2.5},{3.5,4.5},{5.5,6.5}};

  Euclid::SourceCatalog::Photometry photometry_1{filter_1,values_1};
  Euclid::SourceCatalog::Photometry photometry_2{filter_1,values_2};
  Euclid::SourceCatalog::Photometry photometry_3{filter_1,values_3};
  Euclid::SourceCatalog::Photometry photometry_4{filter_1,values_4};
  Euclid::SourceCatalog::Photometry photometry_5{filter_2,values_5};
  Euclid::SourceCatalog::Photometry photometry_6{filter_3,values_1};

  PhzPhotometryGridName_Fixture(){

  }
};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometryGridSerialization_test)

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(serializationException_test, PhzPhotometryGridName_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the Exception throw by the serialization of the Photometry Grid");
  BOOST_TEST_MESSAGE(" ");

  std::stringstream stream;
  boost::archive::text_oarchive oa(stream);

  auto axes=Euclid::PhzDataModel::createAxesTuple({},ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid empty_grid{axes};
  Euclid::PhzDataModel::PhotometryGrid *grid_ptr=&empty_grid;
  // Get the empty grid exception
  BOOST_CHECK_THROW((oa << grid_ptr),Elements::Exception);

  //---------------------------------------------------------------

  axes=Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid{axes};

  original_grid(0,0,0,0)=photometry_1;
  original_grid(1,0,0,0)=photometry_2;
  original_grid(0,1,0,0)=photometry_3;
  original_grid(1,1,0,0)=photometry_5;

  std::stringstream stream2;
  boost::archive::text_oarchive oa2(stream2);
  grid_ptr=&original_grid;
  // Get the exception for not identical filter list (not the same number)
  BOOST_CHECK_THROW((oa2 << grid_ptr),Elements::Exception);
  //---------------------------------------------------------------

  Euclid::PhzDataModel::PhotometryGrid original_grid_2{axes};
  original_grid_2(0,0,0,0)=photometry_1;
  original_grid_2(1,0,0,0)=photometry_2;
  original_grid_2(0,1,0,0)=photometry_3;
  original_grid_2(1,1,0,0)=photometry_6;
  std::stringstream stream3;
  boost::archive::text_oarchive oa3(stream3);
  grid_ptr=&original_grid_2;
  // Get the exception for not identical filter list (not the same name)
  BOOST_CHECK_THROW((oa3 << grid_ptr),Elements::Exception);
}

BOOST_FIXTURE_TEST_CASE(serialization_test, PhzPhotometryGridName_Fixture) {
  BOOST_TEST_MESSAGE(" ");
  BOOST_TEST_MESSAGE("--> Testing the serialization of the Photometry Grid");
  BOOST_TEST_MESSAGE(" ");

  auto axes=Euclid::PhzDataModel::createAxesTuple(zs,ebvs,reddeing_curves,seds);
  Euclid::PhzDataModel::PhotometryGrid original_grid{axes};
  original_grid(0,0,0,0)=photometry_1;
  original_grid(1,0,0,0)=photometry_2;
  original_grid(0,1,0,0)=photometry_3;
  original_grid(1,1,0,0)=photometry_4;
  Euclid::PhzDataModel::PhotometryGrid *original_grid_ptr=&original_grid;
  std::stringstream stream;
  boost::archive::text_oarchive oa(stream);

  oa<<original_grid_ptr;

  boost::archive::text_iarchive ia(stream);

  Euclid::PhzDataModel::PhotometryGrid *retrived_grid_ptr;
  ia >> retrived_grid_ptr;
  std::unique_ptr<Euclid::PhzDataModel::PhotometryGrid> ptr(retrived_grid_ptr);

  //-------------------------------------------------------------------
  BOOST_CHECK_EQUAL(original_grid_ptr->size(),retrived_grid_ptr->size());
  // Check values
  for(int v_index=0;v_index==2;v_index++){
    for(int evb_index=0;evb_index==2;evb_index++){
      auto expected_photometry = original_grid(v_index,evb_index,0,0);
      auto actual_photometry = (*retrived_grid_ptr)(v_index,evb_index,0,0);

      auto expected_iterator = expected_photometry.begin();
      for(auto actual_iterator : actual_photometry){
        BOOST_CHECK(Elements::isEqual((*expected_iterator).flux,actual_iterator.flux));
        BOOST_CHECK(Elements::isEqual((*expected_iterator).error,actual_iterator.error));
        ++expected_iterator;
      }
    }
  }

  // check filter names
  for(int v_index=0;v_index==2;v_index++){
     for(int evb_index=0;evb_index==2;evb_index++){
       auto expected_photometry = original_grid(v_index,evb_index,0,0);
       auto actual_photometry = (*retrived_grid_ptr)(v_index,evb_index,0,0);
       auto expected_iterator = expected_photometry.begin();
       auto actual_iterator = actual_photometry.begin();
       do{
         BOOST_CHECK_EQUAL(expected_iterator.filterName(), actual_iterator.filterName());
         ++expected_iterator;
         ++actual_iterator;
       } while(expected_iterator != expected_photometry.end());
     }
   }

  // check that all the photometries use the same Filter vector
  auto ref_photometry = (*retrived_grid_ptr)(0,0,0,0);
  auto ref_iterator = ref_photometry.begin();
  const std::string* ref_address = &(ref_iterator.filterName());
  for(int v_index=0;v_index==2;v_index++){
    for(int evb_index=0;evb_index==2;evb_index++){
      auto actual_photometry = (*retrived_grid_ptr)(v_index,evb_index,0,0);
      auto actual_iterator = actual_photometry.begin();
      const std::string* actual_address = &(actual_iterator.filterName());
      BOOST_CHECK_EQUAL(ref_address, actual_address);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END ()

