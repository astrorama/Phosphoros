
#include <iostream>
#include <QFile>
#include <QDir>
#include "QtUI/FileUtils.h"
#include "QtUI/DirectoryTreeModel.h"

DirectoryTreeModel::DirectoryTreeModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

std::string DirectoryTreeModel::getRelPath(std::string path) const{
    return Euclid::PhosphorosUiDm::FileUtils::removeStart(Euclid::PhosphorosUiDm::FileUtils::removeStart(path,m_root_dir),QString(QDir::separator()).toStdString());
}

std::string DirectoryTreeModel::getFullPath(std::string path) const{
    if (Euclid::PhosphorosUiDm::FileUtils::starts_with(path,m_root_dir)){
       return path;
    }

    return m_root_dir + QString(QDir::separator()).toStdString() + path;
}

void DirectoryTreeModel::loadDirectory(std::string rootPath, bool singleLeafSelection,std::string rootDisplayName){
    QDir root_qdir(QString::fromStdString(rootPath));
    m_root_dir=root_qdir.absoluteFilePath(QString::fromStdString(rootPath)).toStdString();

    std::list<std::tuple<std::string,std::string,int,bool>> m_structure;
    std::list<std::string> next_scan_folder{{m_root_dir}};

    while (next_scan_folder.size()>0){
        auto curr_dir = next_scan_folder.front();

        QDir curr_qdir(QString::fromStdString(curr_dir));

        QStringList fileNames = curr_qdir.entryList(QDir::Files | QDir::Dirs |  QDir::NoDotAndDotDot );
        foreach (const QString &fileName, fileNames) {
            QFileInfo curr_file(curr_qdir.absoluteFilePath(QString::fromStdString(curr_dir))+QDir::separator()+ fileName) ;
            std::string full_name= curr_file.absoluteFilePath().toStdString();
            m_structure.push_back(std::make_tuple(full_name,curr_qdir.absoluteFilePath(QString::fromStdString(curr_dir)).toStdString(),curr_file.isDir(),false));
            if ( curr_file.isDir()==1 ){
                next_scan_folder.push_back(full_name);
            }
        }

        next_scan_folder.remove(curr_dir);
    }

    this->setColumnCount(1);
    QStandardItem* root_item = new QStandardItem(QString::fromStdString(rootDisplayName));
    root_item->setCheckable(!singleLeafSelection);
    root_item->setBackground(QBrush(QColor(220, 220, 220)));
    this->appendRow(root_item);

    m_map_dir[m_root_dir]=root_item;
    for (auto structure_item : m_structure){
        QStandardItem* item = new QStandardItem(QString::fromStdString(getRelPath(std::get<0>(structure_item))));
        item->setCheckable(std::get<2>(structure_item)==0 || !singleLeafSelection);
        if (std::get<2>(structure_item)==1){
           item->setBackground(QBrush(QColor(230, 230, 230)));
        }

        m_map_dir[std::get<1>(structure_item)]->appendRow(item);

        if (std::get<2>(structure_item)==1){
             m_map_dir[std::get<0>(structure_item)]=item;
        }
    }
}

void DirectoryTreeModel::setEnabled(bool enable){
    for(auto it:m_map_dir){
        it.second->setEnabled(enable);
        for (int i = 0 ; i<it.second->rowCount(); ++i){
            it.second->child(i)->setEnabled(enable);
        }
    }

    setEditionStatus(enable);
}

void DirectoryTreeModel::checkDir(bool checked,std::string dir, std::list<std::string> exclusions){

    if (dir.compare(".")==0 || dir.compare(this->item(0)->text().toStdString())==0){
        dir=m_root_dir;
    } else{
        dir=getFullPath(dir);
    }

    Qt::CheckState checked_status = Qt::CheckState::Unchecked;
    if(checked){
        checked_status = Qt::CheckState::Checked;
    }

    auto root = m_map_dir.at(dir);
    if( std::find(exclusions.begin(), exclusions.end(), root->text().toStdString())==exclusions.end()){
        if (root->isCheckable()){
            root->setCheckState(checked_status);
        }
        for (int i = 0 ; i<root->rowCount(); ++i){
            auto child = root->child(i);
            if( std::find(exclusions.begin(), exclusions.end(), child->text().toStdString())==exclusions.end()){
                 if (child->isCheckable()){
                    child->setCheckState(checked_status);
                 }

                 if (child->hasChildren()){
                    checkDir(checked,child->text().toStdString(),exclusions);
                }
            }
        }
    }
}

void DirectoryTreeModel::setEditionStatus(bool inEdition){
    m_in_edition=inEdition;
}

void DirectoryTreeModel::selectRoot(){
    this->item(0)->setCheckState(Qt::CheckState::Checked);
}

