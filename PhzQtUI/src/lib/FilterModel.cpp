#include "PhzQtUI/FilterModel.h"
#include "FileUtils.h"

namespace Euclid {
namespace PhzQtUI {


FilterModel::FilterModel(std::string basePath):m_base_path(basePath)
{
}

void FilterModel::setFilters(const std::vector<FilterMapping>& initFilterList){

    this->setColumnCount(4);
    this->setRowCount(initFilterList.size());
    QStringList  setHeaders;
    setHeaders<<"Filter Transmission Curve"<<"Flux Column Name"<<"Error Column Name"<<"Filter Transmission File Full";
    this->setHorizontalHeaderLabels(setHeaders);

    int i=0;
    for (auto filter:initFilterList){
        std::string shortName = filter.getFilterFile();
        if (FileUtils::starts_with(shortName,m_base_path)){
           shortName=FileUtils::removeStart(shortName,m_base_path);
        }

        this->setItem(i,0,new QStandardItem(QString::fromStdString(shortName)));
        this->setItem(i,1,new QStandardItem(QString::fromStdString(filter.getFluxColumn())));
        this->setItem(i,2,new QStandardItem(QString::fromStdString(filter.getErrorColumn())));
        this->setItem(i,3,new QStandardItem(QString::fromStdString(filter.getFilterFile())));
        ++i;
    }
}

void FilterModel::setFilter(const FilterMapping& filter, int row){
    std::string shortName = filter.getFilterFile();
     if (FileUtils::starts_with(shortName,m_base_path)){
         shortName=FileUtils::removeStart(shortName,m_base_path);
    }
    item(row,0)->setText(QString::fromStdString(shortName));
    item(row,1)->setText(QString::fromStdString(filter.getFluxColumn()));
    item(row,2)->setText(QString::fromStdString(filter.getErrorColumn()));
    item(row,3)->setText(QString::fromStdString(filter.getFilterFile()));
}

std::vector<FilterMapping> FilterModel::getFilters() const{
    std::vector<FilterMapping> mappings;
    for (int i=0; i<this->rowCount();++i){
      mappings.push_back(getFilter(i));
    }

    return mappings;
}

FilterMapping FilterModel::getFilter(int row) const{
    FilterMapping filter;
    filter.setFluxColumn(item(row,1)->text().toStdString());
    filter.setErrorColumn(item(row,2)->text().toStdString());
    filter.setFilterFile(item(row,3)->text().toStdString());
    return filter;

}

void FilterModel::addFilter(const FilterMapping& filter){
    QList<QStandardItem*> items;
    items.push_back(new QStandardItem(QString::fromStdString(filter.getFluxColumn())));
    items.push_back(new QStandardItem(QString::fromStdString(filter.getErrorColumn())));
    std::string shortName = filter.getFilterFile();
    if (FileUtils::starts_with(shortName,m_base_path)){
        shortName=FileUtils::removeStart(shortName,m_base_path);
    }
    items.push_back(new QStandardItem(QString::fromStdString(shortName)));
    items.push_back(new QStandardItem(QString::fromStdString(filter.getFilterFile())));
    this->appendRow(items);
}

void FilterModel::deleteFilter(int row){
    removeRow(row);
}

}
}
