/*
 * GridButton.h
 *
 *  Created on: Sep 2, 2015
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_GRIDBUTTON_H_
#define PHZQTUI_PHZQTUI_GRIDBUTTON_H_

#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QToolBox>

namespace Euclid {
namespace PhzQtUI {


class GridButton : public QPushButton {
  Q_OBJECT
public:
  GridButton(int x, int y, const QString &text, QWidget *parent=0) ;


  virtual ~GridButton()=default;

  signals:
  void GridButtonClicked(int,int);

private slots:
  void recievClicked();

private:
  int m_x;
  int m_y;
};

}
}


#endif /* PHZQTUI_PHZQTUI_GRIDBUTTON_H_ */
