/**
 * @file BestFitFunctionMock.h
 * @date January 5, 2015
 * @author Florian Dubath
 */

#ifndef BESTFITFUNCTIONMOCK_H
#define	BESTFITFUNCTIONMOCK_H

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "EnableGMock.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "FluxErrorPair_boost.h"

using namespace testing;

namespace Euclid {

class BestFitFunctionMock {

private:

public:


  virtual ~BestFitFunctionMock() = default;

  typedef PhzDataModel::LikelihoodGrid::iterator likely_iter;

  MOCK_METHOD2(FunctorCall, likely_iter(likely_iter likelihood_begin, likely_iter likelihood_end));

  void expectFunctorCall() { //likely_iter likelihood_begin, likely_iter likelihood_end
    EXPECT_CALL(*this, FunctorCall(_, _)).WillOnce(ReturnArg<0>());
  }
};

}
 // end of namespace Euclid

#endif	/* BESTFITFUNCTIONMOCK_H */

