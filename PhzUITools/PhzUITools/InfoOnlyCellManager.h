#include <iostream>
#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include "PhzDataModel/PhzModel.h"

class InfoOnlyCellIter {
public:
  InfoOnlyCellIter(size_t current_index, size_t size, std::vector<std::string>& filters)
        : m_current_index(current_index), m_size(size), m_filters(filters) { }
  InfoOnlyCellIter& operator+=(int n) {
    m_current_index += n;
    if (m_current_index > m_size) {
      m_current_index = m_size;
    }
    return *this;
  }
  int operator-(const InfoOnlyCellIter& other) const {
    return this->m_current_index - other.m_current_index;
  }
  std::vector<std::string>& operator*() {
    return m_filters;
  }
private:
  size_t m_current_index;
  size_t m_size;
  std::vector<std::string>& m_filters;
};

struct InfoOnlyCellManager {
  size_t size = 0;
  std::vector<std::string> filters;
};

namespace Euclid {
namespace GridContainer {

template<>
struct GridCellManagerTraits<InfoOnlyCellManager> {
  typedef std::vector<std::string> data_type;
  typedef InfoOnlyCellIter iterator;
  static std::unique_ptr<InfoOnlyCellManager> factory(size_t size) {
    std::unique_ptr<InfoOnlyCellManager> result {new InfoOnlyCellManager {}};
    result->size = size;
    return result;
  }
  static InfoOnlyCellIter begin(InfoOnlyCellManager& manager) {
    return InfoOnlyCellIter(0, manager.size, manager.filters);
  }
};

}
}

typedef Euclid::PhzDataModel::PhzGrid<InfoOnlyCellManager> MyContainer;

namespace boost {
namespace serialization {

template<typename Archive>
void load(Archive& ar, MyContainer& grid, const unsigned int) {
  auto iter = grid.begin();
  ar >> *iter;
}

template<typename Archive>
void serialize(Archive& ar, MyContainer& t, const unsigned int version) {
  split_free(ar, t, version);
}

}
}

//int main() {
//  std::ifstream in {"/tmp/CreatePhotometryGrid/photometry_grid.dat"};
//  auto grid = Euclid::PhzDataModel::phzGridBinaryImport<InfoOnlyCellManager>(in);
//  std::cout << grid.size() << '\n';
//  auto& sed_axis = grid.getAxis<Euclid::PhzDataModel::ModelParameter::SED>();
//  std::cout << sed_axis.name() << '\n';
//  for (auto& sed : sed_axis) {
//    std::cout << sed.qualifiedName() << '\n';
//  }
//  std::cout << "\nFilters:\n";
//  for (auto& f : *grid.begin()) {
//    std::cout << f << "\n";
//  }
//}
