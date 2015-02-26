#ifndef FORMANALYSIS_H
#define FORMANALYSIS_H

#include <QWidget>
#include <QStandardItem>
#include <map>
#include <string>
#include <boost/program_options.hpp>
#include "SurveyFilterMapping.h"
#include "ModelSet.h"

namespace po = boost::program_options;

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

    void on_btn_editCorrections_clicked();

    void on_cb_AnalysisCorrection_currentIndexChanged(const QString &arg1);

    void on_cb_AnalysisSurvey_currentIndexChanged(const QString &arg1);

    void onFilterSelectionItemChanged(QStandardItem*);

    void on_btn_computeCorrections_clicked();

    void on_cb_AnalysisModel_currentIndexChanged(const QString &);

    void on_cb_CompatibleGrid_textChanged(const QString &);

    void on_btn_GetConfigGrid_clicked();

    void on_btn_RunGrid_clicked();

    void on_gb_corrections_clicked();

    void onCorrectionComputed(const std::string &);

private:
    Ui::FormAnalysis *ui;
    std::list<std::string> getSelectedFilters(bool return_path=false);
    std::list<Euclid::PhosphorosUiDm::FilterMapping> getSelectedFilterMapping();

    void enableDisablePage(bool enabled);
    void adjustPhzGridButtons(bool enabled);

    void updateGridSelection();
    bool checkGridSelection(bool addFileCheck, bool acceptNewFile);
    std::map<std::string, po::variable_value> getGridConfiguration();

    void updateGridProgressBar(size_t step, size_t total);


    void updateCorrectionSelection();
    void setComputeCorrectionEnable();

    void setRunAnnalysisEnable();
    std::map<int,Euclid::PhosphorosUiDm::SurveyFilterMapping>  m_analysis_survey_list;
    std::map<int,Euclid::PhosphorosUiDm::ModelSet> m_analysis_model_list;

};

#endif // FORMANALYSIS_H
