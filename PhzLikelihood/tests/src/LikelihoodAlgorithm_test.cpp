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

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (LikelihoodAlgorithm_test)

//-----------------------------------------------------------------------------
// Check that the algorithm works with the same filter order between source and
// models.
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SameFilterOrder) {
  
  // Given
  std::shared_ptr<std::vector<std::string>> source_filters {new std::vector<std::string>};
  source_filters->push_back("Filter1");
  source_filters->push_back("Filter2");
  source_filters->push_back("Filter3");
  std::vector<SourceCatalog::FluxErrorPair> source_fluxes;
  source_fluxes.push_back({1.,1.});
  source_fluxes.push_back({2.,2.});
  source_fluxes.push_back({3.,3.});
  SourceCatalog::Photometry source_phot {source_filters, source_fluxes};
  
  std::shared_ptr<std::vector<std::string>> model_filters {new std::vector<std::string>};
  model_filters->push_back("Filter1");
  model_filters->push_back("Filter2");
  model_filters->push_back("Filter3");
  std::vector<SourceCatalog::Photometry> model_phot_list {};
  int model_no = 5;
  for (int i=0; i<model_no; ++i) {
    std::vector<SourceCatalog::FluxErrorPair> model_fluxes;
    model_fluxes.emplace_back(i*3,i*3);
    model_fluxes.emplace_back(i*3+1,i*3+1);
    model_fluxes.emplace_back(i*3+2,i*3+2);
    model_phot_list.emplace_back(model_filters, model_fluxes);
  }
  
  ScaleFactorCalcMock scale_factor_calc_mock;
  LikelihoodCalcMock likelihood_calc_mock;
  
  std::vector<double> likelihood_list (model_no);
  
  // Expect
  for (int i=0; i<model_no; ++i) {
    scale_factor_calc_mock.expect_functor_call(source_phot.begin(), source_phot.end(), model_phot_list[i].begin(), i);
    likelihood_calc_mock.expect_functor_call(source_phot.begin(), source_phot.end(), model_phot_list[i].begin(), i, i);
  }
  
  // When
  PhzLikelihood::LikelihoodAlgorithm likelihood_algo {std::move(scale_factor_calc_mock), std::move(likelihood_calc_mock)};
  likelihood_algo(source_phot, model_phot_list.begin(), model_phot_list.end(), likelihood_list.begin());
  
  // Then
  for (int i=0; i<model_no; ++i) {
// We have instruct the mock class to return the exact double representation. We
// want to do a very strict bitwise comparison and not a more relaxed real value
// representation comparison, to guarantee that the algorithm does not do any
// math on the result. The following pragma supress the related warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
    BOOST_CHECK(likelihood_list[i] == i);
#pragma GCC diagnostic pop
  }
  
}

