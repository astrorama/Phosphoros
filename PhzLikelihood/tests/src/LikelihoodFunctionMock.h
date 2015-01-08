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

  void expectFunctorCall(const SourceCatalog::Photometry& source_photometry, phot_iter model_begin, phot_iter model_end) {
    EXPECT_CALL(*this, FunctorCall(_, _, _,_)).With(AllOf(
            Args<0>(Truly([this, source_photometry](std::tuple<const SourceCatalog::Photometry&> args) {

                      auto& recieved_photometry = std::get<0>(args);
                      BOOST_CHECK_EQUAL(source_photometry.size(),recieved_photometry.size());
                      auto expected_iter= source_photometry.begin();
                      for(auto& recieved:recieved_photometry) {
                        BOOST_CHECK(Elements::isEqual((*expected_iter).flux,recieved.flux));
                        ++expected_iter;
                      }
                      return true;
                    })),
            Args<1,2>(Truly([&model_begin,&model_end](std::tuple<phot_iter,phot_iter> args) {
                      auto& provided_iter = std::get<0>(args);
                      do
                      {
                        auto provided_photo_iter=(*provided_iter).begin();
                        for(auto& ref_photo_iter :(*model_begin)){
                          BOOST_CHECK(Elements::isEqual((*provided_photo_iter).flux,ref_photo_iter.flux));
                          ++provided_photo_iter;
                        }

                        ++model_begin;
                        ++provided_iter;
                      }
                      while (model_begin !=model_end);
                      return true;
                    } ))

        )).WillOnce(Return());
  }
};

}
 // end of namespace Euclid

#endif	/* LIKELIHOODFUNCTIONCMOCK_H */

