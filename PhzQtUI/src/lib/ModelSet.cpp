#include <iostream>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDomDocument>
#include "PhzQtUI/FileUtils.h"

#include "PhzQtUI/ModelSet.h"
namespace Euclid {
namespace PhzQtUI {

ModelSet::ModelSet(){}

ModelSet::ModelSet(std::string root_path):m_root_path(root_path)
{

}


std::map<int,ModelSet> ModelSet::loadModelSetsFromFolder(std::string root_path){

    QDir root_dir(QString::fromStdString(root_path));

    std::map<int,ModelSet> map ;

    QStringList fileNames = root_dir.entryList(QDir::Files |  QDir::NoDotAndDotDot );
    int count=0;
    foreach (const QString &fileName, fileNames) {
      try{
        auto set = PhzQtUI::ModelSet::loadModelSetFromFile(fileName.toStdString(),root_path);
        map[count]=set;
        ++count;
      } catch(...){} //if a file do not open correctly: just skip it...
    }

    return map;
}

long long ModelSet::getModelNumber() const{
    long long result=0;
    for(auto it = m_parameter_rules.begin(); it != m_parameter_rules.end(); ++it ){
        result+=it->second.getModelNumber();
    }

    return result;
}

ModelSet ModelSet::loadModelSetFromFile(std::string fileName,std::string root_path){

    ModelSet model(root_path);
    model.setName(FileUtils::removeExt(fileName,".xml"));


    QDomDocument doc("ModelSet");
    QFile file(QString::fromStdString(root_path)+QDir::separator()+QString::fromStdString(fileName));
    if (!file.open(QIODevice::ReadOnly))
         return model;
    if (!doc.setContent(&file)) {
        file.close();
        return model;
    }
    file.close();

    QDomElement root_node = doc.documentElement();
    model.setName(root_node.attribute("Name").toStdString());


    auto rules_node = root_node.firstChildElement("ParameterRules");

    auto list =rules_node.childNodes();

    for(int i=0;i<list.count();++i ){
         ParameterRule rule;

         auto node_rule = list.at(i).toElement();
         rule.setSedRootObject(node_rule.attribute("SedRootObject").toStdString());
         rule.setReddeningRootObject(node_rule.attribute("ReddeningCurveRootObject").toStdString());

         auto ebv_node = node_rule.firstChildElement("EbvRange");
         Range range;
         range.setMin(ebv_node.attribute("Min").toDouble());
         range.setMax(ebv_node.attribute("Max").toDouble());
         range.setStep(ebv_node.attribute("Step").toDouble());
         rule.setEbvRange(std::move(range));

         Range z_range;
         auto z_node = node_rule.firstChildElement("ZRange");
         z_range.setMin(z_node.attribute("Min").toDouble());
         z_range.setMax(z_node.attribute("Max").toDouble());
         z_range.setStep(z_node.attribute("Step").toDouble());
         rule.setZRange(std::move(z_range));


         std::vector<std::string> excluded_reddening_list{};
         auto sub_list = node_rule.firstChildElement("ExcludedReddeningCurves").childNodes();
         for(int j=0;j<list.count();++j ){
             auto sub_node = sub_list.at(j).toElement();
             if (sub_node.hasChildNodes()){
                 excluded_reddening_list.push_back(sub_node.text().toStdString());
             }
         }
         rule.setExcludedReddenings(std::move(excluded_reddening_list));

         std::vector<std::string> excluded_sed_list{};
         sub_list = node_rule.firstChildElement("ExcludedSeds").childNodes();
         for(int j=0;j<list.count();++j ){
             auto sub_node = sub_list.at(j).toElement();
             if (sub_node.hasChildNodes()){
                excluded_sed_list.push_back(sub_node.text().toStdString());
             }
         }
         rule.setExcludedSeds(std::move(excluded_sed_list));

         model.m_parameter_rules[i]=rule;
    }

    return model;
}

void ModelSet::deleteModelSet(){
    QFile(QString::fromStdString(m_root_path) + QDir::separator()+ QString::fromStdString(getName()+".xml")).remove();
}

void ModelSet::saveModelSet(std::string oldName){
    QFile(QString::fromStdString(m_root_path) + QDir::separator()+ QString::fromStdString(oldName+".xml")).remove();
    QFile file(QString::fromStdString(m_root_path) + QDir::separator()+ QString::fromStdString(getName()+".xml"));
    file.open(QIODevice::WriteOnly );
    QTextStream stream(&file);

    QDomDocument doc("ModelSet");
    QDomElement root = doc.createElement("ModelSet");
    root.setAttribute("Name",QString::fromStdString(getName()));
    doc.appendChild(root);

    QDomElement rules_Node = doc.createElement("ParameterRules");
    root.appendChild(rules_Node);

    for(auto& rule_pair : m_parameter_rules){
        auto& rule = rule_pair.second;
        QDomElement rule_node = doc.createElement("ParameterRule");
        rule_node.setAttribute("SedRootObject",QString::fromStdString(rule.getSedRootObject()));
        rule_node.setAttribute("ReddeningCurveRootObject",QString::fromStdString(rule.getReddeningRootObject()));

        QDomElement ebv_range_node = doc.createElement("EbvRange");
        const Range& ebv_range=rule.getEbvRange();
        ebv_range_node.setAttribute("Min",ebv_range.getMin());
        ebv_range_node.setAttribute("Max",ebv_range.getMax());
        ebv_range_node.setAttribute("Step",ebv_range.getStep());
        rule_node.appendChild(ebv_range_node);

        QDomElement z_range_node = doc.createElement("ZRange");
        const Range& redshift_range=rule.getZRange();
        z_range_node.setAttribute("Min",redshift_range.getMin());
        z_range_node.setAttribute("Max",redshift_range.getMax());
        z_range_node.setAttribute("Step",redshift_range.getStep());
        rule_node.appendChild(z_range_node);

        QDomElement excluded_sed_node = doc.createElement("ExcludedSeds");
        for(auto& excluded : rule.getExcludedSeds()){
            QDomElement text_element = doc.createElement("ExcludedPath");
            text_element.appendChild(doc.createTextNode(QString::fromStdString(excluded)));
            excluded_sed_node.appendChild(text_element);
        }
        rule_node.appendChild(excluded_sed_node);

        QDomElement excluded_red_node = doc.createElement("ExcludedReddeningCurves");
        for(auto& excluded : rule.getExcludedReddenings()){
            QDomElement text_element = doc.createElement("ExcludedPath");
            text_element.appendChild(doc.createTextNode(QString::fromStdString(excluded)));
            excluded_red_node.appendChild(text_element);
        }
        rule_node.appendChild(excluded_red_node);

        rules_Node.appendChild(rule_node);
    }


    QString xml = doc.toString();

    stream<<xml;
    file.close();
}


std::string ModelSet::getName() const{
    return m_name;
}

void ModelSet::setName(std::string name){
    m_name=name;
}

std::map<int,ParameterRule> ModelSet::getParameterRules() const{
    return m_parameter_rules;
}

void ModelSet::setParameterRules( std::map<int,ParameterRule> parameter_rules){
    m_parameter_rules=std::move(parameter_rules);
}

}
}
