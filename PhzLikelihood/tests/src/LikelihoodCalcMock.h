/** 
 * @file LikelihoodCalcMock.h
 * @date December 1, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef LIKELIHOODCALCMOCK_H
#define	LIKELIHOODCALCMOCK_H

#include <boost/test/unit_test.hpp>
#include "EnableGMock.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

using namespace testing;

namespace Euclid {

class LikelihoodCalcMock {
  
private:
  
public:
  
  virtual ~LikelihoodCalcMock() = default;
  
  typedef SourceCatalog::Photometry::const_iterator phot_iter;
  
  MOCK_METHOD4(FunctorCall, double(phot_iter source_begin, phot_iter source_end, phot_iter model_begin, double scale));
  
  void expectFunctorCall(const SourceCatalog::Photometry& source, const SourceCatalog::Photometry& model, double scale, double result) {
    EXPECT_CALL(*this, FunctorCall(_, _, _, scale)).With(Args<0,1,2>(Truly([source, model](std::tuple<phot_iter,phot_iter,phot_iter> args){
      phot_iter exp_source = source.begin();
      phot_iter exp_source_end = source.end();
      phot_iter exp_model = model.begin();
      phot_iter source = std::get<0>(args);
      phot_iter source_end = std::get<1>(args);
      phot_iter model = std::get<2>(args);
      for(; exp_source!=exp_source_end && source!=source_end; ++exp_source, ++ source, ++exp_model, ++model) {
        if ((*exp_source).flux != (*source).flux || (*exp_source).error != (*source).error) {
          BOOST_ERROR("Expected source photometry (" << (*exp_source).flux << "," <<
                      (*exp_source).error << ") but got (" << (*source).flux <<
                      "," << (*source).error << ") (for filter " << source.filterName() << ")");
        }
        if ((*exp_model).flux != (*model).flux || (*exp_model).error != (*model).error) {
          BOOST_ERROR("Expected model photometry (" << (*exp_model).flux << "," <<
                      (*exp_model).error << ") but got (" << (*model).flux <<
                      "," << (*model).error << ") (for filter " << source.filterName() << ")");
        }
      }
      if (exp_source!=exp_source_end || source!=source_end) {
        BOOST_ERROR("Wrong source photometry size");
      }
      return true;
    }))).WillOnce(Return(result));
  }
};

} // end of namespace Euclid

#endif	/* LIKELIHOODCALCMOCK_H */

