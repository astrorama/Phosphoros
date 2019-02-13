/*
 * GridButton.cpp
 *
 *  Created on: Sep 2, 2015
 *      Author: fdubath
 */

#include "PhzQtUI/MessageButton.h"

namespace Euclid {
namespace PhzQtUI {


MessageButton::MessageButton(const QString& message, const QString &text, QWidget *parent)
  : QPushButton(text,parent), m_message{message} {
    QObject::connect( this, SIGNAL(clicked()), this,SLOT(recievClicked()));
  }

  void MessageButton::recievClicked() {
    this->MessageButtonClicked(m_message);
  }



}
}
