/*
 * ScalableImageLabel.h
 *
 *  Created on: Apr 29, 2015
 *      Author: fdubath
 */


#ifndef SCALABLEIMAGELABEL_H_
#define SCALABLEIMAGELABEL_H_

#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>

namespace Euclid {
namespace PhzQtUI {

/**
 * @class ScalableImageLabel
 * @brief Widget displaying an image and adjusting its size to its parent
 */
class ScalableImageLabel : public QWidget
{
    Q_OBJECT

public:
    explicit ScalableImageLabel(QWidget *parent = 0);

    const QPixmap* pixmap() const;

    void setTopMargin(int margin);

    void setPixmap(const QPixmap&);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QPixmap pix;
    int m_top_margin=0;
};

}
}
#endif /* SCALABLEIMAGELABEL_H_ */
