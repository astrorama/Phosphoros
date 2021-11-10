#ifndef FORMANALYSIS_H
#define FORMANALYSIS_H

#include <memory>
#include <QWidget>
#include <QStandardItem>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QToolBox>
#include <QFile>
#include <map>
#include <string>
#include "SurveyFilterMapping.h"
#include "ModelSet.h"
#include "PhzQtUI/LuminosityPriorConfig.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"
#include "PhzQtUI/SurveyModel.h"
#include "PhzQtUI/ModelSetModel.h"

namespace boost{
namespace program_options{



 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {


typedef std::shared_ptr<PhzQtUI::DatasetRepository<std::unique_ptr<XYDataset::FileSystemProvider>>> DatasetRepo;

namespace Ui {
class FormAnalysis;
}

///// btn_confLuminosityPrior - cb_luminosityPrior


/**
 * @brief The FormAnalysis class
 * This Widget allows the user to configure and run the analysis
 */
class FormAnalysis : public QWidget
{
    Q_OBJECT

public:
    explicit FormAnalysis(QWidget *parent = 0);
    ~FormAnalysis();
    void loadAnalysisPage(
        std::shared_ptr<SurveyModel> survey_model_ptr,
        std::shared_ptr<ModelSetModel> model_set_model_ptr,
        DatasetRepo filter_repository,
        DatasetRepo luminosity_repository);

    void updateSelection();

signals:

void navigateToParameter(bool);

void navigateToConfig();

void navigateToCatalog(bool);

void navigateToPostProcessing(bool);

void quit(bool);



    void navigateToNewCatalog(std::string);

private slots:

	void on_btn_ToOption_clicked();

	void on_btn_ToCatalog_clicked();

	void on_btn_exit_clicked();

	void on_btn_ToModel_clicked();

	void on_btn_ToPP_clicked();

    void on_btn_editCorrections_clicked();

    void on_cb_AnalysisCorrection_currentIndexChanged(const QString &arg1);

    void on_cb_AnalysisSurvey_currentIndexChanged(const QString &arg1);

    void onFilterSelectionItemChanged(QStandardItem*);

    void on_btn_computeCorrections_clicked();

    void on_btn_lum_filter_clicked();

    void on_cb_AnalysisModel_currentIndexChanged(const QString &);

    void on_cbb_pdf_out_currentIndexChanged(const QString &);

    void on_cb_pdf_z_stateChanged(int);
    void on_cb_likelihood_pdf_z_stateChanged(int);

    void on_cb_igm_currentIndexChanged(const QString &);

    void on_cb_CompatibleGrid_currentTextChanged(const QString &);
    void on_btn_GetConfigGrid_clicked();
    void on_btn_RunGrid_clicked();

    void on_cb_CompatibleGalCorrGrid_currentTextChanged(const QString &);
    void on_btn_GetGalCorrConfigGrid_clicked();
    void on_btn_RunGalCorrGrid_clicked();

    void on_cb_CompatibleShiftGrid_currentTextChanged(const QString &);
    void on_btn_GetShiftConfigGrid_clicked();
    void on_btn_RunShiftGrid_clicked();

    void on_rb_gc_off_clicked();
    void on_rb_gc_col_clicked();
    void on_rb_gc_planck_clicked();

    void on_gb_corrections_clicked();

    void onCorrectionComputed(const QString &);

    void on_btn_BrowseInput_clicked();

    void on_btn_BrowseOutput_clicked();

    void on_btn_GetConfigAnalysis_clicked();

    void on_btn_RunAnalysis_clicked();

    void get_config_run_second_part();

    void run_analysis_second_part();


    void on_btn_pp_clicked();

    void on_cb_gen_posterior_clicked();

    void on_cb_sedweight_clicked();

    void on_btn_confLuminosityPrior_clicked();

    void on_cb_luminosityPrior_2_currentIndexChanged(const QString &);

    void on_rb_luminosityPrior_toggled(bool);
    void on_rb_volumePrior_toggled(bool);
    void on_rb_nzPrior_toggled(bool);
    void on_rb_noPrior_toggled(bool);


    void on_btn_conf_Nz_clicked();

    void on_output_column_btn_clicked();
    void setCopiedColumns(std::map<std::string, std::string> columns);
    void update_pp_selection(std::vector<std::string> params);

    void setNzFilters(std::string b_filter, std::string i_filter);
    void setLumFilter(std::string new_filter);


    void on_rb_best_scaling_toggled(bool);
    void on_rb_sample_scaling_toggled(bool);


    void on_cb_process_limit_stateChanged(int);
    void on_cb_skip_stateChanged(int);

    void httpReadyPlanckRead();
    void cancelDownloadPlanck();
    void updateDownloadProgress(qint64 bytesRead, qint64 totalBytes);


private:
    std::unique_ptr<Ui::FormAnalysis> ui;
    std::list<std::string> getFilters();
    std::list<std::string> getSelectedFilters();
    std::list<std::string> getExcludedFilters();
    std::list<FilterMapping> getSelectedFilterMapping();


    void setupAlgo();

    void updateCopiedColumns(std::list<std::string> new_columns);

    void saveCopiedColumnToCatalog();


    void fillCbColumns(std::set<std::string> columns, std::string default_col);

    void loadLuminosityPriors();

    void setInputCatalogName( std::string name,bool do_test=true);

    void adjustGridsButtons(bool enabled);

    void updateGridSelection();
    void updateGalCorrGridSelection();
    void updateFilterShiftGridSelection();

    bool checkGridSelection(bool addFileCheck, bool acceptNewFile);
    bool checkCompatibleModelGrid(std::string file_name);
    bool checkGalacticGridSelection(bool addFileCheck, bool acceptNewFile);
    bool checkCompatibleGalacticGrid(std::string file_name);
    bool checkFilterShiftGridSelection(bool addFileCheck, bool acceptNewFile);
    bool checkCompatibleFilterShiftGrid(std::string file_name);
    std::map<std::string, boost::program_options::variable_value> getGridConfiguration();
    std::map<std::string, boost::program_options::variable_value> getGalacticCorrectionGridConfiguration();
    std::map<std::string, boost::program_options::variable_value> getFilterShiftGridConfiguration();

    bool checkSedWeightFile(std::string file_name);
    std::string getSedWeightFileName();
    std::map<std::string, boost::program_options::variable_value> getSedWeightOptionMap(std::string output_name);





    static void setToolBoxButtonColor(QToolBox* toolBox, int index, QColor color);

    void updateCorrectionSelection();
    void setComputeCorrectionEnable();

    void setRunAnnalysisEnable(bool enabled);
    std::map < std::string, boost::program_options::variable_value > getRunOptionMap();
    std::set<std::string> getPPListFromConfig();
    std::map<std::string, LuminosityPriorConfig> m_prior_config;
    std::map<std::string,std::string> m_copied_columns = {};

    QProgressDialog *m_progress_dialog = nullptr;

    QNetworkAccessManager *m_network_manager = nullptr;
    QFile *m_downloaded_file = nullptr;
    bool m_httpRequestAborted = false;
    QNetworkReply *m_reply = nullptr;


    DatasetRepo m_filter_repository;
    DatasetRepo m_luminosity_repository;
    std::shared_ptr<SurveyModel> m_survey_model_ptr;
    std::shared_ptr<ModelSetModel> m_model_set_model_ptr;
    std::string m_planck_file = "";
    std::string m_planck_url = "https://lambda.gsfc.nasa.gov/data/foregrounds/EBV/lambda_meisner_finkbeiner_2015_dust_map.fits";

};

}
}
#endif // FORMANALYSIS_H
