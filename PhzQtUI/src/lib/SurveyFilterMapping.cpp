#include <iostream>
#include "FileUtils.h"
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QDomDocument>

#include "PhzQtUI/SurveyFilterMapping.h"

namespace Euclid {
namespace PhzQtUI {
    SurveyFilterMapping::SurveyFilterMapping(){}


    int SurveyFilterMapping::getFilterNumber() const{
        return m_filters.size();
    }

    void SurveyFilterMapping::setName(std::string newSurveyName){
        m_survey_name=newSurveyName;
    }

    std::string SurveyFilterMapping::getName() const{
        return m_survey_name;
    }

    void SurveyFilterMapping::setSourceIdColumn(std::string newSourceIdColumn){
        m_source_id_column=newSourceIdColumn;
    }

    std::string SurveyFilterMapping::getSourceIdColumn() const{
        return m_source_id_column;
    }

    void SurveyFilterMapping::setFilters(std::list<FilterMapping> filters){
        m_filters=std::move(filters);
    }

    const std::set<std::string>& SurveyFilterMapping::getColumnList() const{
      return m_column_list;
    }

    void SurveyFilterMapping::setColumnList(std::set<std::string> new_list){
      m_column_list=std::move(new_list);
    }

    const std::list<FilterMapping>& SurveyFilterMapping::getFilters() const{
        return m_filters;
    }

    void SurveyFilterMapping::setDefaultCatalogFile(std::string new_default_catalog){
      m_default_catalog=new_default_catalog;
    }

    std::string SurveyFilterMapping::getDefaultCatalogFile() const{
      return m_default_catalog;
    }

    std::list<std::string> SurveyFilterMapping::getAvailableCatalogs(){
      auto cat_root_path = FileUtils::getCatalogRootPath(true,"");
      std::list<std::string> all_dirs{};
      QDirIterator directories(QString::fromStdString(cat_root_path), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
      while(directories.hasNext()){
              directories.next();
              all_dirs.push_back(directories.fileName().toStdString());
      }

      return all_dirs;

    }

std::map<int,SurveyFilterMapping> SurveyFilterMapping::loadCatalogMappings(){
  auto additional_info_path = QString::fromStdString(FileUtils::getGUIConfigPath())+QDir::separator()+"Catalogs";
  QFileInfo info(additional_info_path);
  if (!info.exists()){
    QDir().mkpath(additional_info_path);
  }

  QDir root_dir(additional_info_path);

  std::map<int,SurveyFilterMapping> map ;
  QStringList fileNames = root_dir.entryList(QDir::Files | QDir::NoDotAndDotDot );

  int count=0;
  foreach (const QString &fileName, fileNames) {
    try{
      auto survey = SurveyFilterMapping::loadCatalog(fileName.toStdString());
      map[count]=survey;
      ++count;
    } catch (...){} //if a file do not open correctly: just skip it...
  }

  return map;
}

SurveyFilterMapping SurveyFilterMapping::loadCatalog(std::string name) {
  auto additional_info_path = QString::fromStdString(FileUtils::getGUIConfigPath())
  +QDir::separator()+"Catalogs"+QDir::separator();

  SurveyFilterMapping survey;
  survey.setName(FileUtils::removeExt(name, ".xml"));

  // read the xml encoded additional info
  QDomDocument doc("CatalogInfo");
  QFile file(additional_info_path + QDir::separator() + QString::fromStdString(name));
  if (!file.open(QIODevice::ReadOnly))
    return survey;
  if (!doc.setContent(&file)) {
    file.close();
    return survey;
  }
  file.close();

  QDomElement root_node = doc.documentElement();
  survey.setSourceIdColumn(root_node.attribute("SourceColumnId").toStdString());
  survey.setDefaultCatalogFile(root_node.attribute("DefaultCatalogPath").toStdString());

  auto columns_node = root_node.firstChildElement("AvailableColumns");
  auto list = columns_node.childNodes();
  survey.m_column_list.clear();
  for (int i = 0; i < list.count(); ++i) {
    auto node_column = list.at(i).toElement();
    survey.m_column_list.insert(node_column.text().toStdString());
  }

  if (survey.m_column_list.count(survey.getSourceIdColumn())==0){
    survey.m_column_list.insert(survey.getSourceIdColumn());
  }

  // read the filter mapping info
  std::list<FilterMapping> mappings{};

  auto mapping_path=QString::fromStdString(FileUtils::getIntermediaryProductRootPath(true,FileUtils::removeExt(name, ".xml")))+QDir::separator();
  QFile mapping_file(mapping_path+"filter_mapping.txt");

  if (mapping_file.open(QIODevice::ReadOnly))
  {
     QTextStream in(&mapping_file);
     while (!in.atEnd())
     {
        QString line = in.readLine();
        auto tokens = line.split(" ");
        if (tokens.length()==3){
          FilterMapping mapping;
          mapping.setFilterFile(tokens[0].toStdString());
          mapping.setFluxColumn(tokens[1].toStdString());
          mapping.setErrorColumn(tokens[2].toStdString());
          mappings.push_back(mapping);
        }
     }
     mapping_file.close();
  }

  survey.setFilters(mappings);

  return survey;
}

void SurveyFilterMapping::deleteSurvey(){
  auto additional_info_path = QString::fromStdString(FileUtils::getGUIConfigPath())
  +QDir::separator()+"Catalogs"+QDir::separator();
  QFile(additional_info_path+ QString::fromStdString(getName()+".xml")).remove();
}

void SurveyFilterMapping::saveSurvey(std::string oldName){
  // ensure the Catalog path
  auto cat_path = QString::fromStdString(FileUtils::getCatalogRootPath(true,getName()))+QDir::separator();
  QFileInfo info(cat_path);
  if (!info.exists()){
     QDir().mkpath(cat_path);
  }

  // save the xml additional info
  auto additional_info_path = QString::fromStdString(FileUtils::getGUIConfigPath())
  +QDir::separator()+"Catalogs"+QDir::separator();
  QFile(additional_info_path+ QString::fromStdString(oldName +".xml")).remove();
  QFile file(additional_info_path+ QString::fromStdString(getName()+".xml"));
  file.open(QIODevice::WriteOnly );
  QTextStream stream(&file);

  QDomDocument doc("CatalogInfo");
  QDomElement root = doc.createElement("CatalogInfo");
  root.setAttribute("SourceColumnId",QString::fromStdString(m_source_id_column));
  root.setAttribute("DefaultCatalogPath",QString::fromStdString(m_default_catalog));
  doc.appendChild(root);

  QDomElement columns_node = doc.createElement("AvailableColumns");
  for(auto& column : m_column_list){
     QDomElement text_element = doc.createElement("AvailableColumn");
     text_element.appendChild(doc.createTextNode(QString::fromStdString(column)));
     columns_node.appendChild(text_element);
  }
  root.appendChild(columns_node);
  QString xml = doc.toString();
  stream<<xml;
  file.close();

  // save the filter mapping info
  auto mapping_path=QString::fromStdString(FileUtils::getIntermediaryProductRootPath(true,getName()))+QDir::separator();
  QFile mapping_file(mapping_path+"filter_mapping.txt");

  mapping_file.open(QIODevice::WriteOnly );
  QTextStream mapping_stream(&mapping_file);

  for (auto& filter : m_filters) {
    mapping_stream<< QString::fromStdString(filter.getFilterFile()) << " "
            << QString::fromStdString(filter.getFluxColumn())<< " "
            << QString::fromStdString(filter.getErrorColumn()) << "\n";
  }

  mapping_file.close();


}

}
}
