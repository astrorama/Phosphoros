#include "QtUI/FilterMapping.h"


namespace PhosphorosUiDm {

FilterMapping::FilterMapping(){}

std::string FilterMapping::getName() const{
    return m_name;
}

void FilterMapping::setName(std::string name){
    m_name=name;
}

std::string FilterMapping::getFluxColumn() const{
    return m_flux_column;
}

void FilterMapping::setFluxColumn(std::string flux_column){
    m_flux_column=flux_column;
}

std::string FilterMapping::getErrorColumn() const{
    return m_error_column;
}

void FilterMapping::setErrorColumn(std::string error_column){
    m_error_column=error_column;
}

std::string FilterMapping::getFilterFile() const{
    return m_filter_trnsmission_file;
}

void FilterMapping::setFilterFile(std::string filter_trnsmission_file){
    m_filter_trnsmission_file=filter_trnsmission_file;
}

}
