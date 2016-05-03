#ifndef PHZQTUI_FILTERMAPPINGITEMDELEGATE_H
#define PHZQTUI_FILTERMAPPINGITEMDELEGATE_H

#include <QItemDelegate>
#include <set>
#include <string>

namespace Euclid {
namespace PhzQtUI {
/**
 * @class FilterMappingItemDelegate
 * @brief This QItemDelegate allow the edition in a grid using an editable
 * dropdown filled with the provided list of columns
 */
class FilterMappingItemDelegate: public QItemDelegate {
  Q_OBJECT
public:
  /**
   * @brief constructor
   * @param columns List of the available column to be proposed in the dropdown.
   * @param parent Parent object into which the edition take place.
   */
  FilterMappingItemDelegate(std::set<std::string> columns,
      QObject * parent = 0);

  /**
   * @brief Create the editable dropdown and fill the options with the columns.
   */
  QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option,
      const QModelIndex & index) const override;

  /**
   * @brief Get the current cell value, look if it is present in the column list
   * if so select the column, if not enter this value as the current text.
   */
  void setEditorData(QWidget * editor, const QModelIndex & index) const
      override;

  /**
   * read the text from the editable dropdown and push it back to the cell value.
   */
  virtual void setModelData(QWidget * editor, QAbstractItemModel * model,
      const QModelIndex & index) const override;

  signals:
  void editionStarting() const;

private:
  std::set<std::string> m_columns;
};

}
}

#endif // PHZQTUI_FILTERMAPPINGITEMDELEGATE_H
