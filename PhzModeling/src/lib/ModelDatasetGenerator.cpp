/** 
 * @file ModelDatasetGenerator.h
 * @date Sep 17, 2014
 * @author Florian Dubath
 */

#include "PhzModeling/ModelDatasetGenerator.h"

namespace Euclid {
namespace PhzModeling {


  
  ModelDatasetGenerator::ModelDatasetGenerator(const Euclid::PhzDataModel::ModelAxesTuple& axes_tuple,
                        const std::vector<Euclid::XYDataset::XYDataset>& sed_data_vector,
                        const std::vector<ExtinctionFunctor>& reddening_curve_functor_vector,
                        size_t current_index)
      : m_index_helper{Euclid::GridContainer::makeGridIndexHelper(axes_tuple)}, m_axes_tuple(axes_tuple), m_current_index{current_index},
        m_size{m_index_helper.m_axes_index_factors.back()}, m_sed_data_vector(sed_data_vector),
        m_reddening_curve_functor_vector(reddening_curve_functor_vector) { }
       
  ModelDatasetGenerator::ModelDatasetGenerator(const ModelDatasetGenerator& other)
      : m_index_helper{Euclid::GridContainer::makeGridIndexHelper(other.m_axes_tuple)}, m_axes_tuple(other.m_axes_tuple), m_current_index{other.m_current_index},
        m_size{other.m_size}, m_sed_data_vector(other.m_sed_data_vector),
        m_reddening_curve_functor_vector(other.m_reddening_curve_functor_vector) { }
  
  ModelDatasetGenerator& ModelDatasetGenerator::operator=(const ModelDatasetGenerator& other) {
    m_current_index = other.m_current_index;
    m_current_reddened_sed.reset();
    return *this;
  }
  
  ModelDatasetGenerator& ModelDatasetGenerator::operator=(size_t other) {
    m_current_index = other;
    m_current_reddened_sed.reset();
    return *this;
  }
  
  ModelDatasetGenerator& ModelDatasetGenerator::operator++() {
    if (m_current_index < m_size) {
      ++m_current_index;
    }
    return *this;
  }
  
  ModelDatasetGenerator& ModelDatasetGenerator::operator+=(int n) {
    m_current_index += n;
    if (m_current_index > m_size) {
      m_current_index = m_size;
    }
    return *this;
  }
  
  int ModelDatasetGenerator::operator-(size_t other) const {
    return m_current_index - other;
  }
  
  int ModelDatasetGenerator::operator-(const ModelDatasetGenerator& other) const {
    return m_current_index - other.m_current_index;
  }
  
  bool ModelDatasetGenerator::operator==(size_t other) const {
    return m_current_index == other;
  }
  
  bool ModelDatasetGenerator::operator==(const ModelDatasetGenerator& other) const {
    return m_current_index == other.m_current_index;
  }
  
  bool ModelDatasetGenerator::operator!=(size_t other) const {
    return m_current_index != other;
  }
  
  bool ModelDatasetGenerator::operator!=(const ModelDatasetGenerator& other) const {
    return m_current_index != other.m_current_index;
  }
  
  bool ModelDatasetGenerator::operator>(size_t other) const {
    return m_current_index > other;
  }
  
  bool ModelDatasetGenerator::operator>(const ModelDatasetGenerator& other) const {
    return m_current_index > other.m_current_index;
  }
  
  bool ModelDatasetGenerator::operator<(size_t other) const {
    return m_current_index < other;
  }
  
  bool ModelDatasetGenerator::operator<(const ModelDatasetGenerator& other) const {
    return m_current_index < other.m_current_index;
  }
  
  Euclid::XYDataset::XYDataset& ModelDatasetGenerator::operator*() {
    // We calculate the parameter indices for the current index
    size_t new_sed_index = m_index_helper.axisIndex(Euclid::PhzDataModel::ModelParameter::SED, m_current_index);
    size_t new_reddening_curve_index = m_index_helper.axisIndex(Euclid::PhzDataModel::ModelParameter::REDDENING_CURVE, m_current_index);
    size_t new_ebv_index = m_index_helper.axisIndex(Euclid::PhzDataModel::ModelParameter::EBV, m_current_index);
    size_t new_z_index = m_index_helper.axisIndex(Euclid::PhzDataModel::ModelParameter::Z, m_current_index);
    // We check if we need to recalculate the reddened SED
    if (new_sed_index != m_current_sed_index || new_reddening_curve_index != m_current_reddening_curve_index
        || new_ebv_index != m_current_ebv_index || !m_current_reddened_sed) {
      const ExtinctionFunctor& extinction_functor = m_reddening_curve_functor_vector[new_reddening_curve_index];
      double ebv = std::get<Euclid::PhzDataModel::ModelParameter::EBV>(m_axes_tuple)[new_ebv_index];
      m_current_reddened_sed = extinction_functor(m_sed_data_vector[new_sed_index], ebv);
    }
    if (new_sed_index != m_current_sed_index || new_reddening_curve_index != m_current_reddening_curve_index
        || new_ebv_index != m_current_ebv_index || new_z_index != m_current_z_index || !m_current_redshifted_sed) {
      double z = std::get<Euclid::PhzDataModel::ModelParameter::Z>(m_axes_tuple)[new_z_index];
      const auto redshift_functor = PhzModeling::RedshiftFunctor();

      m_current_redshifted_sed.reset( new Euclid::XYDataset::XYDataset(std::move(redshift_functor(*m_current_reddened_sed,z))));
    }
    m_current_sed_index = new_sed_index;
    m_current_reddening_curve_index = new_reddening_curve_index;
    m_current_ebv_index = new_ebv_index;
    m_current_z_index = new_z_index;
    return *m_current_redshifted_sed;
  }
  
} // end of namespace PhzModeling
} // end of namespace Euclid


