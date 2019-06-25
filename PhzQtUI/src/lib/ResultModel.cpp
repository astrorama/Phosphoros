/*
 * ResultModel.cpp
 *
 *  Created on: Jun 25, 2019
 *      Author: fdubath
 */


#include "PhzQtUI/ResultModel.h"
#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include <QDirIterator>
#include <tuple>
#include <vector>
#include <boost/filesystem.hpp>

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("ResultModel");

ResultModel::ResultModel(){

}



void ResultModel::load(){

  this->setColumnCount(3);
  QStringList  setHeaders;
  setHeaders << "Catalog Type" << "Number of Runs" << "Hidden_Id";
  this->setHorizontalHeaderLabels(setHeaders);


  std::string root_path = FileUtils::getResultRootPath(false, "", "");



  auto cat_vector = std::vector<std::tuple<std::string, int, std::string>>{};

  QDirIterator directories(QString::fromStdString(root_path), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    while(directories.hasNext()){
            directories.next();
            int number = 0;

            QDirIterator run_dir(directories.filePath(), QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
            while(run_dir.hasNext()){
                  run_dir.next();
                  auto basepath =  boost::filesystem::path(run_dir.filePath().toStdString());
                  if (boost::filesystem::exists( basepath/"phz_cat.fits") || boost::filesystem::exists(basepath/"phz_cat.txt")){
                    ++number;
                  }
            }


            if (number>0) {
              cat_vector.push_back(std::make_tuple<std::string, int, std::string> (
                    directories.fileName().toStdString(),
                    int(number),
                    directories.filePath().toStdString()
                  ));
            }
   }





  this->setRowCount(cat_vector.size());
  int i = 0;
  for (auto& it : cat_vector) {
      this->setItem(i, 0, new QStandardItem(QString::fromStdString(std::get<0>(it))));
      this->setItem(i, 1, new QStandardItem(QString::number(std::get<1>(it))));
      this->setItem(i, 2, new QStandardItem(QString::fromStdString(std::get<2>(it))));
      ++i;
  }






}







}
}
