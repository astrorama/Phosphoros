/** 
 * @file PhotometricCorrectionAlgorithm_test.cpp
 * @date January 14, 2015
 * @author Nikolaos Apostolakos
 */

#include <vector>
#include <map>
#include <memory>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "SourceCatalog/Source.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionAlgorithm.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::SourceCatalog;

struct PhotometricCorrectionAlgorithm_Fixture {
  
  double tolerance {1E-10};
      
  vector<Source> sources {
    {1, {shared_ptr<Attribute>{new Photometry{make_shared<vector<string>>(
        initializer_list<string>{"Filter1", "Filter2"}),
        vector<FluxErrorPair>{   {1.1, 0.1},  {1.2, 0.2}}}}}},
    {2, {shared_ptr<Attribute>{new Photometry{make_shared<vector<string>>(
        initializer_list<string>{"Filter1", "Filter2"}),
        vector<FluxErrorPair>{   {2.1, 0.1}, {2.2, 0.2}}}}}},
    {3, {shared_ptr<Attribute>{new Photometry{make_shared<vector<string>>(
        initializer_list<string>{"Filter1", "Filter2"}),
        vector<FluxErrorPair>{   {4.1, 0.1}, {4.2, 0.2}}}}}},
    {4, {shared_ptr<Attribute>{new Photometry{make_shared<vector<string>>(
        initializer_list<string>{"Filter1", "Filter2"}),
        vector<FluxErrorPair>{   {3.1, 0.1}, {3.2, 0.2}}}}}}
  };
  
  map<int64_t, double> scale_factors {
    {1, 1.5},
    {2, 2.5},
    {3, 4.5},
    {4, 3.5}
  };
  
  map<int64_t, shared_ptr<Photometry>> models {
    {1, shared_ptr<Photometry>{new Photometry{make_shared<vector<string>>(
        initializer_list<string>{"Filter1", "Filter2"}),
        vector<FluxErrorPair>{   {.11, 0.},  {.12, 0.}}}}},
    {2, shared_ptr<Photometry>{new Photometry{make_shared<vector<string>>(
        initializer_list<string>{"Filter1", "Filter2"}),
        vector<FluxErrorPair>{   {.21, 0.},  {.22, 0.}}}}},
    {3, shared_ptr<Photometry>{new Photometry{make_shared<vector<string>>(
        initializer_list<string>{"Filter1", "Filter2"}),
        vector<FluxErrorPair>{   {.41, 0.},  {.42, 0.}}}}},
    {4, shared_ptr<Photometry>{new Photometry{make_shared<vector<string>>(
        initializer_list<string>{"Filter1", "Filter2"}),
        vector<FluxErrorPair>{   {.31, 0.},  {.32, 0.}}}}}
  };
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometricCorrectionAlgorithm_test)

//-----------------------------------------------------------------------------
// Check that we get exception for no input sources
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(NoInputSources_test, PhotometricCorrectionAlgorithm_Fixture) {
  
  // Given
  vector<Source> empty_sources {};
  
  // When
  PhzPhotometricCorrection::PhotometricCorrectionAlgorithm algo {};
  
  // Then
  BOOST_CHECK_THROW(algo(empty_sources.begin(), empty_sources.end(), scale_factors, models), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that we get exception for source without photometry attribute
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(SourceWithoutPhotometry_test, PhotometricCorrectionAlgorithm_Fixture) {
  
  // Given
  sources.emplace_back(3, std::vector<std::shared_ptr<Attribute>>{});
  
  // When
  PhzPhotometricCorrection::PhotometricCorrectionAlgorithm algo {};
  
  // Then
  BOOST_CHECK_THROW(algo(sources.begin(), sources.end(), scale_factors, models), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that we get exception if a scale factor is missing
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(MissingScaleFactor_test, PhotometricCorrectionAlgorithm_Fixture) {
  
  // Given
  scale_factors.erase(2);
  
  // When
  PhzPhotometricCorrection::PhotometricCorrectionAlgorithm algo {};
  
  // Then
  BOOST_CHECK_THROW(algo(sources.begin(), sources.end(), scale_factors, models), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that we get exception if a model photometry is missing
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(MissingModelPhotometry_test, PhotometricCorrectionAlgorithm_Fixture) {
  
  // Given
  models.erase(2);
  
  // When
  PhzPhotometricCorrection::PhotometricCorrectionAlgorithm algo {};
  
  // Then
  BOOST_CHECK_THROW(algo(sources.begin(), sources.end(), scale_factors, models), Elements::Exception);

}

//-----------------------------------------------------------------------------
// Check that we get correct photometric correction values for even number
// of sources
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(EvenNumberOfSources_test, PhotometricCorrectionAlgorithm_Fixture) {
  
  // Given
  PhzPhotometricCorrection::PhotometricCorrectionAlgorithm algo {};
  
  // When
  auto phot_corr = algo(sources.begin(), sources.end(), scale_factors, models);
  auto pc1 = phot_corr.find(XYDataset::QualifiedName{"Filter1"});
  auto pc2 = phot_corr.find(XYDataset::QualifiedName{"Filter2"});
  
  // Then
  BOOST_CHECK_EQUAL(phot_corr.size(), 2);
  BOOST_CHECK(pc1 != phot_corr.end());
  BOOST_CHECK(pc2 != phot_corr.end());
  BOOST_CHECK_SMALL(pc1->second - 1, tolerance);
  BOOST_CHECK_SMALL(pc2->second - 1, tolerance);
}

//-----------------------------------------------------------------------------
// Check that we get correct photometric correction values for odd number
// of sources
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(OddNumberOfSources_test, PhotometricCorrectionAlgorithm_Fixture) {
  
  // Given
  sources.emplace_back(5, std::vector<std::shared_ptr<Attribute>>{
        shared_ptr<Attribute>{new Photometry{make_shared<vector<string>>(
                initializer_list<string>{"Filter1", "Filter2"}),
                vector<FluxErrorPair>{   {5.1, 0.1}, {5.2, 0.2}}}}
  });
  scale_factors[5] = 2.8;
  models[5] = shared_ptr<Photometry>{new Photometry{make_shared<vector<string>>(
        initializer_list<string>{"Filter1", "Filter2"}),
        vector<FluxErrorPair>{   {.51, 0.},  {.52, 0.}}}};
  PhzPhotometricCorrection::PhotometricCorrectionAlgorithm algo {};
  
  // When
  auto phot_corr = algo(sources.begin(), sources.end(), scale_factors, models);
  auto pc1 = phot_corr.find(XYDataset::QualifiedName{"Filter1"});
  auto pc2 = phot_corr.find(XYDataset::QualifiedName{"Filter2"});
  
  // Then
  BOOST_CHECK_EQUAL(phot_corr.size(), 2);
  BOOST_CHECK(pc1 != phot_corr.end());
  BOOST_CHECK(pc2 != phot_corr.end());
  BOOST_CHECK_SMALL(pc1->second - 1, tolerance);
  BOOST_CHECK_SMALL(pc2->second - 1, tolerance);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()