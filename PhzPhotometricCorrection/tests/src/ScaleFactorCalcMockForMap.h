/**
 * @file ScaleFactorCalcMockForMap.h
 * @date Jan 14, 2015
 * @author Florian Dubath
 */

#ifndef SCALEFACTORCALCMOCKFORMAP_H
#define	SCALEFACTORCALCMOCKFORMAP_H

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "EnableGMock.h"
#include "SourceCatalog/SourceAttributes/Photometry.h"

using namespace testing;

namespace Euclid {

class ScaleFactorCalcMockForMap {

private:

public:


  virtual ~ScaleFactorCalcMockForMap() = default;

  typedef SourceCatalog::Photometry::const_iterator phot_iter;

  MOCK_METHOD3(FCall, double(phot_iter source_begin, phot_iter source_end, phot_iter model_begin));

};

} // end of namespace Euclid

#endif	/* SCALEFACTORCALCMOCKFORMAP_H */

