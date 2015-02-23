#ifndef FILTERMODEL_H
#define FILTERMODEL_H

#include <QStandardItemModel>
#include<list>
#include "FilterMapping.h"

/**
 * @brief The FilterModel class
 * This class provide a Model to be used in TableView.
 * It display the FilterMappings.
 */
class FilterModel : public QStandardItemModel
{
public:
    /**
     * @brief FilterModel
     * @param basePath
     * this is the folder containing the Filter Transmission curves.
     * This path will be removed of the filter file path for display purpose.
     */
    FilterModel(std::string basePath);

    /**
     * @brief Initialise the FilterModel by setting its internal data
     * @param initFilterList list of FilterMapping which provide data to fill the model
     */
    void setFilters(const std::list<Euclid::PhosphorosUiDm::FilterMapping>& initFilterList);

    /**
     * @brief Replace the representation of the FilterMapping at the row 'row' by
     * the data of the provided 'one'filter'
     * @param filter
     * @param row
     */
    void setFilter(const Euclid::PhosphorosUiDm::FilterMapping& filter, int row);

    /**
     * @brief Build a list of FilterMapping out of its internal state
     * @return the list of FilterMapping handled by the model
     */
    std::list<Euclid::PhosphorosUiDm::FilterMapping> getFilters() const;

    /** @brief  Build a FilterMapping out of data at the row 'row'
      * @param row
      * @return the FilterMapping represented by the row 'row'
      */
    Euclid::PhosphorosUiDm::FilterMapping getFilter(int row) const;

    /**
     * @brief Add a new item to the model representing the provider 'filter'
     * @param filter
     */
    void addFilter(const Euclid::PhosphorosUiDm::FilterMapping& filter);

    /**
     * @brief Delete the filter at the row 'row'
     * @param row
     */
    void deleteFilter(int row);
private:

 std::string   m_base_path;
};

#endif // FILTERMODEL_H
