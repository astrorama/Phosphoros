#include "PhzQtUI/FilterModel.h"
#include "FileUtils.h"

namespace Euclid {
namespace PhzQtUI {

FilterModel::FilterModel(std::string basePath) : m_base_path(basePath) {}

void FilterModel::setFilters(const std::vector<FilterMapping>& initFilterList) {

  this->setColumnCount(5);
  this->setRowCount(initFilterList.size());
  QStringList setHeaders;
  setHeaders << "Filter Transmission"
             << "Flux Column"
             << "Flux Error Column"
             << "Filter Transmission File Full"
             << "Upper limit / error"
             << "Alpha"
             << "Beta"
             << "Gamma"
             << "From MAG"
             << "Filter Shift Column";
  this->setHorizontalHeaderLabels(setHeaders);

  int i = 0;
  for (auto filter : initFilterList) {
    std::string shortName = filter.getFilterFile();
    if (FileUtils::starts_with(shortName, m_base_path)) {
      shortName = FileUtils::removeStart(shortName, m_base_path);
    }

    this->setItem(i, 0, new QStandardItem(QString::fromStdString(shortName)));
    this->setItem(i, 1, new QStandardItem(QString::fromStdString(filter.getFluxColumn())));
    this->setItem(i, 2, new QStandardItem(QString::fromStdString(filter.getErrorColumn())));
    this->setItem(i, 3, new QStandardItem(QString::fromStdString(filter.getFilterFile())));
    this->setItem(i, 4, new QStandardItem(QString::number(filter.getN())));
    this->setItem(i, 5, new QStandardItem(QString::number(filter.getAlpha())));
    this->setItem(i, 6, new QStandardItem(QString::number(filter.getBeta())));
    this->setItem(i, 7, new QStandardItem(QString::number(filter.getGamma())));
    this->setItem(i, 8, new QStandardItem(QString::number(filter.getFromMag())));
    this->setItem(i, 9, new QStandardItem(QString::fromStdString(filter.getShiftColumn())));
    ++i;
  }
}

void FilterModel::setFilter(const FilterMapping& filter, int row) {
  std::string shortName = filter.getFilterFile();
  if (FileUtils::starts_with(shortName, m_base_path)) {
    shortName = FileUtils::removeStart(shortName, m_base_path);
  }
  item(row, 0)->setText(QString::fromStdString(shortName));
  item(row, 1)->setText(QString::fromStdString(filter.getFluxColumn()));
  item(row, 2)->setText(QString::fromStdString(filter.getErrorColumn()));
  item(row, 3)->setText(QString::fromStdString(filter.getFilterFile()));
  item(row, 4)->setText(QString::number(filter.getN()));
  item(row, 5)->setText(QString::number(filter.getAlpha()));
  item(row, 6)->setText(QString::number(filter.getBeta()));
  item(row, 7)->setText(QString::number(filter.getGamma()));
  item(row, 8)->setText(QString::number(filter.getFromMag()));
  item(row, 9)->setText(QString::fromStdString(filter.getShiftColumn()));
}

std::vector<FilterMapping> FilterModel::getFilters() const {
  std::vector<FilterMapping> mappings;
  for (int i = 0; i < this->rowCount(); ++i) {
    mappings.push_back(getFilter(i));
  }

  return mappings;
}

FilterMapping FilterModel::getFilter(int row) const {
  FilterMapping filter;
  filter.setFluxColumn(item(row, 1)->text().toStdString());
  filter.setErrorColumn(item(row, 2)->text().toStdString());
  filter.setFilterFile(item(row, 3)->text().toStdString());
  filter.setN(item(row, 4)->text().toDouble());
  filter.setAlpha(item(row, 5)->text().toDouble());
  filter.setBeta(item(row, 6)->text().toDouble());
  filter.setGamma(item(row, 7)->text().toDouble());
  filter.setFromMag(QVariant(item(row, 8)->text()).toBool());
  filter.setShiftColumn(item(row, 9)->text().toStdString());

  return filter;
}

void FilterModel::addFilter(const FilterMapping& filter) {
  QList<QStandardItem*> items;
  items.push_back(new QStandardItem(QString::fromStdString(filter.getFluxColumn())));
  items.push_back(new QStandardItem(QString::fromStdString(filter.getErrorColumn())));
  std::string shortName = filter.getFilterFile();
  if (FileUtils::starts_with(shortName, m_base_path)) {
    shortName = FileUtils::removeStart(shortName, m_base_path);
  }
  items.push_back(new QStandardItem(QString::fromStdString(shortName)));
  items.push_back(new QStandardItem(QString::fromStdString(filter.getFilterFile())));

  items.push_back(new QStandardItem(QString::number(filter.getN())));
  items.push_back(new QStandardItem(QString::number(filter.getAlpha())));
  items.push_back(new QStandardItem(QString::number(filter.getBeta())));
  items.push_back(new QStandardItem(QString::number(filter.getGamma())));
  items.push_back(new QStandardItem(QString::number(filter.getFromMag())));
  items.push_back(new QStandardItem(QString::fromStdString(filter.getShiftColumn())));
  this->appendRow(items);
}

void FilterModel::deleteFilter(int row) {
  removeRow(row);
}

}  // namespace PhzQtUI
}  // namespace Euclid
