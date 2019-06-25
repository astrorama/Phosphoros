/*
 * ResultModel.h
 *
 *  Created on: Jun 25, 2019
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_RESULTMODEL_H_
#define PHZQTUI_PHZQTUI_RESULTMODEL_H_

#include <QString>
#include <QStandardItemModel>
#include <map>
#include <vector>
#include <set>
#include <string>

namespace Euclid {
namespace PhzQtUI {



class ResultModel: public QStandardItemModel {
  Q_OBJECT
public:
  ResultModel();
  void load();



};

}
}



#endif /* PHZQTUI_PHZQTUI_RESULTMODEL_H_ */
