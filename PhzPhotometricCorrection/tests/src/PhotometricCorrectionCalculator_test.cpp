/** 
 * @file PhotometricCorrectionCalculator_test.cpp
 * @date January 20, 2015
 * @author Nikolaos Apostolakos
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "EnableGMock.h"
#include "PhzPhotometricCorrection/PhotometricCorrectionCalculator.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::PhzPhotometricCorrection;
using namespace SourceCatalog;
using namespace std::placeholders;
using namespace testing;

class FindBestFitModelsMock {
public:
  virtual ~FindBestFitModelsMock() = default;
  MOCK_METHOD3(FunctorCall, map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>(
                  const Catalog& calibration_catalog,
                  const PhzDataModel::PhotometryGrid& model_photometric_grid,
                  const PhzDataModel::PhotometricCorrectionMap& photometric_correction));
  void expectFunctorCall(const Catalog& expected_catalog,
                         const PhzDataModel::PhotometryGrid& expected_phot_grid,
                         const PhzDataModel::PhotometricCorrectionMap& expected_phot_corr,
                         const map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>& result) {
    EXPECT_CALL(*this, FunctorCall(
        Truly([&expected_catalog](const Catalog& catalog) { // First argument check
          BOOST_CHECK_EQUAL(catalog.size(), expected_catalog.size());
          for (auto exp_iter=expected_catalog.begin(), iter=catalog.begin(); iter!=catalog.end(); ++exp_iter, ++iter) {
            BOOST_CHECK_EQUAL(iter->getId(), exp_iter->getId());
          }
          return true;
        }),
        Truly([&expected_phot_grid](const PhzDataModel::PhotometryGrid& phot_grid) { // Second argument check
          BOOST_CHECK_EQUAL(phot_grid.size(), expected_phot_grid.size());
          for (auto iter=phot_grid.begin(), exp_iter=expected_phot_grid.begin(); iter!=phot_grid.end(); ++iter, ++exp_iter) {
            BOOST_CHECK(iter.axisValue<0>() == exp_iter.axisValue<0>());
            BOOST_CHECK(iter.axisValue<1>() == exp_iter.axisValue<1>());
            BOOST_CHECK(iter.axisValue<2>() == exp_iter.axisValue<2>());
            BOOST_CHECK(iter.axisValue<3>() == exp_iter.axisValue<3>());
          }
          return true;
        }),
        Truly([&expected_phot_corr](const PhzDataModel::PhotometricCorrectionMap& phot_corr) { // Third argument check
          BOOST_CHECK_EQUAL(phot_corr.size(), expected_phot_corr.size());
          for (auto& pair : phot_corr) {
            auto exp = expected_phot_corr.find(pair.first);
            BOOST_CHECK(exp != expected_phot_corr.end());
            BOOST_CHECK_EQUAL(pair.second, exp->second);
          }
          return true;
        }))
    ).WillOnce(Return(result));
  }
};

class CalculateScaleFactorsMapMock {
public:
  virtual ~CalculateScaleFactorsMapMock() = default;
  MOCK_METHOD3(FunctorCall, map<int64_t, double>(
                  Catalog::const_iterator source_begin,
                  Catalog::const_iterator source_end,
                  const map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>& model_phot_map));
  void expectFunctorCall(Catalog::const_iterator expected_source_begin,
                         Catalog::const_iterator expected_source_end,
                         const map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>& expected_model_phot_map,
                         const map<int64_t, double> result) {
    EXPECT_CALL(*this, FunctorCall(_, _,
        Truly([&expected_model_phot_map](const map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>& model_phot_map) {
          BOOST_CHECK_EQUAL(model_phot_map.size(), expected_model_phot_map.size());
          for (auto iter=model_phot_map.begin(), exp_iter=expected_model_phot_map.begin(); iter!=model_phot_map.end(); ++iter, ++exp_iter) {
            BOOST_CHECK_EQUAL(iter->first, exp_iter->first);
          }
          return true;
        }))).With(Args<0, 1>(
        Truly([expected_source_begin, expected_source_end](tuple<Catalog::const_iterator, Catalog::const_iterator> args) {
          BOOST_CHECK_EQUAL(get<1>(args)-get<0>(args), expected_source_end-expected_source_begin);
          for (auto iter=get<0>(args), exp_iter=expected_source_begin; iter!=get<1>(args); ++iter, ++exp_iter) {
            BOOST_CHECK_EQUAL(iter->getId(), exp_iter->getId());
          }
          return true;
        }))).WillOnce(Return(result));
  }
};

class CalculatePhotometricCorrectionMock {
public:
  virtual ~CalculatePhotometricCorrectionMock() = default;
  MOCK_METHOD4(FunctorCall, PhzDataModel::PhotometricCorrectionMap(
                  Catalog::const_iterator source_begin,
                  Catalog::const_iterator source_end,
                  const map<int64_t, double>& scale_factor_map,
                  const map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>& model_phot_map));
  void expectFunctorCall(Catalog::const_iterator expected_source_begin,
                         Catalog::const_iterator expected_source_end,
                         const map<int64_t, double>& expected_scale_factor_map,
                         const map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>& expected_model_phot_map,
                         const PhzDataModel::PhotometricCorrectionMap& result) {
    EXPECT_CALL(*this, FunctorCall(_, _,
        Truly([&expected_scale_factor_map](const map<int64_t, double>& scale_factor_map) {
          BOOST_CHECK_EQUAL(scale_factor_map.size(), expected_scale_factor_map.size());
          for (auto iter=scale_factor_map.begin(), exp_iter=expected_scale_factor_map.begin(); iter!=scale_factor_map.end(); ++iter, ++exp_iter) {
            BOOST_CHECK_EQUAL(iter->first, exp_iter->first);
          }
          return true;
        }),
        Truly([&expected_model_phot_map](const map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>& model_phot_map) {
          BOOST_CHECK_EQUAL(model_phot_map.size(), expected_model_phot_map.size());
          for (auto iter=model_phot_map.begin(), exp_iter=expected_model_phot_map.begin(); iter!=model_phot_map.end(); ++iter, ++exp_iter) {
            BOOST_CHECK_EQUAL(iter->first, exp_iter->first);
          }
          return true;
        }))).With(Args<0, 1>(
        Truly([expected_source_begin, expected_source_end](tuple<Catalog::const_iterator, Catalog::const_iterator> args) {
          BOOST_CHECK_EQUAL(get<1>(args)-get<0>(args), expected_source_end-expected_source_begin);
          for (auto iter=get<0>(args), exp_iter=expected_source_begin; iter!=get<1>(args); ++iter, ++exp_iter) {
            BOOST_CHECK_EQUAL(iter->getId(), exp_iter->getId());
          }
          return true;
        }))).WillOnce(Return(result));
  }
};

class StopCriteriaMock {
public:
  virtual ~StopCriteriaMock() = default;
  MOCK_METHOD1(FunctorCall, bool(const PhzDataModel::PhotometricCorrectionMap& phot_corr));
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (PhotometricCorrectionCalculator_test)

//-----------------------------------------------------------------------------
// Successful functor call
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(FunctorCallSuccess) {
  
  // Given
  const Catalog catalog {{
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
  }};
  const PhzDataModel::PhotometryGrid model_phot_grid {PhzDataModel::createAxesTuple(
          {0.1, 0.2, 0.3}, // Z
          {0.4, 0.5, 0.6, 0.7}, // E(B-V)
          {{"Curve1"}, {"Curve2"}}, // Reddening Curves
          {{"SED1"}, {"SED2"}, {"SED3"}} // SEDs
  )};
  vector<PhzDataModel::PhotometricCorrectionMap> phot_corr_map_list {
    {{{"Filter1"}, 1.}, {{"Filter2"}, 1.}},
    {{{"Filter1"}, 2.}, {{"Filter2"}, 2.}},
    {{{"Filter1"}, 3.}, {{"Filter2"}, 3.}}
  };
  vector<std::map<int64_t, PhzDataModel::PhotometryGrid::const_iterator>> best_fit_map_list {
    {{1, model_phot_grid.begin()}},
    {{2, model_phot_grid.begin()}}
  };
  vector<std::map<int64_t, double>> scale_map_list {
    {{1, 0.}}, {{2, 0.}}
  };
  
  // Create the mocks
  FindBestFitModelsMock find_best_fit_models_mock;
  CalculateScaleFactorsMapMock calculate_scale_factors_map_mock;
  CalculatePhotometricCorrectionMock calculate_photometric_correction_mock;
  StopCriteriaMock stop_criteria_mock;
  
  // Program the mocks
  EXPECT_CALL(stop_criteria_mock, FunctorCall(_)).Times(2).WillRepeatedly(Return(false));
  EXPECT_CALL(stop_criteria_mock, FunctorCall(Truly([&phot_corr_map_list](const PhzDataModel::PhotometricCorrectionMap& phot_corr){
    auto& exp_phot_corr = phot_corr_map_list.back();
    if (phot_corr.size() != exp_phot_corr.size()) {
      return false;
    }
    for (auto& pair : phot_corr) {
      auto exp = exp_phot_corr.find(pair.first);
      if (exp == exp_phot_corr.end() || pair.second != exp->second) {
        return false;
      }
    }
    return true;
  }))).WillOnce(Return(true));
  InSequence in_sqauence;
  find_best_fit_models_mock.expectFunctorCall(catalog, model_phot_grid, phot_corr_map_list[0], best_fit_map_list[0]);
  calculate_scale_factors_map_mock.expectFunctorCall(catalog.begin(), catalog.end(), best_fit_map_list[0], scale_map_list[0]);
  calculate_photometric_correction_mock.expectFunctorCall(catalog.begin(), catalog.end(), scale_map_list[0], best_fit_map_list[0], phot_corr_map_list[1]);
  find_best_fit_models_mock.expectFunctorCall(catalog, model_phot_grid, phot_corr_map_list[1], best_fit_map_list[1]);
  calculate_scale_factors_map_mock.expectFunctorCall(catalog.begin(), catalog.end(), best_fit_map_list[1], scale_map_list[1]);
  calculate_photometric_correction_mock.expectFunctorCall(catalog.begin(), catalog.end(), scale_map_list[1], best_fit_map_list[1], phot_corr_map_list[2]);
  
  // When
  PhotometricCorrectionCalculator calculator {
      bind(&FindBestFitModelsMock::FunctorCall, & find_best_fit_models_mock, _1, _2, _3),
      bind(&CalculateScaleFactorsMapMock::FunctorCall, &calculate_scale_factors_map_mock, _1, _2, _3),
      bind(&CalculatePhotometricCorrectionMock::FunctorCall, &calculate_photometric_correction_mock, _1, _2, _3, _4)
  };
  auto result_phot_corr_map = calculator(catalog, model_phot_grid,
                  bind(&StopCriteriaMock::FunctorCall, &stop_criteria_mock, _1), {});
  
  // Then
  BOOST_CHECK_EQUAL(result_phot_corr_map.size(), 2);
  auto filter1 = result_phot_corr_map.find({"Filter1"});
  BOOST_CHECK(filter1 != result_phot_corr_map.end());
  BOOST_CHECK_EQUAL(filter1->second, 3.);
  auto filter2 = result_phot_corr_map.find({"Filter2"});
  BOOST_CHECK(filter2 != result_phot_corr_map.end());
  BOOST_CHECK_EQUAL(filter2->second, 3.);
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()