/** 
 * @file MaxMarginalizationFunctor_test.cpp
 * @date February 5, 2015
 * @author Nikolaos Apostolakos
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "PhzLikelihood/MaxMarginalizationFunctor.h"

using namespace Euclid;

struct MaxMarginalizationFunctor_Fixture {
  
  double tolerance = 1e-10;
  
  std::vector<double> z_values {0.0, 0.1, 0.2, 0.4, 0.5};
  std::vector<double> ebv_values {0.0, 0.01, 0.03, 0.04, 0.05};
  std::vector<XYDataset::QualifiedName> reddening_curves {{"Curve1"}, {"Curve2"}};
  std::vector<XYDataset::QualifiedName> seds {{"Sed1"}, {"Sed2"}};
  
  PhzDataModel::PdfGrid pdf_grid {PhzDataModel::createAxesTuple(z_values, ebv_values, reddening_curves, seds)};
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (MaxMarginalizationFunctor_test)

//-----------------------------------------------------------------------------
// Checks the returned 1D PDF of the Z axis
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(ZAxisMarginalization, MaxMarginalizationFunctor_Fixture) {
  
  // Given
  pdf_grid(1,1,0,1) = .1;
  pdf_grid(1,2,0,1) = .05;
  pdf_grid(2,1,0,1) = .15;
  pdf_grid(2,2,0,1) = .2;
  pdf_grid(3,1,0,1) = .4;
  pdf_grid(3,2,0,1) = .35;
  pdf_grid(4,1,0,1) = .44;
  pdf_grid(4,2,0,1) = .5;
  
  // When
  auto pdf = PhzLikelihood::MaxMarginalizationFunctor<PhzDataModel::ModelParameter::Z>()(pdf_grid);
  auto& axis = pdf.getAxis<0>();
  
  // Then
  // Check that the result axis is correct
  BOOST_CHECK_EQUAL(axis.name(), "Z");
  BOOST_CHECK_EQUAL_COLLECTIONS(axis.begin(), axis.end(), z_values.begin(), z_values.end());
  // Check that the calculated node values are correct
  std::vector<double> expected_pdf {0., 0.8, 1.6, 3.2, 4.};
  BOOST_CHECK_EQUAL(pdf.size(), expected_pdf.size());
  
  for (size_t i=0; i<pdf.size(); ++i) {
    BOOST_CHECK_CLOSE(pdf(i), expected_pdf[i], tolerance);
  }
  
}

//-----------------------------------------------------------------------------
// Checks the returned 1D PDF of the E(B-V) axis
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(EbvAxisMarginalization, MaxMarginalizationFunctor_Fixture) {
  
  // Given
  pdf_grid(1,1,0,1) = .01;
  pdf_grid(2,1,0,1) = .005;
  pdf_grid(1,2,0,1) = .015;
  pdf_grid(2,2,0,1) = .03;
  pdf_grid(1,3,0,1) = .04;
  pdf_grid(2,3,0,1) = .035;
  pdf_grid(1,4,0,1) = .044;
  pdf_grid(2,4,0,1) = .05;
  
  // When
  auto pdf = PhzLikelihood::MaxMarginalizationFunctor<PhzDataModel::ModelParameter::EBV>()(pdf_grid);
  auto& axis = pdf.getAxis<0>();
  
  
  // Then
  // Check that the result axis is correct
  BOOST_CHECK_EQUAL(axis.name(), "E(B-V)");
  BOOST_CHECK_EQUAL_COLLECTIONS(axis.begin(), axis.end(), ebv_values.begin(), ebv_values.end());
  // Check that the calculated node values are correct
//  std::vector<double> expected_pdf {0., 2., 4., 8., 10.};
  std::vector<double> expected_pdf {0., 8., 24., 32., 40.};
  BOOST_CHECK_EQUAL(pdf.size(), expected_pdf.size());
  for (size_t i=0; i<pdf.size(); ++i) {
    BOOST_CHECK_CLOSE(pdf(i), expected_pdf[i], tolerance);
  }
  
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END ()