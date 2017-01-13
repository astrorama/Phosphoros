/** 
 * @file LikelihoodAlgorithm_test.cpp
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#include <boost/test/unit_test.hpp>
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzLikelihood/LikelihoodAlgorithm.h"
#include "ScaleFactorCalcMock.h"
#include "LikelihoodCalcMock.h"

using namespace Euclid;
using namespace std::placeholders;

class LikelihoodAlgorithmFixture {
  
public:
  
  std::shared_ptr<std::vector<std::string>> source_filters {new std::vector<std::string> {
    "Filter1", "Filter2", "Filter3"
  }};
  std::vector<SourceCatalog::FluxErrorPair> source_fluxes {
    {1.,1.}, {2.,2.}, {3.,3.}
  };
  SourceCatalog::Photometry source_phot {source_filters, source_fluxes};
  
  int model_no = 5;
  std::shared_ptr<std::vector<std::string>> model_filters {new std::vector<std::string> {
    "Filter1", "Filter2", "Filter3"
  }};
  std::vector<SourceCatalog::Photometry> model_phot_list = makeModelPhotList(model_no, model_filters);
  
private:
  
  std::vector<SourceCatalog::Photometry> makeModelPhotList(int model_no,
                                std::shared_ptr<std::vector<std::string>> model_filters) {
    std::vector<SourceCatalog::Photometry> result {};
    for (int i=0; i<model_no; ++i) {
      std::vector<SourceCatalog::FluxErrorPair> model_fluxes;
      model_fluxes.emplace_back(i*3,i*3);
      model_fluxes.emplace_back(i*3+1,i*3+1);
      model_fluxes.emplace_back(i*3+2,i*3+2);
      result.emplace_back(model_filters, model_fluxes);
    }
    return result;
  }
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LikelihoodAlgorithm_test)

//-----------------------------------------------------------------------------
// Check that the algorithm works with the same filter order between source and
// models.
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(SameFilterOrder, LikelihoodAlgorithmFixture) {
  
  // Given
  ScaleFactorCalcMock scale_factor_calc_mock;
  LikelihoodCalcMock likelihood_calc_mock;
  std::vector<double> likelihood_list (model_no);
  
  // Expect
  InSequence in_sequence;
  for (int i=0; i<model_no; ++i) {
    scale_factor_calc_mock.expectFunctorCall(source_phot, model_phot_list[i], i);
    likelihood_calc_mock.expectFunctorCall(source_phot, model_phot_list[i], i, i);
  }
  
  // When
  PhzLikelihood::LikelihoodAlgorithm likelihood_algo {
            std::bind(&ScaleFactorCalcMock::FunctorCall, &scale_factor_calc_mock, _1, _2, _3),
            std::bind(&LikelihoodCalcMock::FunctorCall, &likelihood_calc_mock, _1, _2, _3, _4)};
  likelihood_algo(source_phot, model_phot_list.begin(), model_phot_list.end(), likelihood_list.begin());
  
  // Then
  for (int i=0; i<model_no; ++i) {
    BOOST_CHECK(likelihood_list[i] == i);
  }
  
}

//-----------------------------------------------------------------------------
// Check that the algorithm works with different filter order between source and
// models.
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(DifferentFilterOrder, LikelihoodAlgorithmFixture) {
  
  // Given
  std::shared_ptr<std::vector<std::string>> source_filters_unordered {new std::vector<std::string>};
  source_filters_unordered->push_back("Filter2");
  source_filters_unordered->push_back("Filter1");
  source_filters_unordered->push_back("Filter4");
  source_filters_unordered->push_back("Filter3");
  std::vector<SourceCatalog::FluxErrorPair> source_fluxes_unordered;
  source_fluxes_unordered.push_back({2.,2.});
  source_fluxes_unordered.push_back({1.,1.});
  source_fluxes_unordered.push_back({1.,4.});
  source_fluxes_unordered.push_back({3.,3.});
  SourceCatalog::Photometry source_phot_unordered {source_filters_unordered, source_fluxes_unordered};
  
  ScaleFactorCalcMock scale_factor_calc_mock;
  LikelihoodCalcMock likelihood_calc_mock;
  
  std::vector<double> likelihood_list (model_no);
  
  // Expect
  InSequence in_sequence;
  for (int i=0; i<model_no; ++i) {
    scale_factor_calc_mock.expectFunctorCall(source_phot, model_phot_list[i], i);
    likelihood_calc_mock.expectFunctorCall(source_phot, model_phot_list[i], i, i);
  }
  
  // When
  PhzLikelihood::LikelihoodAlgorithm likelihood_algo {
            std::bind(&ScaleFactorCalcMock::FunctorCall, &scale_factor_calc_mock, _1, _2, _3),
            std::bind(&LikelihoodCalcMock::FunctorCall, &likelihood_calc_mock, _1, _2, _3, _4)};
  likelihood_algo(source_phot_unordered, model_phot_list.begin(), model_phot_list.end(), likelihood_list.begin());
  
  // Then
  for (int i=0; i<model_no; ++i) {
    BOOST_CHECK(likelihood_list[i] == i);
  }
  
}

//-----------------------------------------------------------------------------
// Check that we get an exception if we miss a photometry
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(MissingSourcePhotometry, LikelihoodAlgorithmFixture) {
  
  // Given
  std::shared_ptr<std::vector<std::string>> source_filtersmissing {new std::vector<std::string>};
  source_filtersmissing->push_back("Filter1");
  source_filtersmissing->push_back("Filter3");
  std::vector<SourceCatalog::FluxErrorPair> source_fluxes;
  source_fluxes.push_back({1.,1.});
  source_fluxes.push_back({3.,3.});
  SourceCatalog::Photometry source_phot_missing {source_filtersmissing, source_fluxes};
  
  ScaleFactorCalcMock scale_factor_calc_mock;
  LikelihoodCalcMock likelihood_calc_mock;
  
  std::vector<double> likelihood_list (model_no);
  
  // When
  PhzLikelihood::LikelihoodAlgorithm likelihood_algo {
            std::bind(&ScaleFactorCalcMock::FunctorCall, &scale_factor_calc_mock, _1, _2, _3),
            std::bind(&LikelihoodCalcMock::FunctorCall, &likelihood_calc_mock, _1, _2, _3, _4)};
  
  // Then
  BOOST_CHECK_THROW(likelihood_algo(source_phot_missing, model_phot_list.begin(),
                                    model_phot_list.end(), likelihood_list.begin()),
                    Elements::Exception);
  
}
    
    
BOOST_AUTO_TEST_SUITE_END ()