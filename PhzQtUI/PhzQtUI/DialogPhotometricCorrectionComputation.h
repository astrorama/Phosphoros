#ifndef DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H
#define DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H

#include <QDialog>
#include <list>
#include "PhzQtUI/FilterMapping.h"

namespace Ui {
class DialogPhotometricCorrectionComputation;
}
/**
 * @brief The DialogPhotometricCorrectionComputation class
 * This popup allows the user to parameter and trigger the computation photometric corrections.
 */
class DialogPhotometricCorrectionComputation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPhotometricCorrectionComputation(QWidget *parent = 0);
    ~DialogPhotometricCorrectionComputation();

    /**
     * @brief Initialise the popup by setting its internal data
     *
     * @param survey
     * the name of the survey
     *
     * @param id_column
     * the name of the ID column into the selected survey
     *
     * @param model
     * the name of the selected model
     *
     * @param grid
     * the (relative) path of the photometric grid file
     *
     * @param selected_filters
     * list of FilterMapping representing the filters to be taken into account
     * for the photometric corrections computation
     */
    void setData(std::string survey, std::string id_column, std::string model,
        std::string grid,
        std::list<Euclid::PhosphorosUiDm::FilterMapping> selected_filters );

signals:
   /**
    * @brief SIGNAL correctionComputed: rised when the poupu is closing and
    * photometric corrections have been computed. The param is the (relative)
    * path of the corrections file.
    */
    void correctionComputed(const std::string &);

private slots:
    /**
     * @brief SLOT open a File dialog for letting the user select the training catalog.
     */
    void on_btn_TrainingCatalog_clicked();

   /**
    * @brief SLOT trigger the refresh of the run button after a change in
    * the specZ column.
    */
    void on_cb_SpectroColumn_currentIndexChanged(const QString &);

   /**
    * @brief SLOT trigger the refresh of the run button after a change in
    * the iteration number.
    */
    void on_txt_Iteration_textChanged(const QString &);

    /**
     * @brief SLOT trigger the refresh of the run button after a change in
     * the tolerance value.
     */
    void on_txt_Tolerence_textChanged(const QString &);

    /**
     * @brief SLOT trigger the refresh of the run button after a change in
     * the output file name.
     */
    void on_txt_FileName_textChanged(const QString &);

    /**
     * @brief SLOT Check if the output file name will override an existing file.
     * If so ask user confirmation, then compute the corrections and write
     * them into the file.
     */
    void on_bt_Run_clicked();


private:
    Ui::DialogPhotometricCorrectionComputation *ui;
    std::list<Euclid::PhosphorosUiDm::FilterMapping> m_selected_filters;
    std::string m_id_column;
    void setRunEnability();
};

#endif // DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H
