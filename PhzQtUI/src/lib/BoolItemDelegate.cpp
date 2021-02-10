#include "PhzQtUI/BoolItemDelegate.h"
#include <QComboBox>

namespace Euclid {
namespace PhzQtUI {

BoolItemDelegate::BoolItemDelegate( QObject * parent):QItemDelegate(parent) {

}

void  BoolItemDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const{
  if (editor==0){
    return;
  }

  auto combo = static_cast<QComboBox*>(editor);
  auto value = QString::number(combo->currentIndex());
  model->setData(index, value);

}

void BoolItemDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const{
  if (editor==0){
      return;
    }

    auto combo = static_cast<QComboBox*>(editor);

    if(index.data().toString().toStdString() =="1") {
      combo->setCurrentIndex(1);
    }



}

QWidget * BoolItemDelegate::createEditor(QWidget * parent , const QStyleOptionViewItem & , const QModelIndex & index) const{
  if (index.column()==0){
    return 0;
  }

  editionStarting();
  auto combo = new QComboBox(parent);
  combo->setEditable(true);

  combo->addItem("False");
  combo->addItem("True");
  return combo;
}



}
}
