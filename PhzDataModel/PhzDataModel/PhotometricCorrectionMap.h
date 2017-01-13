/**
 * @file PhotometricCorrectionMap.h
 * @date December 2, 2014
 * @author Nikolaos Apostolakos
 */

#ifndef PHZDATAMODEL_PHOTOMETRICCORRECTIONMAP_H
#define	PHZDATAMODEL_PHOTOMETRICCORRECTIONMAP_H

#include <map>
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzDataModel {

typedef std::map<XYDataset::QualifiedName, double> PhotometricCorrectionMap;

/**
 * @brief Write the Correction Map to a stream.
 *
 * @details
 * The serialization is done by writing the map in a table
 * with 2 columns (Filter <string>,Correction<double>). Then the table is
 * rendered as an ASCII output. The Filter column contains the full (also with groups)
 * filter name. The output can be written in a text file and is human readable.
 *
 * @param out
 * A ostream to which the correction map will be written.
 *
 * @param corrections
 * A PhotometricCorrectionMap to be written on the stream.
 */
void writePhotometricCorrectionMap(std::ostream& out, const PhotometricCorrectionMap& corrections);

/**
 * @brief Read the Correction Map from a stream.
 *
 * @details
 * The stream must contain a ASCII representation of a Table
 * with 2 columns (Filter <string>,Correction<double>).
 * The Filter column contains the full (also with groups)
 * filter name.
 * 
 * Example file:
 * \code
 * #             Filter Correction
 * #             string     double
 *
 *  MER/Ynir_WFC3f105w   0.929515
 *   MER/Iext_ACSf775w    1.08226
 *  MER/Hnir_WFC3f160w   0.600265
 *   MER/Rext_ACSf606w    1.57643
 *  MER/Jnir_WFC3f125w   0.654886
 *  MER/Zext_ACSf850lp    1.02498
 *    MER/VIS_ACSf814w    1.13837
 * \endcode
 *
 * @param in
 * A istream encoding the table representing the correction map .
 *
 * @return
 * A PhotometricCorrectionMap read from the stream.
 *
 * @throws ElementsException
 *    If the table contained into the stream do not have columns (Filter,Correction).
 * @throws ElementsException
 *    If the column Filter is not of type string.
 * @throws ElementsException
 *    If the column Correction is not of type double.

 */
PhotometricCorrectionMap readPhotometricCorrectionMap(std::istream& in);

} // end of namespace PhzDataModel
} // end of namespace Euclid

#endif	/* PHZDATAMODEL_PHOTOMETRICCORRECTIONMAP_H */

