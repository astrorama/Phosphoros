#ifndef PHZQTUI_FILTERMAPPINGITEMDELEGATE_H
#define PHZQTUI_FILTERMAPPINGITEMDELEGATE_H

#include <QItemDelegate>
#include <set>
#include <string>

namespace Euclid {
namespace PhzQtUI {

class FilterMappingItemDelegate: public QItemDelegate
{
public:
  FilterMappingItemDelegate( std::set<std::string> columns,QObject * parent = 0);

  QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

  void setEditorData(QWidget * editor, const QModelIndex & index) const override;

  virtual void  setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override;

private:
  std::set<std::string> m_columns;
};

}
}

#endif // PHZQTUI_FILTERMAPPINGITEMDELEGATE_H
