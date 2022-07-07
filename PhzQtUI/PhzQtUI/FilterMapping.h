#ifndef FILTERMAPPING_H
#define FILTERMAPPING_H
#include <string>

namespace Euclid {
namespace PhzQtUI {

/**
 * @class FilterMapping
 *
 * @brief
 *  Object storing the description of a FilterMapping.
 *
 */
class FilterMapping {
public:
  /**
   * @brief FilterMapping.
   */
  FilterMapping();

  /**
   * @brief Get the FluxColumn.
   * @return the current FluxColumn.
   */
  std::string getFluxColumn() const;

  /**
   * @brief Set the FluxColumn.
   * @param flux_column.
   */
  void setFluxColumn(std::string flux_column);

  /**
   * @brief Get the ErrorColumn.
   * @return the current ErrorColumn.
   */
  std::string getErrorColumn() const;

  /**
   * @brief Set the ErrorColumn.
   * @param error_column.
   */
  void setErrorColumn(std::string error_column);

  /**
   * @brief Get the Filter Shift Column.
   * @return the current Filter Shift Column.
   */
  std::string getShiftColumn() const;

  /**
   * @brief Set the Filter Shift Column.
   * @param shift_column.
   */
  void setShiftColumn(std::string shift_column);

  /**
   * @brief Get the Filter File Name.
   * @return the current Filter File Name.
   */
  std::string getFilterFile() const;
  /**
   * @brief Set the Filter File Name.
   * @param filter_trnsmission_file
   */
  void setFilterFile(std::string filter_trnsmission_file);

  double getN() const;

  void setN(double new_n);

  double getAlpha() const;

  void setAlpha(double new_alpha);

  double getBeta() const;

  void setBeta(double new_beta);

  double getGamma() const;

  void setGamma(double new_gamma);

  bool getFromMag() const;

  void setFromMag(bool new_from_mag);

private:
  std::string m_flux_column;
  std::string m_error_column;
  std::string m_shift_column;
  std::string m_filter_trnsmission_file;
  double      m_n        = 3;
  double      m_alpha    = 1.0;
  double      m_beta     = 0.0;
  double      m_gamma    = 0.0;
  bool        m_from_mag = false;
};

}  // namespace PhzQtUI
}  // namespace Euclid
#endif  // FILTERMAPPING_H
