#ifndef PHZQTUI_DATASETREPOSITORY
#define PHZQTUI_DATASETREPOSITORY

#include <vector>
#include <memory>
#include "XYDataset/QualifiedName.h"

namespace Euclid {
namespace PhzQtUI {

template<class T>
class DatasetRepository
{
public:
   DatasetRepository(T provider)
        :m_provider(std::move(provider)){}
  void reload(){
    m_content = m_provider->listContents("");
  }

  void resetProvider(T provider){
    m_provider=std::move(provider);
    reload();
  }

  const  std::vector<XYDataset::QualifiedName> & getContent() const{
    return m_content;
  }

private:
    T m_provider;
    std::vector<XYDataset::QualifiedName> m_content;
};

}
}

#endif // PHZQTUI_DATASETREPOSITORY
