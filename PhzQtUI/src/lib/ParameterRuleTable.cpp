
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
    this->setColumnHidden(7, true);
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


void ParameterRuleTable::setRangesToSelectedRule(Range ebvRange, Range zRange){
    QModelIndexList index = this->selectionModel()->selectedIndexes();
    getModel()->setRanges(std::move(ebvRange),std::move(zRange),index[0].row());
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