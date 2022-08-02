/*
 * ResultRunModel.cpp
 *
 *  Created on: Jun 25, 2019
 *      Author: fdubath
 */

#include "PhzQtUI/ResultRunModel.h"
#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include <QDirIterator>
#include <boost/filesystem.hpp>
#include <tuple>
#include <vector>

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("ResultRunModel");

ResultRunModel::ResultRunModel() {}

void ResultRunModel::load(std::string catalog_result_folder) {

  this->setColumnCount(4);
  QStringList setHeaders;
  setHeaders << "Result Folder"
             << "Action"
             << "flags"
             << "full path";
  this->setHorizontalHeaderLabels(setHeaders);

  auto cat_vector = std::vector<std::pair<std::string, std::string>>{};

  if (catalog_result_folder != "") {
    QDirIterator run_dir(QString::fromStdString(catalog_result_folder),
                         QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    while (run_dir.hasNext()) {
      run_dir.next();
      auto basepath = boost::filesystem::path(run_dir.filePath().toStdString());
      if (boost::filesystem::exists(basepath / "phz_cat.fits") || boost::filesystem::exists(basepath / "phz_cat.txt")) {
        cat_vector.push_back(std::make_pair<std::string, std::string>(run_dir.fileName().toStdString(),
                                                                      run_dir.filePath().toStdString()));
      }
    }
  }

  this->setRowCount(cat_vector.size());
  int i = 0;
  for (auto& it : cat_vector) {
    this->setItem(i, 0, new QStandardItem(QString::fromStdString(it.first)));
    this->setItem(i, 2, new QStandardItem(QString::fromStdString(it.second)));
    ++i;
  }
}

}  // namespace PhzQtUI
}  // namespace Euclid
