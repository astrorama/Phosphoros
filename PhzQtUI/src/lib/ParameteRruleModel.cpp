#include "PhzQtUI/ParameterRuleModel.h"

namespace Euclid {
namespace PhzQtUI {


ParameterRuleModel::ParameterRuleModel(std::map<int,ParameterRule> init_parameter_rules, std::string sedRootPath, std::string redRootPath):
    m_parameter_rules(init_parameter_rules) //copy the rules
  ,m_sed_root_path(sedRootPath)
  ,m_red_root_path(redRootPath)
{
    this->setColumnCount(8);
    this->setRowCount(m_parameter_rules.size());
    QStringList  setHeaders;
    setHeaders<<"SED(s)"<<""<<"Reddening Curve(s)"<<""<<"E(B-V) Range"<<"z Range"<<"Number of Models"<<"Hidden_Id";
    this->setHorizontalHeaderLabels(setHeaders);

    int i=0;
    for(auto it = m_parameter_rules.begin(); it != m_parameter_rules.end(); ++it ) {
        std::list<QString> list = getItemsRepresentation(it->second,it->first);
        int j=0;
        for (QString item : list){
            this->setItem(i,j,new QStandardItem(item));
            ++j;
        }
        ++i;
    }
}


std::list<QString> ParameterRuleModel::getItemsRepresentation(const ParameterRule& rule, int id) const{

    long sed_number=rule.getSedNumber();
    long total_sed_number = sed_number+rule.getExcludedSeds().size();
    std::string status_sed=std::to_string(sed_number)+"/"+std::to_string(total_sed_number);

    long red_number=rule.getRedCurveNumber();
    long total_red_number = red_number+rule.getExcludedReddenings().size();
    std::string status_red=std::to_string(red_number)+"/"+std::to_string(total_red_number);

    std::list<QString> list;

     std::string root_sed=rule.getSedRootObject(m_sed_root_path);
     if (root_sed.length()==0){
       root_sed="<Root>";
     }
     std::string root_red=rule.getReddeningRootObject(m_red_root_path);
     if (root_red.length()==0){
       root_red="<Root>";
     }
     list.push_back(QString::fromStdString(root_sed));
     list.push_back(QString::fromStdString(status_sed));
     list.push_back(QString::fromStdString(root_red));
     list.push_back(QString::fromStdString(status_red));
     list.push_back(QString::fromStdString(rule.getEbvRange().getStringRepresentation()));
     list.push_back(QString::fromStdString(rule.getZRange().getStringRepresentation()));
     list.push_back(QString::number(rule.getModelNumber()));
     list.push_back(QString::number(id));

     return list;
}

const std::map<int,ParameterRule>& ParameterRuleModel::getParameterRules() const{
    return m_parameter_rules;
}


const QString ParameterRuleModel::getValue(int row,int column) const{
    return this->item(row,column)->text();
}


void ParameterRuleModel::setRanges(Range ebvRange,Range zRange,int row){
     int ref = getValue(row,7).toInt();
      m_parameter_rules[ref].setEbvRange(std::move(ebvRange));
      m_parameter_rules[ref].setZRange(std::move(zRange));

      this->item(row,4)->setText(QString::fromStdString( m_parameter_rules[ref].getEbvRange().getStringRepresentation()));
      this->item(row,5)->setText(QString::fromStdString( m_parameter_rules[ref].getZRange().getStringRepresentation()));
      this->item(row,6)->setText(QString::number( m_parameter_rules[ref].getModelNumber()));
}


void ParameterRuleModel::setSeds(std::string root, std::vector<std::string> exceptions,int row){
    int ref = getValue(row,7).toInt();
     m_parameter_rules[ref].setSedRootObject(std::move(root));
     m_parameter_rules[ref].setExcludedSeds(std::move(exceptions));

     std::string status_sed="All";
     if (  m_parameter_rules[ref].getExcludedSeds().size()>0){
         status_sed="Some";
     }

     this->item(row,0)->setText(QString::fromStdString(m_parameter_rules[ref].getSedRootObject(m_sed_root_path)));
     this->item(row,1)->setText(QString::fromStdString(status_sed));
}

void ParameterRuleModel::setRedCurves(std::string root, std::vector<std::string> exceptions,int row){
    int ref = getValue(row,7).toInt();
     m_parameter_rules[ref].setReddeningRootObject(std::move(root));
     m_parameter_rules[ref].setExcludedReddenings(std::move(exceptions));

     std::string status_red="All";
     if (  m_parameter_rules[ref].getExcludedReddenings().size()>0){
         status_red="Some";
     }

     this->item(row,2)->setText(QString::fromStdString(m_parameter_rules[ref].getReddeningRootObject(m_red_root_path)));
     this->item(row,3)->setText(QString::fromStdString(status_red));
}


int ParameterRuleModel::newParameterRule(int duplicate_from_row ){
    int max_ref = 0;

    for(auto  it = m_parameter_rules.begin(); it != m_parameter_rules.end(); ++it ) {
        if (it->first > max_ref) {
            max_ref = it->first;
        }
    }

    ++max_ref;

    if (duplicate_from_row>=0){
        int ref = getValue(duplicate_from_row,7).toInt();
        ParameterRule rule=m_parameter_rules[ref];
        m_parameter_rules[max_ref]=rule;
    }
    else{
         m_parameter_rules[max_ref]=ParameterRule();
    }

    auto list = getItemsRepresentation( m_parameter_rules.at(max_ref),max_ref);
    QList<QStandardItem*> items;
    for (QString item : list){
       items.push_back(new QStandardItem(item));
    }
    this->appendRow(items);
    return items[0]->row();
}

void ParameterRuleModel::deletRule(int row){
    int ref = getValue(row,7).toInt();
    m_parameter_rules.erase(ref);
    this->removeRow(row);
}


const ParameterRule& ParameterRuleModel::getRule(int row) const{
    int ref = getValue(row,7).toInt();
    return m_parameter_rules.at(ref);
}

}
}


