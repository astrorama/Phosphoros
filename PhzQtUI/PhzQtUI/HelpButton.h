/*
 * HelpButton.h
 *
 *  Created on: may 6, 2016
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_HELPBUTTON_H_
#define PHZQTUI_PHZQTUI_HELPBUTTON_H_

#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QToolBox>

namespace Euclid {
namespace PhzQtUI {


class HelpButton : public QPushButton {
  Q_OBJECT
public:
  explicit HelpButton( QWidget *parent=0) ;

  HelpButton(const QString &text, QWidget *parent=0) ;


  virtual ~HelpButton()=default;

private slots:
  void recievClicked();

private:
};

}
}


#endif /* PHZQTUI_PHZQTUI_HELPBUTTON_H_ */
