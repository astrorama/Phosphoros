#ifndef FORMANALYSIS_H
#define FORMANALYSIS_H

#include <QWidget>
#include <QStandardItem>
#include "SurveyFilterMapping.h"
#include "ModelSet.h"

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

    void on_cb_AnalysisModel_currentIndexChanged(const QString &arg1);

private:
    Ui::FormAnalysis *ui;
    std::map<std::string,bool> getSelectedFilters(bool return_path=false);
    std::map<int,Euclid::PhosphorosUiDm::SurveyFilterMapping>  m_analysis_survey_list;
    std::map<int,Euclid::PhosphorosUiDm::ModelSet> m_analysis_model_list;

};

#endif // FORMANALYSIS_H