//-----------------------------------------------------------------------------
// Check that the algorithm works with different filter order between source and
// models.
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DifferentFilterOrder) {
  
  // Given
  std::shared_ptr<std::vector<std::string>> source_filters {new std::vector<std::string>};
  source_filters->push_back("Filter2");
  source_filters->push_back("Filter1");
  source_filters->push_back("Filter4");
  source_filters->push_back("Filter3");
  std::vector<SourceCatalog::FluxErrorPair> source_fluxes;
  source_fluxes.push_back({2.,2.});
  source_fluxes.push_back({1.,1.});
  source_fluxes.push_back({1.,4.});
  source_fluxes.push_back({3.,3.});
  SourceCatalog::Photometry source_phot {source_filters, source_fluxes};
  
  std::shared_ptr<std::vector<std::string>> source_filters_ord {new std::vector<std::string>};
  source_filters_ord->push_back("Filter1");
  source_filters_ord->push_back("Filter2");
  source_filters_ord->push_back("Filter3");
  std::vector<SourceCatalog::FluxErrorPair> source_fluxes_ord;
  source_fluxes_ord.push_back({1.,1.});
  source_fluxes_ord.push_back({2.,2.});
  source_fluxes_ord.push_back({3.,3.});
  SourceCatalog::Photometry source_phot_ord {source_filters_ord, source_fluxes_ord};
  
  std::shared_ptr<std::vector<std::string>> model_filters {new std::vector<std::string>};
  model_filters->push_back("Filter1");
  model_filters->push_back("Filter2");
  model_filters->push_back("Filter3");
  std::vector<SourceCatalog::Photometry> model_phot_list {};
  int model_no = 5;
  for (int i=0; i<model_no; ++i) {
    std::vector<SourceCatalog::FluxErrorPair> model_fluxes;
    model_fluxes.emplace_back(i*3,i*3);
    model_fluxes.emplace_back(i*3+1,i*3+1);
    model_fluxes.emplace_back(i*3+2,i*3+2);
    model_phot_list.emplace_back(model_filters, model_fluxes);
  }
  
  ScaleFactorCalcMock scale_factor_calc_mock;
  LikelihoodCalcMock likelihood_calc_mock;
  
  std::vector<double> likelihood_list (model_no);
  
  // Expect
  for (int i=0; i<model_no; ++i) {
    scale_factor_calc_mock.expect_functor_call(source_phot_ord.begin(), source_phot_ord.end(), model_phot_list[i].begin(), i);
    likelihood_calc_mock.expect_functor_call(source_phot_ord.begin(), source_phot_ord.end(), model_phot_list[i].begin(), i, i);
  }
  
  // When
  PhzLikelihood::LikelihoodAlgorithm likelihood_algo {std::move(scale_factor_calc_mock), std::move(likelihood_calc_mock)};
  likelihood_algo(source_phot, model_phot_list.begin(), model_phot_list.end(), likelihood_list.begin());
  
  // Then
  for (int i=0; i<model_no; ++i) {
// We have instruct the mock class to return the exact double representation. We
// want to do a very strict bitwise comparison and not a more relaxed real value
// representation comparison, to guarantee that the algorithm does not do any
// math on the result. The following pragma supress the related warnings.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
    BOOST_CHECK(likelihood_list[i] == i);
#pragma GCC diagnostic pop
  }
  
}

//-----------------------------------------------------------------------------
// Check that we get an exception if we miss a photometry
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(MissingSourcePhotometry) {
  
  // Given
  std::shared_ptr<std::vector<std::string>> source_filters {new std::vector<std::string>};
  source_filters->push_back("Filter1");
  source_filters->push_back("Filter3");
  std::vector<SourceCatalog::FluxErrorPair> source_fluxes;
  source_fluxes.push_back({1.,1.});
  source_fluxes.push_back({3.,3.});
  SourceCatalog::Photometry source_phot {source_filters, source_fluxes};
  
  std::shared_ptr<std::vector<std::string>> model_filters {new std::vector<std::string>};
  model_filters->push_back("Filter1");
  model_filters->push_back("Filter2");
  model_filters->push_back("Filter3");
  std::vector<SourceCatalog::Photometry> model_phot_list {};
  int model_no = 5;
  for (int i=0; i<model_no; ++i) {
    std::vector<SourceCatalog::FluxErrorPair> model_fluxes;
    model_fluxes.emplace_back(i*3,i*3);
    model_fluxes.emplace_back(i*3+1,i*3+1);
    model_fluxes.emplace_back(i*3+2,i*3+2);
    model_phot_list.emplace_back(model_filters, model_fluxes);
  }
  
  ScaleFactorCalcMock scale_factor_calc_mock;
  LikelihoodCalcMock likelihood_calc_mock;
  
  std::vector<double> likelihood_list (model_no);
  
  // When
  PhzLikelihood::LikelihoodAlgorithm likelihood_algo {std::move(scale_factor_calc_mock), std::move(likelihood_calc_mock)};
  
  // Then
  BOOST_CHECK_THROW(likelihood_algo(source_phot, model_phot_list.begin(), model_phot_list.end(), likelihood_list.begin()), Elements::Exception);
  
}
    
    
BOOST_AUTO_TEST_SUITE_END ()