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
      PhzDataModel::PhotometryGrid phot_grid): m_phot_corr_map{std::move(phot_corr_map)}, m_phot_grid{std::move(phot_grid)}{
  }

  MOCK_METHOD1(FunctorCall, PhzDataModel::PhotometryGrid::const_iterator(const SourceCatalog::Photometry& source_phot));

private:
  PhzDataModel::PhotometricCorrectionMap m_phot_corr_map;
  PhzDataModel::PhotometryGrid m_phot_grid;
};




}

#endif /* SOURCEPHZCALCULATOR_MOCK_H_ */
