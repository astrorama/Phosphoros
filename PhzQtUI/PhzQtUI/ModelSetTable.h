#ifndef MODELSETTABLE_H
#define MODELSETTABLE_H

#include "ModelSetModel.h"
#include "ParameterRule.h"
#include <QTableView>
#include <memory>

namespace Euclid {
namespace PhzQtUI {

/**
 * @brief The ModelSetTable class
 * A tableView to display the ModelSetModel.
 */
class ModelSetTable : public QTableView {

  Q_OBJECT
public:
  ModelSetTable(QWidget*& parent);

  void load(std::shared_ptr<ModelSetModel> model_set_model_ptr);

private:
};

}  // namespace PhzQtUI
}  // namespace Euclid
#endif  // MODELSETTABLE_H
