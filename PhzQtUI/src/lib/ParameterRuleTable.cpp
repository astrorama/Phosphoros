
#include <QHeaderView>
#include "PhzQtUI/ParameterRuleTable.h"

namespace Euclid {
namespace PhzQtUI {

ParameterRuleTable::ParameterRuleTable(QWidget*& parent): QTableView(parent){

}


ParameterRuleTable::ParameterRuleTable(QDialog*&){

}

void ParameterRuleTable::loadParameterRules(std::map<int,ParameterRule> parameter_rules,std::string sedRootPath ,std::string redRootPath){
    ParameterRuleModel* new_model = new ParameterRuleModel(parameter_rules,sedRootPath,redRootPath);
    setModel(new_model);
    this->setColumnHidden(6, true);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    this->update(QModelIndex());

}

ParameterRuleModel* ParameterRuleTable::getModel(){
    return static_cast<ParameterRuleModel*>(this->model());
}


const ParameterRuleModel* ParameterRuleTable::cGetModel() const{
    return static_cast<const ParameterRuleModel*>(this->model());
}


bool ParameterRuleTable::hasSelectedPArameterRule(){
    return this->selectionModel()->currentIndex().isValid();
}


void ParameterRuleTable::deletSelectedRule(){
    QModelIndexList index = this->selectionModel()->selectedIndexes();
    getModel()->deletRule(index[0].row());
}

void ParameterRuleTable::newRule(bool duplicate_selected){
    int index_to_duplicate =-1;
    if (duplicate_selected){
        QModelIndexList index = this->selectionModel()->selectedIndexes();
        index_to_duplicate=index[0].row();
    }

    int row_to_select = getModel()->newParameterRule(index_to_duplicate);
    this->selectRow(row_to_select);
}


bool ParameterRuleTable::checkNameAlreadyUsed(std::string new_name) const{
  QModelIndexList index = this->selectionModel()->selectedIndexes();
   return cGetModel()->checkNameAlreadyUsed(new_name,index[0].row());
}

void ParameterRuleTable::setNameToSelectedRule(std::string new_name){

  QModelIndexList index = this->selectionModel()->selectedIndexes();
  getModel()->setName(new_name,index[0].row());
}

void ParameterRuleTable::setRedshiftRangesToSelectedRule(std::vector<Range> ranges){
  QModelIndexList index = this->selectionModel()->selectedIndexes();
  getModel()->setRedshiftRanges(std::move(ranges),index[0].row());
}

void ParameterRuleTable::setEbvRangesToSelectedRule(std::vector<Range> ranges){
  QModelIndexList index = this->selectionModel()->selectedIndexes();
  getModel()->setEbvRanges(ranges,index[0].row());
}

void ParameterRuleTable::setEbvValuesToSelectedRule(std::set<double> values){
  QModelIndexList index = this->selectionModel()->selectedIndexes();
  getModel()->setEbvValues(values,index[0].row());
}

void ParameterRuleTable::setRedshiftValuesToSelectedRule(std::set<double> values){
  QModelIndexList index = this->selectionModel()->selectedIndexes();
  getModel()->setRedshiftValues(values,index[0].row());
}

void ParameterRuleTable::setSedsToSelectedRule(std::string root, std::vector<std::string> exceptions){
    QModelIndexList index = this->selectionModel()->selectedIndexes();
    getModel()->setSeds(std::move(root),std::move(exceptions),index[0].row());
}

void ParameterRuleTable::setRedCurvesToSelectedRule(std::string root, std::vector<std::string> exceptions){
    QModelIndexList index = this->selectionModel()->selectedIndexes();
    getModel()->setRedCurves(std::move(root),std::move(exceptions),index[0].row());
}


const ParameterRule& ParameterRuleTable::getSelectedRule() const{
    QModelIndexList index = this->selectionModel()->selectedIndexes();
    return cGetModel()->getRule(index[0].row());

}

}
}
