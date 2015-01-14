/**
 * @file tests/src/CalculateScaleFactorMap_test.cpp
 * @date Jan 14, 2015
 * @author Florian Dubath
 */

#include <string>
#include <vector>
#include <set>
#include <boost/test/unit_test.hpp>

#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include "SourceCatalog/Catalog.h"
#include "PhzPhotometricCorrection/CalculateScaleFactorMap.h"


namespace Euclid {
namespace PhzPhotometricCorrection {


struct CalculateScaleFactorMap_Fixture {

  std::shared_ptr<std::vector<std::string>> filters;

  std::vector<SourceCatalog::Source> m_catalog{};

  std::map<int64_t, std::unique_ptr<SourceCatalog::Photometry>> model_map{};


  CalculateScaleFactorMap_Fixture(){
    // define the filters list
    filters= std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>{"filter_1","filter_2","filter_3"});

    int64_t source_1_id =1;
    // build the catalog
    auto value_photometry_1 = std::vector<SourceCatalog::FluxErrorPair>{{1.1,0.1},{1.2,0.2},{1.3,0.3}};
    auto attribute_1 = std::vector<std::shared_ptr<SourceCatalog::Attribute>> ();

    attribute_1.push_back(
        std::shared_ptr<SourceCatalog::Attribute>(
            new SourceCatalog::Photometry(filters,std::move(value_photometry_1))
    ));

    auto source_1 = SourceCatalog::Source(source_1_id,std::move(attribute_1));
    m_catalog.push_back(std::move(source_1));

    int64_t source_2_id =2;
    auto value_photometry_2 = std::vector<SourceCatalog::FluxErrorPair>{{2.1,0.1},{2.2,0.2},{2.3,0.3}};
    auto attribute_2 = std::vector<std::shared_ptr<SourceCatalog::Attribute>> ();

      attribute_2.push_back(
          std::shared_ptr<SourceCatalog::Attribute>(
              new SourceCatalog::Photometry(filters,std::move(value_photometry_2))
      ));
      auto source_2 = SourceCatalog::Source(source_2_id,std::move(attribute_2));
      m_catalog.push_back(std::move(source_1));

    auto model_photo_Value_1 = std::vector<SourceCatalog::FluxErrorPair>{{10.1,0.1},{10.2,0.2},{10.3,0.3}};
    auto model_ptr_1 = std::unique_ptr<SourceCatalog::Photometry>(new SourceCatalog::Photometry(filters,std::move(model_photo_Value_1)));
    auto model_photo_Value_2 = std::vector<SourceCatalog::FluxErrorPair>{{20.1,0.1},{20.2,0.2},{20.3,0.3}};
    auto model_ptr_2 = std::unique_ptr<SourceCatalog::Photometry>(new SourceCatalog::Photometry(filters,std::move(model_photo_Value_2)));

    model_map.emplace(source_1_id,std::move(model_ptr_1));
    model_map.emplace(source_2_id,std::move(model_ptr_2));

    }

};


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (CalculateScaleFactorMap_test)
BOOST_FIXTURE_TEST_CASE(Functional_call_test, CalculateScaleFactorMap_Fixture) {

  auto functor = CalculateScaleFactorMap();
  auto scale = functor(m_catalog.begin(),m_catalog.end(),model_map);



}


BOOST_AUTO_TEST_SUITE_END ()

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid
