/*
 * ResultModel.h
 *
 *  Created on: Jun 25, 2019
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_RESULTMODEL_H_
#define PHZQTUI_PHZQTUI_RESULTMODEL_H_

#include <QStandardItemModel>
#include <QString>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

class ResultModel : public QStandardItemModel {
  Q_OBJECT
public:
  ResultModel();
  void load();
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif /* PHZQTUI_PHZQTUI_RESULTMODEL_H_ */
