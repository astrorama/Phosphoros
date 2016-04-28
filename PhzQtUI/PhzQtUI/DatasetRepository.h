#ifndef PHZQTUI_DATASETREPOSITORY
#define PHZQTUI_DATASETREPOSITORY

#include <vector>
#include <memory>
#include "XYDataset/QualifiedName.h"


namespace Euclid {
namespace PhzQtUI {
/**
 * @class DatasetRepository
 * Templated class
 * @brief This class store the data provided by an object implementing the
 * XYDataset::XYDatasetProvider interface. It cache the data preventing
 * a (costly) access to the underlying storage every time the content
 * of the provider is requested.
 */
template<class T>
class DatasetRepository
{
public:
  /**
   * @brief constructor
   * @param provider
   * The XYDatasetProvider to be moved into the DatasetRepository.
   */
  DatasetRepository(T provider) :
      m_provider(std::move(provider)) {
  }

  /**
   * @brief Flush the data and reload them from the provider.
   */
  void reload() {

    m_content = m_provider->listContents("");
  }

  /**
   * @brief Replace the provider by moving in the parameter instance
   * and reload the data.
   * @param provider
   * The new XYDatasetProvider to be moved into the DatasetRepository.
   */
  void resetProvider(T provider) {
    m_provider = std::move(provider);
    reload();
  }

  /**
   * @brief Get the (cached copy of the) content of the provider.
   * @return A vector of qualified name representing the provider content.
   */
  const std::vector<XYDataset::QualifiedName> & getContent() const {
    return m_content;
  }

private:
  T m_provider;
  std::vector<XYDataset::QualifiedName> m_content;
};

}
}

#endif // PHZQTUI_DATASETREPOSITORY
