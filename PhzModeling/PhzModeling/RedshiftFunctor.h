/**
 * @file PhzModeling/RedshiftFunctor.h
 * @date Sep 15, 2014
 * @author Florian Dubath
 */

#ifndef PHZMODELING_REDSHIFTFUNCTOR_H
#define	PHZMODELING_REDSHIFTFUNCTOR_H


#include "XYDataset/XYDataset.h"

namespace Euclid {
namespace PhzModeling {

/**
 * @class Euclid::PhzModeling::RedshiftFunctor
 * @brief
 * This functor is in charge of redshifting the SED
 * @details
 * This functor is redshifting a SED to a given value z of the redshift. The shift is applied to all the pair of the SED.
 * For a given z, the wavelength component of the SED is multiplied by the factor(1+z)
 * while the corresponding flux is multiplied by 1/(1+z)²
 */
class RedshiftFunctor {

public:
	/**
	    * @brief Default Constructor
	    */
	RedshiftFunctor() = default;

	/**
	    * @brief Move Constructor
		*/
	RedshiftFunctor(RedshiftFunctor&&) = default;

	/**
		* @brief Move operator
		*/
	RedshiftFunctor& operator=(RedshiftFunctor&&) = default;

	/**
		* @brief Destructor
		*/
	virtual ~RedshiftFunctor() = default;

	/**
		* @brief Function Call Operator
		* @details
		* Execute the redshifting of a SED
		*
	    * @param sed
		* A XYDataset representing the SED to be redshifted.
		*
		* @param z
		* The redshift to be applied as a double.
		*
		* @return
		* A XYDataset representing the redshifted SED.
		*/
	Euclid::XYDataset::XYDataset operator()(const Euclid::XYDataset::XYDataset& sed, double z) const;
};

} // end of namespace PhzModeling
} // end of namespace Euclid

#endif	/* PHZMODELING_REDSHIFTFUNCTOR_H */
