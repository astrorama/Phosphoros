#include "PhzQtUI/NumberItemDelegate.h"
#include <QDoubleSpinBox>

namespace Euclid {
namespace PhzQtUI {

NumberItemDelegate::NumberItemDelegate(QObject * parent):QItemDelegate(parent) {

}

void  NumberItemDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const {
  if (editor == 0) {
    return;
  }

  auto spinBox = static_cast<QDoubleSpinBox*>(editor);
  auto value =  QString::number(spinBox->value());
  model->setData(index, value);

}

void NumberItemDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const {
  if (editor == 0) {
      return;
    }

    auto spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(index.data().toDouble());
}

QWidget * NumberItemDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem &, const QModelIndex & index) const {
  if (index.column() == 0) {
    return 0;
  }

  editionStarting();
  auto spinbox = new QDoubleSpinBox(parent);


   return spinbox;
}



}
}
