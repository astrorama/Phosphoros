/** 
 * @file SumMarginalizationFunctor_test.cpp
 * @date January 12, 2015
 * @author Nikolaos Apostolakos
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "PhzLikelihood/SumMarginalizationFunctor.h"

using namespace Euclid;

struct SumMarginalizationFunctor_Fixture {
  
  double tolerance = 1e-10;
  
  std::vector<double> z_values {0.0, 0.1, 0.2, 0.4, 0.5};
  std::vector<double> ebv_values {0.0, 0.01, 0.03, 0.04, 0.05};
  std::vector<XYDataset::QualifiedName> reddening_curves {{"Curve1"}, {"Curve2"}};
  std::vector<XYDataset::QualifiedName> seds {{"Sed1"}, {"Sed2"}};
  
  PhzDataModel::PdfGrid pdf_grid {PhzDataModel::createAxesTuple(z_values, ebv_values, reddening_curves, seds)};
  
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE (ChiSquareFunctor_test)

//-----------------------------------------------------------------------------
// Checks the returned 1D PDF of the Z axis
//-----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(ZAxisMarginalization, SumMarginalizationFunctor_Fixture) {
  
  // Given
  for (double z : z_values) {
    for (auto iter = pdf_grid.begin().fixAxisByValue<PhzDataModel::ModelParameter::Z>(z); iter != pdf_grid.end(); ++iter) {
      (*iter) = z;
    }
  }
  
  // When
  auto pdf = PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::Z>()(pdf_grid);
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
BOOST_FIXTURE_TEST_CASE(EbvAxisMarginalization, SumMarginalizationFunctor_Fixture) {
  
  // Given
  for (double ebv : ebv_values) {
    for (auto iter = pdf_grid.begin().fixAxisByValue<PhzDataModel::ModelParameter::EBV>(ebv); iter != pdf_grid.end(); ++iter) {
      (*iter) = ebv;
    }
  }
  
  // When
  auto pdf = PhzLikelihood::SumMarginalizationFunctor<PhzDataModel::ModelParameter::EBV>()(pdf_grid);
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