/*
 * HelpButton.cpp
 *
 *  Created on: MAy 6, 2016
 *      Author: fdubath
 */

#include "PhzQtUI/HelpButton.h"
#include <QDesktopServices>
#include <QUrl>

namespace Euclid {
namespace PhzQtUI {

HelpButton::HelpButton(QWidget *parent) : QPushButton(parent){
    QObject::connect( this, SIGNAL(clicked()), this,SLOT(recievClicked()));
  }

HelpButton::HelpButton( const QString &text, QWidget *parent) : QPushButton(text,parent){
    QObject::connect( this, SIGNAL(clicked()), this,SLOT(recievClicked()));
  }

  void HelpButton::recievClicked(){
    QDesktopServices::openUrl(QUrl(whatsThis()) );
  }



}
}
