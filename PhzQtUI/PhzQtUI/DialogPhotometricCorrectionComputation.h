#ifndef DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H
#define DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H

#include <map>
#include <string>
#include <memory>
#include <QDialog>
#include <QFutureWatcher>
#include <list>
#include "PhzQtUI/FilterMapping.h"

namespace boost{
namespace program_options{



 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

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
    /**
     * @brief Constructor
     */
    explicit DialogPhotometricCorrectionComputation(QWidget *parent = 0);

    /**
     * @brief Destructor
     */
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
     *
     * @param default_catalog_path the catalog path used to get the columns when
     * constructing the survey: it is proposed as a default value for the
     * training catalog.
     */
    void setData(std::string survey, std::string id_column, std::string model,
        std::string grid,
        std::list<FilterMapping> selected_filters,
        std::list<std::string> excluded_filters,
        std::string default_z_column,
        std::map<std::string, boost::program_options::variable_value> run_option,
        const std::map<std::string, boost::program_options::variable_value>& sed_config,
        double non_detection,
        std::string dust_map_file,
        std::string ra_col ="",
        std::string dec_col ="");

signals:
   /**
    * @brief SIGNAL correctionComputed: rised when the poupu is closing and
    * photometric corrections have been computed. The param is the (relative)
    * path of the corrections file.
    */
    void correctionComputed(const QString &);

    void signalUpdateCurrentIteration(const QString&);

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
     * @brief SLOT trigger the refresh of the default name.
     */
    void on_cb_SelectionMethod_currentIndexChanged(const QString &);

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

    void runFinished();
    void lumFinished();
    void sedFinished();

    void on_bt_Cancel_clicked();

private:
    QFutureWatcher<std::string> m_future_watcher {};
    QFutureWatcher<std::string> m_future_lum_watcher {};
    QFutureWatcher<std::string> m_future_sed_watcher {};
    std::unique_ptr<Ui::DialogPhotometricCorrectionComputation> ui;
    std::list<FilterMapping> m_selected_filters;
    std::list<std::string> m_excluded_filters;
    std::string m_id_column;
    std::string m_refZ_column;
    std::string m_dust_map_file;
    std::string m_ra_col;
    std::string m_dec_col;
    std::map<std::string, boost::program_options::variable_value> m_run_option;
    std::map<std::string, boost::program_options::variable_value> m_sed_config;
    double m_non_detection;
    bool m_computing = false;
    void disablePage();
    void enablePage();
    std::string runFunction();
    std::string runSedFunction();
    void setRunEnability();
    bool loadTestCatalog(QString file_name, bool with_warning);

    bool needLuminosityGrid();
    void reject() override;
};

}
}

#endif // DIALOGPHOTOMETRICCORRECTIONCOMPUTATION_H
