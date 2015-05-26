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

namespace boost{
namespace program_options{
 class variable_value;
}
}

namespace Euclid {
namespace PhzQtUI {

namespace Ui {
class FormAnalysis;
}

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
    void loadAnalysisPage();

signals:
    void navigateToHome();

private slots:
    void on_btn_AnalysisToHome_clicked();

    void on_btn_backHome_clicked();

    void on_btn_editCorrections_clicked();

    void on_cb_AnalysisCorrection_currentIndexChanged(const QString &arg1);

    void on_cb_AnalysisSurvey_currentIndexChanged(const QString &arg1);

    void onFilterSelectionItemChanged(QStandardItem*);

    void on_btn_computeCorrections_clicked();

    void on_cb_AnalysisModel_currentIndexChanged(const QString &);

    void on_cb_igm_currentIndexChanged(const QString &);

    void on_cb_CompatibleGrid_textChanged(const QString &);

    void on_cb_cat_output_type_currentIndexChanged(const QString &);

    void on_btn_GetConfigGrid_clicked();

    void on_btn_RunGrid_clicked();

    void on_gb_corrections_clicked();

    void onCorrectionComputed(const QString &);

    void on_btn_BrowseInput_clicked();

    void on_btn_BrowseOutput_clicked();

    void on_btn_GetConfigAnalysis_clicked();

    void on_btn_RunAnalysis_clicked();

    void on_btn_BrowseOutputPdf_clicked();

    void on_gb_cat_clicked();

    void on_gb_pdf_clicked();

    void on_gb_lik_clicked();

    void on_btn_BrowseLikelihood_clicked();


private:
    std::unique_ptr<Ui::FormAnalysis> ui;
    std::list<std::string> getSelectedFilters(bool return_path=false);
    std::list<FilterMapping> getSelectedFilterMapping();

    std::string getSelectedSurveySourceColumn();
    void adjustPhzGridButtons(bool enabled);

    void updateGridSelection();
    bool checkGridSelection(bool addFileCheck, bool acceptNewFile);
    std::map<std::string, boost::program_options::variable_value> getGridConfiguration();


    static void setToolBoxButtonColor(QToolBox* toolBox, int index, QColor color);

    void updateCorrectionSelection();
    void setComputeCorrectionEnable();

    void setRunAnnalysisEnable(bool enabled);
    std::map < std::string, boost::program_options::variable_value > getRunOptionMap();
    std::map<int,SurveyFilterMapping>  m_analysis_survey_list;
    std::map<int,ModelSet> m_analysis_model_list;

};

}
}
#endif // FORMANALYSIS_H
