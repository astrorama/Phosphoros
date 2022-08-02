/*
 * GridButton.cpp
 *
 *  Created on: Sep 2, 2015
 *      Author: fdubath
 */

#include "PhzQtUI/GridButton.h"

namespace Euclid {
namespace PhzQtUI {

GridButton::GridButton(size_t x, size_t y, const QString& text, QWidget* parent)
    : QPushButton(text, parent), m_x{x}, m_y{y} {
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(recievClicked()));
}

void GridButton::recievClicked() {
  this->GridButtonClicked(m_x, m_y);
}

}  // namespace PhzQtUI
}  // namespace Euclid
