#ifndef PARAMETERRULE_H
#define PARAMETERRULE_H

#include <string>
#include <vector>
#include <set>
#include "Range.h"

namespace Euclid {
namespace PhzQtUI {

/**
 * @class ParameterRule
 *
 * @brief
 *  Object storing the description of a ParameterRule.
 *
 */
class ParameterRule
{
public:
    /**
     * @brief ParameterRule
     */
    ParameterRule();

    /**
     * @brief Get the name of the ParameterRule
     * @return the name of the ParameterRule
     */
    std::string getName() const;

    /**
     * @brief Set the name of the ParameterRule
     * @param new_name the new name of the ParameterRule
     */
    void setName(std::string new_name);
    /**
     * @brief get Reddening Curves Number
     * @return the cardinality of selected Reddening Curves in the the ParameterRule
     */
    long  getRedCurveNumber() const;
    /**
     * @brief get SED Number
     * @return the cardinality of selected SED in the the ParameterRule
     */
    long  getSedNumber() const;

    /**
     * @brief get Model Number
     * @return the cardinality of models which can be build from the ParameterRule
     */
    long long getModelNumber() const;

    /**
     * @brief get the Sed Root Object
     * @param rootPath
     * @return the root object path after trimming the root path.
     */
    std::string getSedRootObject(std::string rootPath="") const;

    /**
     * @brief set the SedRootObject
     * @param sed_root_object
     */
    void setSedRootObject(std::string sed_root_object);

    /**
     * @brief get the Reddening Root Object
     * @param rootPath
     * @return the root object path after trimming the root path.
     */
    std::string getReddeningRootObject(std::string rootPath="") const;

    /**
     * @brief set the ReddeningRootObject
     * @param reddening_root_object
     */
    void setReddeningRootObject(std::string reddening_root_object);

    /**
     * @brief get the Excluded SED list.
     * @return the Excluded SED list.
     */
    const std::vector<std::string>& getExcludedSeds() const;

    /**
     * @brief set the Excluded SED list by moving in the provided list
     * @param excluded_sed
     */
    void setExcludedSeds( std::vector<std::string> excluded_sed);

    /**
     * @brief get the Excluded Reddening Curve list.
     * @return the Excluded Reddening Curve list.
     */
    const std::vector<std::string>& getExcludedReddenings() const;

    /**
     * @brief set the Excluded Reddening Curve list by moving in the provided list
     * @param excluded_reddening
     */
    void setExcludedReddenings( std::vector<std::string> excluded_reddening);


    const bool hasEbvRange() const;

    void setHasEbvRange(bool has_range);

    const std::set<double>& getEbvValues() const;

    void setEbvValues(std::set<double> values);


    const bool hasRedshiftRange() const;

    void setHasRedshiftRange(bool has_range);

    const std::set<double>& getRedshiftValues() const;

    void setRedshiftValues(std::set<double> values);


    /**
     * @brief get the E(B-V) Range.
     * @return the E(B-V) Range.
     */
    const Range& getEbvRange() const;

    /**
     * @brief set the E(B-V) Range by moving in the provided range
     * @param ebv_range
     */
    void setEbvRange(Range ebv_range);

    /**
     * @brief get the redshift Range.
     * @return the redshiftRange.
     */
    const Range& getZRange() const;

    /**
     * @brief set the redshift Range by moving in the provided range
     * @param z_range
     */
    void setZRange(Range z_range);

private:
    std::string m_name;
    std::string m_sed_root_object;
    std::string m_reddening_root_object;

    std::vector<std::string> m_excluded_sed;
    std::vector<std::string> m_excluded_reddening;

    bool m_has_ebv_range = true;
    bool m_has_redshift_range = true;

    std::set<double> m_ebv_value{0.};
    std::set<double> m_redshift_value{0.};

    Range m_ebv_range;
    Range m_redshift_range;
};

}
}
#endif // PARAMETERRULE_H
