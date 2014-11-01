/**
 * @file PhzModeling/ExtinctionFunctor.h
 * @date May 28, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZMODELING_EXTINCTIONFUNCTOR_H
#define	PHZMODELING_EXTINCTIONFUNCTOR_H

#include <memory>
#include <cmath>
#include "MathUtils/function/Function.h"

namespace Euclid {

namespace XYDataset {
  class XYDataset;
}
namespace PhzModeling {
/**
 * @class Euclid::PhzModeling::ExtinctionFunctor
 * @brief
 * This functor is in charge of applying extinction on the SED
 * @details
 * This functor is applying extinction on a SED using a provided
 * Extinction Law and the E(B-V) value.
 *
 */
class ExtinctionFunctor {

public:

  /**
  * @brief Function Call Operator
  * @details
  * Apply extinction on the SED
  *
  * @param sed
  * A XYDataset representing the SED to be reddened.
  *
  * @param reddening_curve
  * The extinction law implementing the function \lambda->k(\lambda)
  * such that k(B)-k(V)=1.
  *
  * @param ebv
  * The color excess E(B-V)
  *
  * @return
  * A XYDataset representing the reddened SED.
  */
  Euclid::XYDataset::XYDataset operator()(const Euclid::XYDataset::XYDataset& sed,
      const Euclid::MathUtils::Function& reddening_curve, double ebv) const;
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_EXTINCTIONFUNCTOR_H */

