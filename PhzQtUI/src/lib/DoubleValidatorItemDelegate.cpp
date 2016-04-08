#include "PhzQtUI/DoubleValidatorItemDelegate.h"
#include <QLineEdit>
#include <QDoubleValidator>

namespace Euclid {
namespace PhzQtUI {

DoubleValidatorItemDelegate::DoubleValidatorItemDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *DoubleValidatorItemDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setValidator(new  QDoubleValidator(0,10000,4));
    return editor;
}


void DoubleValidatorItemDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    QString value =index.model()->data(index, Qt::EditRole).toString();
        QLineEdit *line = static_cast<QLineEdit*>(editor);
        line->setText(value);
}


void DoubleValidatorItemDelegate::setModelData(QWidget *editor,
                                QAbstractItemModel *model,
                                const QModelIndex &index) const
{
    QLineEdit *line = static_cast<QLineEdit*>(editor);
    QString value = line->text();
    model->setData(index, value);
}


void DoubleValidatorItemDelegate::updateEditorGeometry(QWidget *editor,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

}
}
