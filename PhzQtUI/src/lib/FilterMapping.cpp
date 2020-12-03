#include "PhzQtUI/FilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

FilterMapping::FilterMapping() {}


std::string FilterMapping::getFluxColumn() const {
    return m_flux_column;
}

void FilterMapping::setFluxColumn(std::string flux_column) {
    m_flux_column = flux_column;
}

std::string FilterMapping::getErrorColumn() const {
    return m_error_column;
}

void FilterMapping::setErrorColumn(std::string error_column) {
    m_error_column = error_column;
}

std::string FilterMapping::getFilterFile() const {
    return m_filter_trnsmission_file;
}

void FilterMapping::setFilterFile(std::string filter_trnsmission_file) {
    m_filter_trnsmission_file = filter_trnsmission_file;
}

double FilterMapping::getN() const {
  return m_n;
}

void FilterMapping::setN(double new_n) {
  m_n = new_n;
}

double FilterMapping::getAlpha() const {
  return m_alpha;
}
void FilterMapping::setAlpha(double new_alpha) {
  m_alpha = new_alpha;
}

double FilterMapping::getBeta() const {
  return m_beta;
}

void FilterMapping::setBeta(double new_beta) {
  m_beta = new_beta;
}

double FilterMapping::getGamma() const {
  return m_gamma;
}

void FilterMapping::setGamma(double new_gamma) {
  m_gamma = new_gamma;
}

}
}
