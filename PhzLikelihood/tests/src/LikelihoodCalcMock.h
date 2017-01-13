/** 
 * @file LikelihoodCalcMock.h
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef LIKELIHOODCALCMOCK_H
#define	LIKELIHOODCALCMOCK_H

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "EnableGMock.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "FluxErrorPair_boost.h"

using namespace testing;

namespace Euclid {

class LikelihoodCalcMock {
  
private:
  
public:
  
  virtual ~LikelihoodCalcMock() = default;
  
  typedef SourceCatalog::Photometry::const_iterator phot_iter;
  
  MOCK_METHOD4(FunctorCall, double(phot_iter source_begin, phot_iter source_end, phot_iter model_begin, double scale));
  
  void expectFunctorCall(const SourceCatalog::Photometry& source, const SourceCatalog::Photometry& model, double scale, double result) {
    EXPECT_CALL(*this, FunctorCall(_, _, _, scale)).With(AllOf(
        Args<0,1>(Truly([source](std::tuple<phot_iter,phot_iter> args) {
          BOOST_CHECK_EQUAL_COLLECTIONS(source.begin(), source.end(), std::get<0>(args), std::get<1>(args));
          return true;
        })),
        Args<2>(Truly([model](std::tuple<phot_iter> args) {
          BOOST_CHECK_EQUAL_COLLECTIONS(model.begin(), model.end(), std::get<0>(args), model.end());
          return true;
        }))
    )).WillOnce(Return(result));
  }
};

} // end of namespace Euclid

#endif	/* LIKELIHOODCALCMOCK_H */

