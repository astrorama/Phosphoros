/*
 * ResultRunModel.h
 *
 *  Created on: Jun 25, 2019
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_RESULTRUNMODEL_H_
#define PHZQTUI_PHZQTUI_RESULTRUNMODEL_H_

#include <QString>
#include <QStandardItemModel>
#include <map>
#include <vector>
#include <set>
#include <string>

namespace Euclid {
namespace PhzQtUI {



class ResultRunModel: public QStandardItemModel {
  Q_OBJECT
public:
  ResultRunModel();
  void load(std::string catalog_result_folder);



};

}
}



#endif /* PHZQTUI_PHZQTUI_RESULTRUNMODEL_H_ */
