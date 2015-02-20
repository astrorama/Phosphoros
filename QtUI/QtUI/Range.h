#ifndef RANGE_H
#define RANGE_H

#include <string>

namespace PhosphorosUiDm {
/**
 * @class Range
 *
 * @brief
 *  Object storing the description of a Range.
 *
 */
class Range
{
public:
    /**
     * @brief create a Range with min,max and step values set to 0
     */
    Range();

    /**
     * @brief Create a Range with the provided values
     * @param min
     * @param max
     * @param step
     */
    Range(double min,double max, double step);

    /**
     * @brief Get a String Representation of the Range
     * @return a string to be displayed to the user and containing the range information.
     */
    std::string getStringRepresentation() const;

    /**
     * @brief Get the range lower bound.
     * @return the range current lower bound.
     */
    double getMin() const;

    /**
     * @brief Set the range lower bound.
     * @param min.
     */
    void setMin(double min);

    /**
     * @brief Get the range upper bound.
     * @return  the range current upper bound.
     */
    double getMax() const;

    /**
     * @brief Set the range upper bound.
     * @param max
     */
    void setMax(double max);

    /**
     * @brief Get the range step.
     * @return the current range step.
     */
    double getStep() const;

    /**
     * @brief set the range step.
     * @param step
     */
    void setStep(double step);

private:

    double m_min;
    double m_max;
    double m_step;
};

}

#endif // RANGE_H
