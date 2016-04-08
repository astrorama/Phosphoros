#ifndef PARAMETERRULE_H
#define PARAMETERRULE_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "Range.h"

namespace boost{
namespace program_options{
 class variable_value;
}
}

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


    const std::string getEbvRangeString() const;

    const std::string getRedshiftRangeString() const;



    const std::set<double>& getEbvValues() const;

    void setEbvValues(std::set<double> values);


    const std::set<double>& getRedshiftValues() const;

    void setRedshiftValues(std::set<double> values);


    /**
     * @brief get the E(B-V) Ranges.
     * @return the E(B-V) Ranges.
     */
    const std::vector<Range>& getEbvRanges() const;

    /**
     * @brief set the E(B-V) Ranges by moving in the provided ranges
     * @param ebv_ranges
     */
    void setEbvRanges(std::vector<Range> ebv_ranges);

    /**
     * @brief get the redshift Ranges.
     * @return the redshift Ranges.
     */
    const std::vector<Range>& getZRanges() const;

    /**
     * @brief set the redshift Ranges by moving in the provided ranges
     * @param z_ranges
     */
    void setZRanges(std::vector<Range> z_ranges);


    std::map<std::string, boost::program_options::variable_value> getConfigOptions(std::string region) const;


    std::string getRedshiftStringValueList() const;
    std::string getEbvStringValueList() const ;


    static std::set<double> parseValueList(const std::string& list);

private:
    std::string getAxisStringValue(std::vector<double> axis) const;
    std::string getStringValueList(const std::set<double>& list) const;
    std::string m_name;

    std::string m_sed_root_object;
    std::string m_reddening_root_object;

    std::vector<std::string> m_excluded_sed;
    std::vector<std::string> m_excluded_reddening;


    std::set<double> m_ebv_values{};
    std::set<double> m_redshift_values{};

    std::vector<Range> m_ebv_ranges{};
    std::vector<Range> m_redshift_ranges{};
};

}
}
#endif // PARAMETERRULE_H
