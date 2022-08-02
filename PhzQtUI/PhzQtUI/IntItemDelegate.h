#ifndef PHZQTUI_INTITEMDELEGATE_H
#define PHZQTUI_INTITEMDELEGATE_H

#include <QItemDelegate>
#include <set>
#include <string>

namespace Euclid {
namespace PhzQtUI {
/**
 * @class IntItemDelegate
 * @brief This QItemDelegate allow the edition in a grid using an editable
 * spinbox
 */
class IntItemDelegate : public QItemDelegate {
  Q_OBJECT
public:
  /**
   * @brief constructor
   * @param parent Parent object into which the edition take place.
   */
  IntItemDelegate(int min = 0, int max = 99, QObject* parent = 0);

  /**
   * @brief Create the editable textbox.
   */
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

  /**
   * @brief Get the current cell value, look if it is present in the column list
   * if so select the column, if not enter this value as the current text.
   */
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;

  /**
   * read the text from the editable dropdown and push it back to the cell value.
   */
  virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

signals:
  void editionStarting() const;

private:
  int m_min;
  int m_max;
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif  // PHZQTUI_NUMBERITEMDELEGATE_H
