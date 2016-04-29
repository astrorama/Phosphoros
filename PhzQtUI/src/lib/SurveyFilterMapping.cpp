#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
using boost::regex;
using boost::regex_match;
using boost::smatch;
#include <boost/algorithm/string.hpp>
#include "ElementsKernel/Exception.h"
#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QTextStream>
#include <QDomDocument>

#include "PhzQtUI/SurveyFilterMapping.h"

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("SurveyFilterMapping");

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

    void SurveyFilterMapping::setFilters(std::vector<FilterMapping> filters){
        m_filters=std::move(filters);
    }

    const std::set<std::string>& SurveyFilterMapping::getColumnList() const{
      return m_column_list;
    }

    void SurveyFilterMapping::setColumnList(std::set<std::string> new_list){
      m_column_list=std::move(new_list);
    }

    const std::vector<FilterMapping>& SurveyFilterMapping::getFilters() const{
        return m_filters;
    }

    void SurveyFilterMapping::setDefaultCatalogFile(std::string new_default_catalog){
      m_default_catalog=new_default_catalog;
    }

    std::string SurveyFilterMapping::getDefaultCatalogFile() const{
      return m_default_catalog;
    }

    void SurveyFilterMapping::setNonDetection(double non_detection){
      m_non_detection=non_detection;
    }

    double SurveyFilterMapping::getNonDetection() const{
      return m_non_detection;
    }

    std::vector<std::string> SurveyFilterMapping::getAvailableCatalogs(){
      auto cat_root_path = FileUtils::getCatalogRootPath(true,"");
      std::vector<std::string> all_dirs{};
      QDirIterator directories(QString::fromStdString(cat_root_path), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
      while(directories.hasNext()){
              directories.next();
              all_dirs.push_back(directories.fileName().toStdString());
      }

      return all_dirs;

    }

std::map<int,SurveyFilterMapping> SurveyFilterMapping::loadCatalogMappings(){
  auto catalog_config_path = QString::fromStdString(FileUtils::getCatalogConfigRootPath(true));

  std::map<int,SurveyFilterMapping> mappings{};
  int id = 0;

  // 1) list the folders under the Catalog folder
  for (auto& catalog_name : getAvailableCatalogs()){
    // look for existing config
    auto catalog_conf = catalog_config_path+QDir::separator()+QString::fromStdString(catalog_name)+".xml";

    QFileInfo info(catalog_conf);
     if (info.exists()){
       mappings[id]= SurveyFilterMapping::loadCatalog(catalog_name+".xml");
     } else {
       SurveyFilterMapping mapping{};
       mapping.setName(catalog_name);
       mapping.ReadFilters();
       mappings[id]=std::move(mapping);
     }
     ++id;
  }


  return mappings;
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
  survey.setNonDetection(root_node.attribute("NonDetection").toDouble());

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

  survey.ReadFilters();

  return survey;
}


void SurveyFilterMapping::ReadFilters(){
  std::vector<FilterMapping> mappings { };

  auto mapping_path = QString::fromStdString(
      FileUtils::getIntermediaryProductRootPath(true, getName()))
      + QDir::separator() + "filter_mapping.txt";

  try{
    std::ifstream in { mapping_path.toStdString() };
    std::string line;
    regex expr { "\\s*([^\\s#]+)\\s+([^\\s#]+)\\s+([^\\s#]+)\\s*(#.*)?" };
    while (std::getline(in, line)) {
      boost::trim(line);
      if (line[0] == '#') {
        continue;
      }
      smatch match_res;
      if (!regex_match(line, match_res, expr)) {
        logger.error() << "Syntax error in "
            << mapping_path.toStdString() << ": " << line;
        throw Elements::Exception() << "Syntax error in "
            << mapping_path.toStdString() << ": " << line;
      }

      FilterMapping mapping;
      mapping.setFilterFile(match_res.str(1));
      mapping.setFluxColumn(match_res.str(2));
      mapping.setErrorColumn(match_res.str(3));
      mappings.push_back(mapping);

    }
  } catch (...){}

  setFilters(mappings);
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
  root.setAttribute("NonDetection",QString::number(m_non_detection));
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
