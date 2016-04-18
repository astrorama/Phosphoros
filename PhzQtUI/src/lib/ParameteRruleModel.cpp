#include "PhzQtUI/ParameterRuleModel.h"

namespace Euclid {
namespace PhzQtUI {


ParameterRuleModel::ParameterRuleModel(std::map<int,ParameterRule> init_parameter_rules, DatasetRepo sed_repo, DatasetRepo red_curve_repo):
    m_parameter_rules(init_parameter_rules) //copy the rules
  ,m_sed_repo(sed_repo)
  ,m_red_curve_repo(red_curve_repo)
{
    this->setColumnCount(7);
    this->setRowCount(m_parameter_rules.size());
    QStringList  setHeaders;
    setHeaders<<"Name"<<"SED(s)"<<"Reddening Curve(s)"<<"E(B-V) Range"<<"z Range"<<"Size"<<"Hidden_Id";
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


std::string ParameterRuleModel::getParamName(const ParameterRule& rule) const{
  return rule.getName();
}


std::string ParameterRuleModel::getSedStatus(const ParameterRule& rule) const{
  auto sed_number=rule.getSedNumber(m_sed_repo);
  return std::to_string(sed_number.first)+"/"+std::to_string(sed_number.second);

}

std::string ParameterRuleModel::getSedGroupName(const ParameterRule& rule) const{
  return rule.getSedGroupName();
}

std::string ParameterRuleModel::getRedStatus(const ParameterRule& rule) const{
  auto red_number=rule.getRedCurveNumber(m_red_curve_repo);
  return std::to_string(red_number.first)+"/"+std::to_string(red_number.second);

}

std::string ParameterRuleModel::getRedGroupName(const ParameterRule& rule) const{
  return rule.getRedCurveGroupName();
}


std::list<QString> ParameterRuleModel::getItemsRepresentation(ParameterRule& rule, int id) const{
     std::list<QString> list;

     list.push_back(QString::fromStdString(getParamName(rule)));
     list.push_back(QString::fromStdString(getSedGroupName(rule) +" ("+getSedStatus(rule)+")"));
     list.push_back(QString::fromStdString(getRedGroupName(rule)+" ("+getRedStatus(rule)+")"));
     list.push_back(QString::fromStdString(rule.getEbvRangeString()));
     list.push_back(QString::fromStdString(rule.getRedshiftRangeString()));
     list.push_back(QString::number(rule.getModelNumber()));
     list.push_back(QString::number(id));

     return list;
}

const std::map<int,ParameterRule>& ParameterRuleModel::getParameterRules() const{
    return m_parameter_rules;
}

bool ParameterRuleModel::checkNameAlreadyUsed(std::string new_name,int row) const{
  int ref = getValue(row,6).toInt();
  for(auto it = m_parameter_rules.begin(); it != m_parameter_rules.end(); ++it ) {
    if (it->second.getName()==new_name &&  it->first!=ref){
      return false;
    }
  }

  return true;
}


const QString ParameterRuleModel::getValue(int row,int column) const{
    return this->item(row,column)->text();
}

void ParameterRuleModel::setName(std::string new_name,int row){
  int ref = getValue(row,6).toInt();
  m_parameter_rules[ref].setName(new_name);
  this->item(row,0)->setText(QString::fromStdString(new_name));
}


void ParameterRuleModel::setRedshiftRanges(std::vector<Range> z_ranges,int row){
  int ref = getValue(row,6).toInt();
  m_parameter_rules[ref].setZRanges(std::move(z_ranges));
  this->item(row,4)->setText(QString::fromStdString( m_parameter_rules[ref].getRedshiftRangeString()));
  this->item(row,5)->setText(QString::number( m_parameter_rules[ref].getModelNumber(true)));
}

void ParameterRuleModel::setEbvRanges(std::vector<Range> ebv_ranges,int row){
  int ref = getValue(row,6).toInt();
  m_parameter_rules[ref].setEbvRanges(std::move(ebv_ranges));
  this->item(row,3)->setText(QString::fromStdString( m_parameter_rules[ref].getEbvRangeString()));
  this->item(row,5)->setText(QString::number( m_parameter_rules[ref].getModelNumber(true)));

}

void ParameterRuleModel::setEbvValues(std::set<double> values,int row){
  int ref = getValue(row,6).toInt();
  m_parameter_rules[ref].setEbvValues(values);
  this->item(row,3)->setText(QString::fromStdString( m_parameter_rules[ref].getEbvRangeString()));
  this->item(row,5)->setText(QString::number( m_parameter_rules[ref].getModelNumber(true)));
 }

void ParameterRuleModel::setRedshiftValues(std::set<double> values,int row){
  int ref = getValue(row,6).toInt();
  m_parameter_rules[ref].setRedshiftValues(values);
  this->item(row,4)->setText(QString::fromStdString(m_parameter_rules[ref].getRedshiftRangeString()));
  this->item(row,5)->setText(QString::number( m_parameter_rules[ref].getModelNumber(true)));
 }

void ParameterRuleModel::setSeds(DatasetSelection state_selection,int row){
    int ref = getValue(row,6).toInt();
     m_parameter_rules[ref].setSedSelection(std::move(state_selection));

     this->item(row,1)->setText(QString::fromStdString(getSedGroupName(m_parameter_rules[ref])
         +" ("+getSedStatus(m_parameter_rules[ref])+")"));
}

void ParameterRuleModel::setRedCurves(DatasetSelection state_selection,int row){
    int ref = getValue(row,6).toInt();
     m_parameter_rules[ref].setRedCurveSelection(std::move(state_selection));

     this->item(row,2)->setText(QString::fromStdString(getRedGroupName(m_parameter_rules[ref])
         +" ("+getRedStatus(m_parameter_rules[ref])+")"));
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
        int ref = getValue(duplicate_from_row,6).toInt();
        ParameterRule rule=m_parameter_rules[ref];
        m_parameter_rules[max_ref]=rule;
        m_parameter_rules[max_ref].setName(rule.getName()+" copy");
    }
    else{
         m_parameter_rules[max_ref]=ParameterRule();
         m_parameter_rules[max_ref].setName("New Parameter Set "+ std::to_string(max_ref+1));
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
    int ref = getValue(row,6).toInt();
    m_parameter_rules.erase(ref);
    this->removeRow(row);
}


const ParameterRule& ParameterRuleModel::getRule(int row) const{
    int ref = getValue(row,6).toInt();
    return m_parameter_rules.at(ref);
}

}
}


