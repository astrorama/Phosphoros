/** 
 * @file DefaultStopCriteria_test.cpp
 * @date January 20, 2015
 * @author Nikolaos Apostolakos
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "ElementsKernel/Exception.h"
#include "PhzPhotometricCorrection/DefaultStopCriteria.h"

using namespace Euclid;
using namespace Euclid::PhzPhotometricCorrection;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (DefaultStopCriteria_test)

//-----------------------------------------------------------------------------
// Check that the constructor throws exception for negative tolerance
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(negativeTolerance) {
  
  // When
  BOOST_CHECK_THROW(DefaultStopCriteria(10, -1.), Elements::Exception);
  
}

//-----------------------------------------------------------------------------
// Check that the the loop stops when we reach max_iter
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(maxIterationsStop) {
  
  // Given
  DefaultStopCriteria checker {5, 0.0};
  
  // When
  int counter = 0;
  while (!checker({{XYDataset::QualifiedName("Filter1"), counter},
                   {XYDataset::QualifiedName("Filter2"), counter}})) {
    ++counter;
  }
  
  // Then
  BOOST_CHECK_EQUAL(counter, 5);
  
}

//-----------------------------------------------------------------------------
// Check that the the loop stops when we have correct accuracy
//-----------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(toleranceStop) {
  
  // Given
  DefaultStopCriteria checker {10, 0.1};
  
  // When
  int counter = 0;
  bool stop = false;
  while (!stop) {
    ++counter;
    PhzDataModel::PhotometricCorrectionMap map {
      {XYDataset::QualifiedName("Filter1"), 1./counter},
      {XYDataset::QualifiedName("Filter2"), 1./counter}
    };
    stop = checker(map);
  }
  
  // Then
  BOOST_CHECK_EQUAL(counter, 4);
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()