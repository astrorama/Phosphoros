/**
 * @file tests/src/FindBestFitModels_test.cpp
 * @date Jan 16, 2015
 * @author Florian Dubath
 */

#include <string>
#include <vector>
#include <set>
#include <functional>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "SourceCatalog/Catalog.h"
#include "PhzLikelihood/SourcePhzFunctor.h"
#include "PhzPhotometricCorrection/FindBestFitModels.h"
#include "SourcePhzCalculatorMock.h"


namespace Euclid {
namespace PhzPhotometricCorrection {

using namespace std::placeholders;
using namespace std;

struct FindBestFitModels_Fixture {
  vector<double> zs { 0.0, 0.1 };
   vector<double> ebvs { 0.0, 0.001 };
   vector<XYDataset::QualifiedName> reddeing_curves {
       { "reddeningCurves/Curve1" } };
   vector<XYDataset::QualifiedName> seds { { "sed/Curve1" } };

   shared_ptr<vector<string>> filters = shared_ptr<vector<string>>(
       new vector<string> { "filter_1", "filter_2", "filter_3" });

   SourceCatalog::Photometry photometry_1 { filters,
     vector<SourceCatalog::FluxErrorPair> { { 1.1, 0. }, { 1.2, 0. }, {1.3, 0. } } };
   SourceCatalog::Photometry photometry_2 { filters,
     vector<SourceCatalog::FluxErrorPair> { { 2.1, 0. }, { 2.2, 0. }, {2.3, 0. } } };
   SourceCatalog::Photometry photometry_3 { filters,
     vector<SourceCatalog::FluxErrorPair> { { 3.1, 0. }, { 3.2, 0. }, {3.3, 0. } } };
   SourceCatalog::Photometry photometry_4 { filters,
     vector<SourceCatalog::FluxErrorPair> { { 4.1, 0. }, { 4.2, 0. }, {4.3, 0. } } };
   SourceCatalog::Photometry photometry_source { filters,
     vector<SourceCatalog::FluxErrorPair> { { 0.1, 0. }, { 0.2, 0. }, {0.3, 0. } } };

   PhzDataModel::ModelAxesTuple axes = PhzDataModel::createAxesTuple(zs, ebvs,
       reddeing_curves, seds);
   PhzDataModel::PhotometryGrid photo_grid { axes };
   PhzDataModel::PhotometryGrid ref_photo_grid { axes };

   PhzDataModel::PhotometricCorrectionMap correctionMap { {
       XYDataset::QualifiedName { "filter_1" }, 1.0 }, {
       XYDataset::QualifiedName { "filter_2" }, 2.0 }, {
       XYDataset::QualifiedName { "filter_3" }, 3.0 } };

   SourceCatalog::Photometry photometry_corrected { filters, ComputeCorrection(
       vector<SourceCatalog::FluxErrorPair> { { 0.1, 0. }, { 0.2, 0. }, {0.3, 0. } } , correctionMap) };

   vector<SourceCatalog::Source> sources {
     {1, {shared_ptr<SourceCatalog::Attribute>{new SourceCatalog::Photometry{make_shared<vector<string>>(
         initializer_list<string>{"Filter1", "Filter2"}),
         vector<SourceCatalog::FluxErrorPair>{   {1.1, 0.1},  {1.2, 0.2}}}}}},
     {2, {shared_ptr<SourceCatalog::Attribute>{new SourceCatalog::Photometry{make_shared<vector<string>>(
         initializer_list<string>{"Filter1", "Filter2"}),
         vector<SourceCatalog::FluxErrorPair>{   {2.1, 0.1}, {2.2, 0.2}}}}}},
     {3, {shared_ptr<SourceCatalog::Attribute>{new SourceCatalog::Photometry{make_shared<vector<string>>(
         initializer_list<string>{"Filter1", "Filter2"}),
         vector<SourceCatalog::FluxErrorPair>{   {4.1, 0.1}, {4.2, 0.2}}}}}},
     {4, {shared_ptr<SourceCatalog::Attribute>{new SourceCatalog::Photometry{make_shared<vector<string>>(
         initializer_list<string>{"Filter1", "Filter2"}),
         vector<SourceCatalog::FluxErrorPair>{   {3.1, 0.1}, {3.2, 0.2}}}}}}
   };



   vector<SourceCatalog::FluxErrorPair> ComputeCorrection(
       const vector<SourceCatalog::FluxErrorPair>& values,
       const PhzDataModel::PhotometricCorrectionMap& corrMap) {
     vector<SourceCatalog::FluxErrorPair> result { };

     auto filter_iter = filters->begin();
     for (auto& flux_value : values) {
       result.push_back(
           SourceCatalog::FluxErrorPair { flux_value.flux
               * corrMap.at(XYDataset::QualifiedName {*filter_iter}), flux_value.error });

       ++filter_iter;
     }
     return move(result);
   }
   FindBestFitModels_Fixture(){
     photo_grid(0, 0, 0, 0) = photometry_1;
     photo_grid(1, 0, 0, 0) = photometry_2;
     photo_grid(0, 1, 0, 0) = photometry_3;
     photo_grid(1, 1, 0, 0) = photometry_4;

     ref_photo_grid(0, 0, 0, 0) = photometry_1;
     ref_photo_grid(1, 0, 0, 0) = photometry_2;
     ref_photo_grid(0, 1, 0, 0) = photometry_3;
     ref_photo_grid(1, 1, 0, 0) = photometry_4;
   }


};

/*

 * */

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FindBestFitModels_test)
BOOST_FIXTURE_TEST_CASE(Functional_call_test, FindBestFitModels_Fixture) {
  auto test_object = FindBestFitModels<PhzLikelihood::SourcePhzFunctor>();

  const auto& grid_ref=photo_grid;
  const auto& sources_ref=sources;
  const auto& map_ref=correctionMap;
  test_object(sources_ref,grid_ref,map_ref);
}


BOOST_AUTO_TEST_SUITE_END ()

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid
