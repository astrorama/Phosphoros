
#include "PhzQtUI/SedGroupModel.h"
#include "XYDataset/QualifiedName.h"
#include <QMimeData>
#include <set>


namespace Euclid {
namespace PhzQtUI {

SedGroupModel::SedGroupModel(QObject *parent) : QStandardItemModel(parent){
  QStringList horzHeaders;  horzHeaders << "name" << "full" << "group";
  setHorizontalHeaderLabels( horzHeaders );
}

 void SedGroupModel::load(std::vector<std::string> items){
   // to be changed
   for (auto& item_name : items){
     addItems(item_name);
   }
 }


 void SedGroupModel::addItems(std::string new_item){
   XYDataset::QualifiedName item_q_name(new_item);
   if (item_q_name.groups().size()==0){
     auto item = new QStandardItem();
     auto item2 = new QStandardItem();
     auto item3 = new QStandardItem();
     item->setText(QString::fromStdString(item_q_name.datasetName()));
     item2->setText(QString::fromStdString(item_q_name.qualifiedName()));
     item3->setText(QString::number(0));
     item->setDropEnabled(false);
     this->appendRow({item,item2,item3});
     return;
   }

   // find (or build) the parent
   QStandardItem* parent;
   bool top_level=true;
   for (auto group : item_q_name.groups()){
     if (top_level){
       top_level=false;
       bool found=false;
       for (int i=0; i< rowCount();++i){
            std::string parent_name = data(index(i,1), Qt::DisplayRole).toString().toStdString();
            XYDataset::QualifiedName parent_q_name(parent_name);
            if (item_q_name.belongsInGroup(parent_q_name)){
              found=true;
              parent=this->item(i);
              break;
            }
        }

       if (!found){
         auto item = new QStandardItem();
         auto item2 = new QStandardItem();
         auto item3 = new QStandardItem();
         item->setText(QString::fromStdString(group));
         item2->setText(QString::fromStdString(group));
         item3->setText(QString::number(1));
         item->setDropEnabled(false);
         this->appendRow({item,item2,item3});
         parent = item;
       }
     } else {
       bool found = false;
       for (int i=0; i< parent->rowCount();++i){
         std::string parent_name = parent->child(i)->text().toStdString();
         XYDataset::QualifiedName parent_q_name(parent_name);
         if (item_q_name.belongsInGroup(parent_q_name)) {
          found = true;
          parent = parent->child(i);
          break;
        }
       }

       if (!found){
               auto item = new QStandardItem();
               auto item2 = new QStandardItem();
               auto item3 = new QStandardItem();
               item->setText(QString::fromStdString(group));
               item2->setText(QString::fromStdString(group));
               item->setDropEnabled(false);
               item3->setText(QString::number(1));
               parent->appendRow({item,item2,item3});
               parent = item;
             }
     }
   }

   // add to the parent
   auto item = new QStandardItem();
   auto item2 = new QStandardItem();
   auto item3 = new QStandardItem();
   item->setText(QString::fromStdString(item_q_name.datasetName()));
   item2->setText(QString::fromStdString(item_q_name.qualifiedName()));
   item3->setText(QString::number(0));
   item->setDropEnabled(false);
   parent->appendRow({item,item2,item3});

 }




 std::vector<std::string> SedGroupModel::getSeds() const{
   std::vector<QModelIndex> all_index{};
   for (int i=0; i<this->rowCount();++i){
     auto root_level_index = this->index(i,0);
     all_index.push_back(root_level_index);

     for(auto& child_index : getAllChildren(root_level_index)){
       all_index.push_back(child_index);
     }
   }

   std::vector<std::string> seds{};
   for(auto& index :  all_index){
     QString full_name = index.sibling(index.row(),1).data().toString();
     QString is_group = index.sibling(index.row(),2).data().toString();
     if (is_group.toInt()==0){
       seds.push_back(full_name.toStdString());
     }
   }


   return seds;
 }



 Qt::DropActions SedGroupModel::supportedDropActions() const{
   return  Qt::MoveAction;
 }
 bool SedGroupModel::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int, int, const QModelIndex &)
 {


  if (action != 2  || !data->hasFormat("application/x-sedgroupitem")) {
    return false;
  }

  QByteArray encodedData = data->data("application/x-sedgroupitem");
  QDataStream stream(&encodedData, QIODevice::ReadOnly);

  std::vector<std::string> new_items{};

  while (!stream.atEnd()) {
    QString name ;
    QString full_name;
    QString is_group;
    stream >> name >> full_name >> is_group;
      if (is_group.toInt()==0){
      new_items.push_back(full_name.toStdString());
    }
  }

  load(new_items);


  return true;
 }

 QStringList SedGroupModel::mimeTypes() const
 {
     QStringList types;
     types << "application/x-sedgroupitem";
     return types;
 }

 QMimeData *SedGroupModel::mimeData(const QModelIndexList &indexes) const
 {

  std::map<std::string,QModelIndex> item_to_push {};
  for (QModelIndex index: indexes) {
    if (index.isValid()) {
      std::string item_name = index.data().toString().toStdString();
      item_to_push[item_name]=index;
      for(auto child :getAllChildren(index)) {
       std::string child_name = child.data().toString().toStdString();
        item_to_push[child_name]=child;
     }
    }
  }

  QMimeData *mimeData = new QMimeData();
  QByteArray encodedData;
  QDataStream stream(&encodedData, QIODevice::WriteOnly);

  for(auto item_pair : item_to_push){
    auto index = item_pair.second;
    QString name = index.data().toString();
    QString full_name = index.sibling(index.row(),1).data().toString();
    QString is_group = index.sibling(index.row(),2).data().toString();
    stream << name << full_name << is_group;
  }

  mimeData->setData("application/x-sedgroupitem", encodedData);
  return mimeData;
 }



 std::vector<QModelIndex> SedGroupModel::getAllChildren(QModelIndex parent) const{
   std::vector<QModelIndex> result{};

   auto parent_item = this->itemFromIndex(parent);
   for(int i=0; i<parent_item->rowCount();++i){
     auto child = parent_item->child(i);
     result.push_back(child->index());
     for (auto grand_child: getAllChildren(child->index())){
       result.push_back(grand_child);
     }
   }

   return result;
 }

}
}
