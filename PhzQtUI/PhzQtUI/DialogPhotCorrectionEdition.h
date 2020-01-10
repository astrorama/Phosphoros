#ifndef DIALOGPHOTCORRECTIONEDITION_H
#define DIALOGPHOTCORRECTIONEDITION_H

#include <memory>
#include <QDialog>
#include <QModelIndex>
#include "PhzQtUI/FilterMapping.h"
#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogPhotCorrectionEdition;
}

/**
 * @class DialogPhotCorrectionEdition
 * This popup allows the user to view and edit the photometric corrections
 * stored in a file.
 */
class DialogPhotCorrectionEdition : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    explicit DialogPhotCorrectionEdition(QWidget *parent = 0);

    /**
     * @brief Destructor
     */
    ~DialogPhotCorrectionEdition();

    /**
     * @brief Initialise the popup by setting its internal data
     * @param filePath
     * the (relative) path of the  photometric corrections file
     * @param filters
     * the filter mapping allowing to display the aliases
     */
    void setCorrectionsFile(std::string catalog, std::string filePath, std::list<FilterMapping> filters);

private slots:
    /**
     * SLOT switch back the popup in view and revert changes
     */
    void on_btn_Cancel_clicked();

    /**
     * SLOT Update the photometric corrections file with the input values and
     * switch back the popup in view
     */
    void on_btn_Save_clicked();

    /**
     * SLOT Switch the popup in edition (occure when the user starts edition by
     * double-clicking the grid)
     */
    void on_tableView_doubleClicked ( const QModelIndex & );

private:
    std::unique_ptr<Ui::DialogPhotCorrectionEdition> ui;
    std::string m_catalog;
    std::string m_file_path;
    PhzDataModel::PhotometricCorrectionMap m_map;
    std::list<FilterMapping> m_filters;
};

}
}
#endif // DIALOGPHOTCORRECTIONEDITION_H
