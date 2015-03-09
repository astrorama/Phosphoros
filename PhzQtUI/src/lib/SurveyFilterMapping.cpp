#include <iostream>
#include "PhzQtUI/FileUtils.h"
#include <QDir>
#include <QTextStream>
#include <QDomDocument>

#include "PhzQtUI/SurveyFilterMapping.h"

namespace Euclid {
namespace PhzQtUI {
    SurveyFilterMapping::SurveyFilterMapping(){}

    SurveyFilterMapping::SurveyFilterMapping(std::string root_path):m_root_path(root_path){}

    std::string SurveyFilterMapping::getFilterNames(std::string separator) const{
        std::string result="";
        for (auto filter : m_filters){
            result=result + filter.getName() + separator;
        }
        return result.substr(0,result.length()-separator.length());
    }

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

    void SurveyFilterMapping::setDefaultCatalog(std::string new_default_catalog){
      m_default_catalog=new_default_catalog;
    }

    std::string SurveyFilterMapping::getDefaultCatalog() const{
      return m_default_catalog;
    }



    std::map<int,SurveyFilterMapping> SurveyFilterMapping::loadSurveysFromFolder(std::string root_path){
        QDir root_dir(QString::fromStdString(root_path));

                std::map<int,SurveyFilterMapping> map ;

        QStringList fileNames = root_dir.entryList(QDir::Files |  QDir::NoDotAndDotDot );
        int count=0;
        foreach (const QString &fileName, fileNames) {
          try{
            auto survey = SurveyFilterMapping::loadSurveyFromFile(fileName.toStdString(),root_path);
            map[count]=survey;
            ++count;
          } catch (...){} //if a file do not open correctly: just skip it...
        }

        return map;
   }

SurveyFilterMapping SurveyFilterMapping::loadSurveyFromFile(
    std::string fileName, std::string root_path) {

  SurveyFilterMapping survey(root_path);
  survey.setName(FileUtils::removeExt(fileName, ".xml"));

  QDomDocument doc("SurveyFilterMapping");
  QFile file(
      QString::fromStdString(root_path) + QDir::separator()
          + QString::fromStdString(fileName));
  if (!file.open(QIODevice::ReadOnly))
    return survey;
  if (!doc.setContent(&file)) {
    file.close();
    return survey;
  }
  file.close();

  QDomElement root_node = doc.documentElement();
  survey.setName(root_node.attribute("Name").toStdString());
  survey.setSourceIdColumn(root_node.attribute("SourceColumnId").toStdString());
  survey.setDefaultCatalog(root_node.attribute("DefaultCatalogPath").toStdString());

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

  auto filters_node = root_node.firstChildElement("Filters");

   list = filters_node.childNodes();

  for (int i = 0; i < list.count(); ++i) {
    FilterMapping mapping;

    auto node_filter = list.at(i).toElement();
    mapping.setName(node_filter.attribute("Name").toStdString());
    mapping.setFluxColumn(node_filter.attribute("FluxColumn").toStdString());

    if (survey.m_column_list.count(mapping.getFluxColumn()) == 0) {
      survey.m_column_list.insert(mapping.getFluxColumn());
    }
    mapping.setErrorColumn(node_filter.attribute("ErrorColumn").toStdString());

    if (survey.m_column_list.count(mapping.getErrorColumn()) == 0) {
      survey.m_column_list.insert(mapping.getErrorColumn());
    }
    mapping.setFilterFile(
        node_filter.attribute("TransmissionFile").toStdString());
    survey.m_filters.push_back(mapping);
  }

  return survey;
}

     void SurveyFilterMapping::deleteSurvey(){
         QFile(QString::fromStdString(m_root_path) + QDir::separator()+ QString::fromStdString(getName()+".xml")).remove();
     }

     void SurveyFilterMapping::saveSurvey(std::string oldName){
         QFile(QString::fromStdString(m_root_path) + QDir::separator()+ QString::fromStdString(oldName +".xml")).remove();
         QFile file(QString::fromStdString(m_root_path) + QDir::separator()+ QString::fromStdString(getName()+".xml"));
         file.open(QIODevice::WriteOnly );
         QTextStream stream(&file);

          QDomDocument doc("SurveyFilterMapping");
          QDomElement root = doc.createElement("SurveyFilterMapping");
          root.setAttribute("Name",QString::fromStdString(m_survey_name));
          root.setAttribute("SourceColumnId",QString::fromStdString(m_source_id_column));
          root.setAttribute("DefaultCatalogPath",QString::fromStdString(m_default_catalog));
          doc.appendChild(root);

          QDomElement filters_Node = doc.createElement("Filters");
          root.appendChild(filters_Node);

          for(auto& filter : m_filters){
              QDomElement filter_node = doc.createElement("Filter");
              filter_node.setAttribute("Name",QString::fromStdString(filter.getName()));
              filter_node.setAttribute("FluxColumn",QString::fromStdString(filter.getFluxColumn()));
              filter_node.setAttribute("ErrorColumn",QString::fromStdString(filter.getErrorColumn()));
              filter_node.setAttribute("TransmissionFile",QString::fromStdString(filter.getFilterFile()));
              filters_Node.appendChild(filter_node);
          }


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
     }

}
}
