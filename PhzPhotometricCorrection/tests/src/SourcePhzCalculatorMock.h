/*
 * SourcePhzCalculatorMock.h
 *
 *  Created on: Jan 16, 2015
 *      Author: fdubath
 */

#ifndef SOURCEPHZCALCULATOR_MOCK_H_
#define SOURCEPHZCALCULATOR_MOCK_H_

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "EnableGMock.h"

using namespace testing;

namespace Euclid {
class SourcePhzCalculatorMock{
private:

public:

  virtual ~SourcePhzCalculatorMock() = default;

  SourcePhzCalculatorMock(PhzDataModel::PhotometricCorrectionMap phot_corr_map,
     const PhzDataModel::PhotometryGrid& phot_grid):
       m_phot_corr_map{std::move(phot_corr_map)},
       m_phot_grid(phot_grid){
        expectFunctorCall();


      }

  MOCK_METHOD1(FunctorCall,
      PhzLikelihood::SourcePhzFunctor::result_type*(const SourceCatalog::Photometry& source_phot));

  PhzLikelihood::SourcePhzFunctor::result_type operator()(const SourceCatalog::Photometry& source_phot){
    std::unique_ptr< PhzLikelihood::SourcePhzFunctor::result_type> res(FunctorCall(source_phot));
    return std::move(*res);
  }

  void expectFunctorCall() {
      EXPECT_CALL(*this, FunctorCall(_)).WillOnce(Return(
          new PhzLikelihood::SourcePhzFunctor::result_type{
        m_phot_grid.begin(),
            PhzDataModel::Pdf1D{GridContainer::GridAxis<double>{"Axis",{}}}}));
  };



private:
  PhzDataModel::PhotometricCorrectionMap m_phot_corr_map;
  const PhzDataModel::PhotometryGrid& m_phot_grid;
};




}

#endif /* SOURCEPHZCALCULATOR_MOCK_H_ */
