/*
 * DoubleValidatorItemDelegate.h
 *
 *  Created on: Apr 7, 2016
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_DOUBLEVALIDATORITEMDELEGATE_H_
#define PHZQTUI_PHZQTUI_DOUBLEVALIDATORITEMDELEGATE_H_


#include <QItemDelegate>

namespace Euclid {
namespace PhzQtUI {
class DoubleValidatorItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit DoubleValidatorItemDelegate(QObject *parent = 0);

protected:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
    void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;

signals:

public slots:

};

}
}


#endif /* PHZQTUI_PHZQTUI_DOUBLEVALIDATORITEMDELEGATE_H_ */
