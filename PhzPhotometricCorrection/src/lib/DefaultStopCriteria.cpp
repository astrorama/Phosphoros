/** 
 * @file DefaultStopCriteria.cpp
 * @date January 20, 2015
 * @author Nikolaos Apostolakos
 */

#include <cmath>
#include "ElementsKernel/Exception.h"
#include "PhzPhotometricCorrection/DefaultStopCriteria.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

DefaultStopCriteria::DefaultStopCriteria(int max_iter, double tolerance)
      : m_max_iter(max_iter), m_tolerance(tolerance) {
  if (m_tolerance < 0) {
    throw Elements::Exception() << "Negative tolerance " << m_tolerance;
  }
}

bool DefaultStopCriteria::operator ()(const PhzDataModel::PhotometricCorrectionMap& phot_corr) {
  ++m_current_iter;
  // If we already have reached the maximum iterations we stop
  if (m_current_iter == m_max_iter) {
    return true;
  }
  bool result = true;
  // If it is the first time we return always false
  if (m_previous.empty()) {
    result = false;
  } else {
    // Check if any of the corrections is bigger than the tolerance
    auto iter = phot_corr.begin();
    auto prev_iter = m_previous.begin();
    for (; iter != phot_corr.end(); ++iter, ++prev_iter) {
      if (std::abs(iter->second - prev_iter->second) > m_tolerance) {
        result = false;
      }
    }
  }
  m_previous = phot_corr;
  return result;
}

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid