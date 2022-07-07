/*
 * GridButton.h
 *
 *  Created on: Sep 2, 2015
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_MESSAGEBUTTON_H_
#define PHZQTUI_PHZQTUI_MESSAGEBUTTON_H_

#include <QPushButton>
#include <QString>
#include <QToolBox>
#include <QWidget>

namespace Euclid {
namespace PhzQtUI {

class MessageButton : public QPushButton {
  Q_OBJECT
public:
  MessageButton(const QString& message, const QString& text, QWidget* parent = 0);

  virtual ~MessageButton() = default;

signals:
  void MessageButtonClicked(const QString&);

private slots:
  void recievClicked();

private:
  QString m_message;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif /* PHZQTUI_PHZQTUI_MESSAGEBUTTON_H_ */
