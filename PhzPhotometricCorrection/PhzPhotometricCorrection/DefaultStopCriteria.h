/** 
 * @file DefaultStopCriteria.h
 * @date January 20, 2015
 * @author Nikolaos Apostolakos
 */

#ifndef PHZPHOTOMETRICCORRECTION_DEFAULTSTOPCRITERIA_H
#define	PHZPHOTOMETRICCORRECTION_DEFAULTSTOPCRITERIA_H

#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzPhotometricCorrection {

/**
 * @class DefaultStopCriteria
 * 
 * @brief
 * Class which provides the default behavior for stopping the loop of the
 * photometric correction calculation
 * 
 * @details
 * This class will stop the loop after a specific number of iterations or when
 * all the photometric corrections have been modified less than a specific
 * tolerance, whicherver is first.
 */
class DefaultStopCriteria {
  
public:

  /**
   * @brief Constructs a new DefaultStopCriteria with the given parameters
   * @param max_iter the maximum number of iterations (negative number means unlimited)
   * @param tolerance the tolerance between two iterations for stopping the loop
   * @throws Elements::Exception
   *    if the tolerance is a negative number
   */
  DefaultStopCriteria(int max_iter, double tolerance);

  /**
   * @brief returns if the photometric correction calculation loop should be stoped
   * or not
   * @details
   * The loop should be stoped if the maximum number of iterations has been
   * reached or if all the corrections have changed less than the tolerance.
   * @param phot_corr The next photometric corrections map
   * @return true if the loop should be stoped, true if not
   */
  bool operator()(const PhzDataModel::PhotometricCorrectionMap& phot_corr);
  
private:
  
  int m_max_iter;
  double m_tolerance;
  int m_current_iter {0};
  PhzDataModel::PhotometricCorrectionMap m_previous {};
  
};

} // end of namespace PhzPhotometricCorrection
} // end of namespace Euclid

#endif	/* PHZPHOTOMETRICCORRECTION_DEFAULTSTOPCRITERIA_H */

