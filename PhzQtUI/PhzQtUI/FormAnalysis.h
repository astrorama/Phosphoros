#ifndef FORMANALYSIS_H
#define FORMANALYSIS_H

#include <memory>
#include <QWidget>
#include <QStandardItem>
#include <QToolBox>
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

    void on_cb_AnalysisModel_currentIndexChanged(const QString &);

    void on_cbb_pdf_out_currentIndexChanged(const QString &);

    void on_cb_pdf_z_stateChanged(int);
    void on_cb_likelihood_pdf_z_stateChanged(int);

    void on_cb_igm_currentIndexChanged(const QString &);

    void on_cb_CompatibleGrid_currentTextChanged(const QString &);

    void on_cb_CompatibleGalCorrGrid_textChanged(const QString &);

    void on_btn_GetConfigGrid_clicked();

    void on_btn_RunGrid_clicked();

    void on_btn_GetGalCorrConfigGrid_clicked();

    void on_btn_RunGalCorrGrid_clicked();

    void on_rb_gc_off_clicked();
    void on_rb_gc_col_clicked();
    void on_rb_gc_planck_clicked();

    void on_gb_corrections_clicked();

    void onCorrectionComputed(const QString &);

    void on_btn_BrowseInput_clicked();

    void on_btn_BrowseOutput_clicked();

    void on_btn_GetConfigAnalysis_clicked();

    void on_btn_RunAnalysis_clicked();

    void on_cb_gen_posterior_clicked();

    void on_cb_gen_likelihood_clicked();

    void on_btn_confLuminosityPrior_clicked();

    void on_cb_luminosityPrior_2_currentIndexChanged(const QString &);

    void on_rb_luminosityPrior_toggled(bool);
    void on_rb_volumePrior_toggled(bool);
    void on_rb_nzPrior_toggled(bool);
    void on_rb_noPrior_toggled(bool);


    void on_btn_conf_Nz_clicked();

    void on_output_column_btn_clicked();
    void setCopiedColumns(std::map<std::string, std::string> columns);

    void setNzFilters(std::string b_filter, std::string i_filter);


private:
    std::unique_ptr<Ui::FormAnalysis> ui;
    std::list<std::string> getFilters();
    std::list<std::string> getSelectedFilters();
    std::list<std::string> getExcludedFilters();
    std::list<FilterMapping> getSelectedFilterMapping();


    void updateCopiedColumns(std::list<std::string> new_columns);

    void saveCopiedColumnToCatalog();


    void fillCbColumns(std::set<std::string> columns);

    void loadLuminosityPriors();

    void setInputCatalogName( std::string name,bool do_test=true);

    void adjustPhzGridButtons(bool enabled);
    void adjustGalCorrGridButtons(bool enabled);

    void updateGridSelection();
    void updateGalCorrGridSelection();
    bool checkGridSelection(bool addFileCheck, bool acceptNewFile);
    bool checkGalacticGridSelection(bool addFileCheck, bool acceptNewFile);
    std::map<std::string, boost::program_options::variable_value> getGridConfiguration();
    std::map<std::string, boost::program_options::variable_value> getGalacticCorrectionGridConfiguration();





    static void setToolBoxButtonColor(QToolBox* toolBox, int index, QColor color);

    void updateCorrectionSelection();
    void setComputeCorrectionEnable();

    void setRunAnnalysisEnable(bool enabled);
    std::map < std::string, boost::program_options::variable_value > getRunOptionMap();
    std::map < std::string, boost::program_options::variable_value > getLuminosityOptionMap();
    std::map<std::string, LuminosityPriorConfig> m_prior_config;
    std::map<std::string,std::string> m_copied_columns = {};


    DatasetRepo m_filter_repository;
    DatasetRepo m_luminosity_repository;
    std::shared_ptr<SurveyModel> m_survey_model_ptr;
    std::shared_ptr<ModelSetModel> m_model_set_model_ptr;

};

}
}
#endif // FORMANALYSIS_H
