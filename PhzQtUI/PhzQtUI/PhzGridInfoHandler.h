#ifndef PHZGRIDINFOHANDLER_H
#define PHZGRIDINFOHANDLER_H

#include "PhzDataModel/PhzModel.h"
#include "PhzQtUI/ModelSet.h"

namespace Euclid {
namespace PhosphorosUiDm {


/**
 * @brief The PhzGridInfoHandler class
 */
class PhzGridInfoHandler
{
public:

  static PhzDataModel::ModelAxesTuple getAxesTuple(const ModelSet& model_set);
};

}
}

#endif // PHZGRIDINFOHANDLER_H

