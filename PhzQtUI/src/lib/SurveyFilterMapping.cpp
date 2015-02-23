#include <iostream>
#include "PhzQtUI/FileUtils.h"
#include <QDir>
#include <QTextStream>
#include <QDomDocument>

#include "PhzQtUI/SurveyFilterMapping.h"

namespace Euclid {
namespace PhosphorosUiDm {
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

    const std::list<FilterMapping>& SurveyFilterMapping::getFilters() const{
        return m_filters;
    }

    std::map<int,SurveyFilterMapping> SurveyFilterMapping::loadSurveysFromFolder(std::string root_path){
        QDir root_dir(QString::fromStdString(root_path));

                std::map<int,SurveyFilterMapping> map ;

        QStringList fileNames = root_dir.entryList(QDir::Files |  QDir::NoDotAndDotDot );
        int count=0;
        foreach (const QString &fileName, fileNames) {
            auto survey = PhosphorosUiDm::SurveyFilterMapping::loadSurveyFromFile(fileName.toStdString(),root_path);
            map[count]=survey;
            ++count;
        }

        return map;
   }

     SurveyFilterMapping SurveyFilterMapping::loadSurveyFromFile(std::string fileName, std::string root_path){

        SurveyFilterMapping survey(root_path);
        survey.setName(FileUtils::removeExt(fileName,".xml"));


        QDomDocument doc("SurveyFilterMapping");
        QFile file(QString::fromStdString(root_path)+QDir::separator()+QString::fromStdString(fileName));
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

        auto filters_node = root_node.firstChildElement("Filters");

        auto list =filters_node.childNodes();

        for(int i=0;i<list.count();++i ){
             FilterMapping mapping;

             auto node_filter = list.at(i).toElement();
             mapping.setName(node_filter.attribute("Name").toStdString());
             mapping.setFluxColumn(node_filter.attribute("FluxColumn").toStdString());
             mapping.setErrorColumn(node_filter.attribute("ErrorColumn").toStdString());
             mapping.setFilterFile(node_filter.attribute("TransmissionFile").toStdString());

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
          QString xml = doc.toString();

         stream<<xml;
         file.close();
     }

}
}
