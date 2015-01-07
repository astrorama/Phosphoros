/**
 * @file LikelihoodFunctionMock.h
 * @date January 5, 2015
 * @author Florian Dubath
 */

#ifndef LIKELIHOODFUNCTIONCMOCK_H
#define	LIKELIHOODFUNCTIONCMOCK_H

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/Real.h"
#include "EnableGMock.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"
#include "PhzDataModel/LikelihoodGrid.h"
#include "FluxErrorPair_boost.h"
#include "PhzDataModel/PhotometryGrid.h"

using namespace testing;

namespace Euclid {

class LikelihoodFunctionMock {

public:


  virtual ~LikelihoodFunctionMock() = default;

  typedef PhzDataModel::PhotometryGrid::const_iterator phot_iter;

  MOCK_METHOD4(FunctorCall, void(const SourceCatalog::Photometry& source_photometry,
          phot_iter model_begin,
          phot_iter model_end,
          PhzDataModel::LikelihoodGrid::iterator likelihood_begin)
  );

  void expectFunctorCall(const SourceCatalog::Photometry& source_photometry) {
    EXPECT_CALL(*this, FunctorCall(_, _, _,_)).With(
        Args<0>(Truly([this, source_photometry](std::tuple<const SourceCatalog::Photometry&> args) {

           auto& recieved_photometry = std::get<0>(args);
           BOOST_CHECK_EQUAL(source_photometry.size(),recieved_photometry.size());
           auto expected_iter= source_photometry.begin();
           for(auto& recieved:recieved_photometry){
             BOOST_CHECK(Elements::isEqual((*expected_iter).flux,recieved.flux));
             ++expected_iter;
           }
           return true;
        }))
    ).WillOnce(Return());
  }
};

}
 // end of namespace Euclid

#endif	/* LIKELIHOODFUNCTIONCMOCK_H */

