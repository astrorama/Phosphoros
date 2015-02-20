#include <QStandardItemModel>
#include "QtUI/FormAnalysis.h"
#include "ui_FormAnalysis.h"
#include "QtUI/DialogPhotCorrectionEdition.h"
#include "QtUI/DialogPhotometricCorrectionComputation.h"
#include "QtUI/FileUtils.h"
#include "QtUI/ModelSet.h"

FormAnalysis::FormAnalysis(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAnalysis)
{
    ui->setupUi(this);
}

FormAnalysis::~FormAnalysis()
{
    delete ui;
}

void FormAnalysis::loadAnalysisPage(){
   m_analysis_survey_list = PhosphorosUiDm::SurveyFilterMapping::loadSurveysFromFolder(FileUtils::getMappingRootPath(false));

   ui->cb_AnalysisSurvey->clear();
   for(auto& survey:m_analysis_survey_list){
       ui->cb_AnalysisSurvey->addItem(QString::fromStdString(survey.second.getName()));
   }

    auto models = PhosphorosUiDm::ModelSet::loadModelSetsFromFolder(FileUtils::getModelRootPath(false));
    ui->cb_AnalysisModel->clear();
    for(auto& model:models){
        ui->cb_AnalysisModel->addItem(QString::fromStdString(model.second.getName()));
    }

    // TODO

}


std::map<std::string,bool> FormAnalysis::getSelectedFilters(){
    std::map<std::string,bool> res;
    auto model = static_cast<QStandardItemModel*>(ui->listView_filter->model());
    for (int i=0; i<model->rowCount();++i ){
        auto item = model->item(i);
        bool checked = item->checkState()==Qt::CheckState::Checked;
        res[item->text().toStdString()]=checked;
    }

    return res;
}


//  - Slot on this page
void FormAnalysis::on_btn_AnalysisToHome_clicked()
{
      navigateToHome();
}

void FormAnalysis::on_btn_editCorrections_clicked()
{
    DialogPhotCorrectionEdition* popup = new DialogPhotCorrectionEdition();
    popup->setCorrectionsFile(ui->cb_AnalysisCorrection->currentText().toStdString());
    popup->exec();

}

void FormAnalysis::on_cb_AnalysisCorrection_currentIndexChanged(const QString &selectedText)
{
    ui->btn_editCorrections->setEnabled(selectedText.length()>0);
}

void FormAnalysis::on_cb_AnalysisSurvey_currentIndexChanged(const QString &selectedName)
{

    PhosphorosUiDm::SurveyFilterMapping selected_survey;

    for(auto&survey:m_analysis_survey_list){
        if (survey.second.getName().compare(selectedName.toStdString())==0){
            selected_survey=survey.second;
            break;
        }
    }

    QStandardItemModel* grid_model = new QStandardItemModel();
    grid_model->setColumnCount(1);

    for (auto filter : selected_survey.getFilters()){
        QStandardItem* item = new QStandardItem(QString::fromStdString(filter.getName()));
        item->setCheckable(true);
        item->setCheckState(Qt::CheckState::Checked);
        QList<QStandardItem*> items{{item}};
        grid_model->appendRow(items);
    }

    ui->listView_filter->setModel(grid_model);
    connect( grid_model, SIGNAL(itemChanged(QStandardItem*)),
                 SLOT(onFilterSelectionItemChanged(QStandardItem*)));
}


void FormAnalysis::onFilterSelectionItemChanged(QStandardItem* affectedItem){
    // TODO change the GRID and the corrections fie
    affectedItem->checkState();
    affectedItem->text();
}

void FormAnalysis::on_btn_computeCorrections_clicked()
{
    DialogPhotometricCorrectionComputation* popup = new DialogPhotometricCorrectionComputation();
    popup->setData(ui->cb_AnalysisSurvey->currentText().toStdString(),
                   ui->cb_AnalysisModel->currentText().toStdString(),
                   ui->txt_CompatibleGrid->text().toStdString(),
                   getSelectedFilters());
    popup->exec();
}
