/*
 * GridButton.h
 *
 *  Created on: Sep 2, 2015
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_MESSAGEBUTTON_H_
#define PHZQTUI_PHZQTUI_MESSAGEBUTTON_H_

#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QToolBox>

namespace Euclid {
namespace PhzQtUI {


class MessageButton : public QPushButton {
  Q_OBJECT
public:
  MessageButton(const QString& message, const QString &text, QWidget *parent=0) ;


  virtual ~MessageButton()=default;

  signals:
  void MessageButtonClicked(const QString&);

private slots:
  void recievClicked();

private:
  QString m_message;
};

}
}


#endif /* PHZQTUI_PHZQTUI_MESSAGEBUTTON_H_ */
