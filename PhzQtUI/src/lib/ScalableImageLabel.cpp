/*
 * ScalableImageLabel.cpp
 *
 *  Created on: Apr 29, 2015
 *      Author: fdubath
 */

#include "PhzQtUI/ScalableImageLabel.h"
#include <QPainter>

namespace Euclid {
namespace PhzQtUI {

ScalableImageLabel::ScalableImageLabel(QWidget* parent) : QWidget(parent) {}

void ScalableImageLabel::paintEvent(QPaintEvent* event) {
  QWidget::paintEvent(event);

  if (pix.isNull())
    return;

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  QSize pixSize   = pix.size();
  auto  dest_size = event->rect().size();
  dest_size.setHeight(dest_size.height() - m_top_margin);

  pixSize.scale(dest_size, Qt::KeepAspectRatio);

  QPixmap scaledPix = pix.scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  auto    offset    = (dest_size.width() - pixSize.width()) / 2;
  painter.drawPixmap(QPoint(offset, m_top_margin), scaledPix);
}

const QPixmap* ScalableImageLabel::pixmap() const {
  return &pix;
}

void ScalableImageLabel::setPixmap(const QPixmap& pixmap) {
  pix = pixmap;
}

void ScalableImageLabel::setTopMargin(int margin) {
  m_top_margin = margin;
}

}  // namespace PhzQtUI
}  // namespace Euclid
