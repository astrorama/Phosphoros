#include "PhzQtUI/FilterMappingItemDelegate.h"
#include <QComboBox>

namespace Euclid {
namespace PhzQtUI {

FilterMappingItemDelegate::FilterMappingItemDelegate( std::set<std::string> columns, QObject * parent):QItemDelegate(parent),m_columns{columns}{

}

void  FilterMappingItemDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const{
  if (editor==0){
    return;
  }

  auto combo = static_cast<QComboBox*>(editor);
  auto value = combo->currentText();
  model->setData(index,value);

}

void FilterMappingItemDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const{
  if (editor==0){
      return;
    }

    auto combo = static_cast<QComboBox*>(editor);

    auto current_text = index.data().toString().toStdString();

    combo->addItem("");
    bool found=false;
    int idx=1;
    for(auto item : m_columns){
       if (item.compare(current_text)==0){
         found=true;
         combo->setCurrentIndex(idx);
       }

        ++idx;
     }

     if (!found){
       combo->setItemText(0,QString::fromStdString(current_text));
     }
}

QWidget * FilterMappingItemDelegate::createEditor(QWidget * parent , const QStyleOptionViewItem & , const QModelIndex & index) const{
  if (index.column()==0){
    return 0;
  }

  editionStarting();
  auto combo = new QComboBox(parent);
  combo->setEditable(true);

  combo->addItem("");
  for(auto item : m_columns){
     combo->addItem(QString::fromStdString(item));
   }

   return combo;
}



}
}
