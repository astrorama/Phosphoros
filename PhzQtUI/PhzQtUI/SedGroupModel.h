#ifndef PHZQTUI_SEDGROUPMODEL_H
#define PHZQTUI_SEDGROUPMODEL_H

#include <vector>
#include <QStandardItemModel>

namespace Euclid {
namespace PhzQtUI {


/**
 * @brief The ModelSetTable class
 * A tableView to display the ModelSetModel.
 */
class SedGroupModel : public QStandardItemModel
{
  Q_OBJECT
public:
  explicit SedGroupModel(QObject *parent = 0);

  void load(std::vector<std::string> items);

  std::vector<std::string> getSeds() const;

  Qt::DropActions supportedDropActions() const override;
  QStringList mimeTypes() const override;
  QMimeData * mimeData(const QModelIndexList &indexes) const override;



  bool dropMimeData(const QMimeData *data,
      Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

private:
   void addItems(std::string item);
   std::vector<QModelIndex> getAllChildren(QModelIndex parent) const;

};

}
}
#endif // PHZQTUI_SEDGROUPMODEL_H