void DirectoryTreeModel::onItemChangedSingleLeaf(QStandardItem* item){
    if(m_in_edition && !m_bypass_item_changed){
        m_bypass_item_changed=true;
        if(item->checkState()==Qt::CheckState::Checked){
             checkDir(false,m_root_dir);
             item->setCheckState(Qt::CheckState::Checked);
        }
        m_bypass_item_changed=false;
    }
}

void DirectoryTreeModel::onItemChangedUniqueSelection(QStandardItem* item){
    if(m_in_edition && !m_bypass_item_changed){
        m_bypass_item_changed=true;
        if(item->checkState()==Qt::CheckState::Checked){
             checkDir(false,m_root_dir);
             item->setCheckState(Qt::CheckState::Checked);
        } else{
            this->item(0)->setCheckState(Qt::CheckState::Checked);
        }
        m_bypass_item_changed=false;
    }
}

void DirectoryTreeModel::onItemChanged(QStandardItem* item){
    if(m_in_edition && !m_bypass_item_changed){
        m_bypass_item_changed=true;

        bool checked=item->checkState()==Qt::CheckState::Checked;

        if (item->parent() && item->parent()->checkState() >0 ){
            // parent checked: exclusion
             if (item->hasChildren()){
                  checkDir(checked,item->text().toStdString());
             }
        }
        else{
            // inclusion
            checkDir(false,m_root_dir);
            if(checked){
                 if (item->hasChildren()){
                    checkDir(true,item->text().toStdString());
                 } else{
                     item->setCheckState(Qt::CheckState::Checked);
                 }
            }
        }

        m_bypass_item_changed=false;
    }
}

void DirectoryTreeModel::setState(std::string root, const std::list<std::string>& exclusions){
    m_bypass_item_changed=true;


    checkDir(false,m_root_dir);

    if ( root.compare(".")==0 || root.compare(item(0)->text().toStdString())==0 || root.compare(m_root_dir)==0  ){

        checkDir(true,root,exclusions);
    }
    else{
        root= getFullPath(root);

        if (m_map_dir.count(root)){
            checkDir(true,root,exclusions);
        } else{
            std::list<QStandardItem *> item_to_explore{this->item(0)};

            while( item_to_explore.size()>0){
                auto item = item_to_explore.front();

                for(int i=0; i<item->rowCount(); ++i){
                    auto child = item->child(i);
                    if(child->text().toStdString().compare(getRelPath(root))==0){
                       child->setCheckState(Qt::CheckState::Checked);
                       break;
                    }

                    if (child->hasChildren()){
                        item_to_explore.push_back(child);
                    }
                }

                item_to_explore.remove(item);
            }
        }
    }

    m_bypass_item_changed=false;
}

std::pair<bool,std::string> DirectoryTreeModel::getRootSelection(std::string from) const{

    auto root_item = item(0);
    if ( from.compare(".")==0 || from.compare(item(0)->text().toStdString())==0 || from.compare(m_root_dir)==0  ){
        if (root_item->checkState()==Qt::CheckState::Checked){
            return std::make_pair(true,m_root_dir);
        }
    } else {
        from= getFullPath(from);
        root_item = m_map_dir.at(from);
    }

     if (root_item->checkState()==Qt::CheckState::Checked){
         return std::make_pair(true,getFullPath(root_item->text().toStdString()));
     }

     for (int i = 0 ; i<root_item->rowCount(); ++i){
         auto child = root_item->child(i);
         if (child->checkState()==Qt::CheckState::Checked){
             return std::make_pair(true,getFullPath(child->text().toStdString()));
         }

         if (child->hasChildren()){
             auto res= getRootSelection(getFullPath(child->text().toStdString()));
             if (res.first){
                 return res;
             }
         }
     }

     return std::make_pair(false,"");
 }

std::string DirectoryTreeModel::getGroup() const{
      auto res = getRootSelection();
      if (!res.first){
          return "";
      }

      if (m_map_dir.count(res.second)==1 || res.second.compare(m_root_dir)==0){
          return res.second;
      }

      int index = res.second.find_last_of("/", std::string::npos);
      if (index>=0){
        return res.second.substr(0,index);
      } else {
          return "";
      }
  }

std::list<std::string> DirectoryTreeModel::DirectoryTreeModel::getExclusions(std::string root) const{
    std::list<std::string> list;

    auto root_item =item(0);
    if ( root.compare(".")!=0 && root.compare(item(0)->text().toStdString())!=0 && root.compare(m_root_dir)!=0  ){
        root=getFullPath(root);
        if (m_map_dir.count( root )){
          root_item = m_map_dir.at(root);
       } else {
            return list;
       }
    }

   for (int i = 0 ; i<root_item->rowCount(); ++i){
      auto child = root_item->child(i);
      if(child->hasChildren()){
         list.merge(getExclusions(child->text().toStdString()));
      } else if ( child->checkState()!=Qt::CheckState::Checked){
          list.push_back(child->text().toStdString());
      }
   }

   return list;
}
