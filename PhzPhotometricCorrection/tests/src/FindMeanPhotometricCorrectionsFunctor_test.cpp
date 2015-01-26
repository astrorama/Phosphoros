/**
 * @file FindMeanPhotometricCorrectionsFunctor_test.cpp
 * @date January 26, 2015
 * @author Florian Dubath
 */

#include <vector>
#include <map>
#include <memory>
#include "ElementsKernel/Real.h"
#include "ElementsKernel/Exception.h"
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "SourceCatalog/Source.h"
#include "PhzPhotometricCorrection/FindMeanPhotometricCorrectionsFunctor.h"

using namespace std;
using namespace Euclid;
using namespace Euclid::SourceCatalog;

struct FindMeanPhotometricCorrectionsFunctor_Fixture {

  std::map<int64_t, PhzDataModel::PhotometricCorrectionMap> source_phot_corr_map
  {
    {1,{
          {XYDataset::QualifiedName{"Filter_1"},1},
          {XYDataset::QualifiedName{"Filter_2"},11}
       }
    },
    {2,{
              {XYDataset::QualifiedName{"Filter_1"},3},
              {XYDataset::QualifiedName{"Filter_2"},10}
       }
    },
    {3,{
              {XYDataset::QualifiedName{"Filter_1"},5},
              {XYDataset::QualifiedName{"Filter_2"},8}
        }
    },
    {4,{
              {XYDataset::QualifiedName{"Filter_1"},7},
              {XYDataset::QualifiedName{"Filter_2"},12}
       }
    },
    {5,{
              {XYDataset::QualifiedName{"Filter_1"},101},
              {XYDataset::QualifiedName{"Filter_2"},20}
        }
    }
  };

  std::vector<int64_t> sources {1,2,3,4,5};

};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (FindMeanPhotometricCorrectionsFunctor_test)

//-----------------------------------------------------------------------------
// Check the functor returns the median value also for unordered inputs
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(NoInputSources_test, FindMeanPhotometricCorrectionsFunctor_Fixture) {
  PhzPhotometricCorrection::FindMeanPhotometricCorrectionsFunctor functor{};
  auto result = functor(source_phot_corr_map,sources.begin(),sources.end());

  BOOST_CHECK(Elements::isEqual(23.4, result.at({"Filter_1"})));
  BOOST_CHECK(Elements::isEqual(12.2, result.at({"Filter_2"})));

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()
