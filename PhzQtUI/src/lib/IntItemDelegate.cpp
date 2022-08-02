#include "PhzQtUI/IntItemDelegate.h"
#include <QSpinBox>

namespace Euclid {
namespace PhzQtUI {

IntItemDelegate::IntItemDelegate(int min, int max, QObject* parent) : QItemDelegate(parent), m_min{min}, m_max{max} {}

void IntItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
  if (editor == 0) {
    return;
  }

  auto spinBox = static_cast<QSpinBox*>(editor);
  auto value   = spinBox->value();
  model->setData(index, value);
}

void IntItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
  if (editor == 0) {
    return;
  }

  auto spinBox = static_cast<QSpinBox*>(editor);
  spinBox->setValue(index.data().toInt());
}

QWidget* IntItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const {
  if (index.column() == 0) {
    return 0;
  }

  editionStarting();
  auto spinbox = new QSpinBox(parent);
  spinbox->setMinimum(m_min);
  spinbox->setMaximum(m_max);

  return spinbox;
}

}  // namespace PhzQtUI
}  // namespace Euclid
