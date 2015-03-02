#ifndef DIALOGPHOTCORRECTIONEDITION_H
#define DIALOGPHOTCORRECTIONEDITION_H

#include <QDialog>
#include <QModelIndex>
#include "PhzDataModel/PhotometricCorrectionMap.h"

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class DialogPhotCorrectionEdition;
}

/**
 * @brief The DialogPhotCorrectionEdition class
 * This popup allows the user to view and edit the photometric corrections
 * stored in a file.
 */
class DialogPhotCorrectionEdition : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPhotCorrectionEdition(QWidget *parent = 0);
    ~DialogPhotCorrectionEdition();

    /**
     * @brief Initialise the popup by setting its internal data
     * @param filePath
     * the (relative) path of the  photometric corrections file
     */
    void setCorrectionsFile(std::string filePath);

private slots:
    /**
     * SLOT Switch the popup in edition.
     */
    void on_btn_Edit_clicked();

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
    Ui::DialogPhotCorrectionEdition *ui;
    std::string m_file_path;
    PhzDataModel::PhotometricCorrectionMap m_map;
};

}
}
#endif // DIALOGPHOTCORRECTIONEDITION_H
