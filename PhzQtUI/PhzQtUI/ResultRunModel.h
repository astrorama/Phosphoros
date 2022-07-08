/*
 * ResultRunModel.h
 *
 *  Created on: Jun 25, 2019
 *      Author: fdubath
 */

#ifndef PHZQTUI_PHZQTUI_RESULTRUNMODEL_H_
#define PHZQTUI_PHZQTUI_RESULTRUNMODEL_H_

#include <QStandardItemModel>
#include <QString>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Euclid {
namespace PhzQtUI {

class ResultRunModel : public QStandardItemModel {
  Q_OBJECT
public:
  ResultRunModel();
  void load(std::string catalog_result_folder);
};

}  // namespace PhzQtUI
}  // namespace Euclid

#endif /* PHZQTUI_PHZQTUI_RESULTRUNMODEL_H_ */
