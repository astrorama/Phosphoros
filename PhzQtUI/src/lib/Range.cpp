#include "PhzQtUI/Range.h"
#include <QString>
#include <sstream>
#include <string>

namespace Euclid {
namespace PhzQtUI {

Range::Range() {
  m_min  = 0.;
  m_max  = 0;
  m_step = 0;
}

Range::Range(double min, double max, double step) : m_min(min), m_max(max), m_step(step) {}

std::string Range::getStringRepresentation() const {
  QString string = "[" + QString::number(m_min, 'g', 2) + " , " + QString::number(m_max, 'g', 2) + "] step " +
                   QString::number(m_step, 'g', 4);
  return string.toStdString();
}

std::string Range::getConfigStringRepresentation() const {
  std::stringstream stream;
  stream.imbue(std::locale("C"));
  stream << m_min << ' ' << m_max << ' ' << m_step;
  return stream.str();
}

double Range::getMin() const {
  return m_min;
}

void Range::setMin(double min) {
  m_min = min;
}

double Range::getMax() const {
  return m_max;
}

void Range::setMax(double max) {
  m_max = max;
}

double Range::getStep() const {
  return m_step;
}

void Range::setStep(double step) {
  m_step = step;
}

}  // namespace PhzQtUI
}  // namespace Euclid
