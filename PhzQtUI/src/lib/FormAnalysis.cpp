#include <QStandardItemModel>
#include "PhzQtUI/FormAnalysis.h"
#include "ui_FormAnalysis.h"
#include "PhzQtUI/DialogPhotCorrectionEdition.h"
#include "PhzQtUI/DialogPhotometricCorrectionComputation.h"
#include "PhzQtUI/FileUtils.h"
#include "PhzQtUI/ModelSet.h"
#include "PhzQtUI/PhotometricCorrectionHandler.h"
#include "PhzQtUI/SurveyFilterMapping.h"
#include "PhzQtUI/PhzGridInfoHandler.h"

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
   m_analysis_survey_list =  Euclid::PhosphorosUiDm::SurveyFilterMapping::loadSurveysFromFolder(Euclid::PhosphorosUiDm::FileUtils::getMappingRootPath(false));

   ui->cb_AnalysisSurvey->clear();
   for(auto& survey:m_analysis_survey_list){
       ui->cb_AnalysisSurvey->addItem(QString::fromStdString(survey.second.getName()));
   }

    m_analysis_model_list = Euclid::PhosphorosUiDm::ModelSet::loadModelSetsFromFolder(Euclid::PhosphorosUiDm::FileUtils::getModelRootPath(false));
    ui->cb_AnalysisModel->clear();
    for(auto& model:m_analysis_model_list){
        ui->cb_AnalysisModel->addItem(QString::fromStdString(model.second.getName()));
    }

    // TODO

}


std::map<std::string,bool> FormAnalysis::getSelectedFilters(bool return_path){
    std::map<std::string,bool> res;
    auto model = static_cast<QStandardItemModel*>(ui->tableView_filter->model());
    for (int i=0; i<model->rowCount();++i ){
        auto item = model->item(i);
        bool checked = item->checkState()==Qt::CheckState::Checked;

        res[model->item(i,return_path)->text().toStdString()]=checked;
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

    Euclid::PhosphorosUiDm::SurveyFilterMapping selected_survey;

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
        QStandardItem* file_item = new QStandardItem(QString::fromStdString(filter.getFilterFile()));

        QList<QStandardItem*> items{{item,file_item}};
        grid_model->appendRow(items);
    }

    ui->tableView_filter->setModel(grid_model);
   // ui->tableView_filter->setColumnHidden(1, true);
    connect( grid_model, SIGNAL(itemChanged(QStandardItem*)),
                 SLOT(onFilterSelectionItemChanged(QStandardItem*)));
}

void FormAnalysis::on_cb_AnalysisModel_currentIndexChanged(const QString &selectedName)
{
     Euclid::PhosphorosUiDm::ModelSet selected_model;

     for(auto&model:m_analysis_model_list){
         if (model.second.getName().compare(selectedName.toStdString())==0){
           selected_model=model.second;
             break;
         }
     }

     auto axis = Euclid::PhosphorosUiDm::PhzGridInfoHandler::getAxesTuple(selected_model);
}


void FormAnalysis::onFilterSelectionItemChanged(QStandardItem* affectedItem){
    // TODO change the GRID and the corrections file
    affectedItem->checkState();
    affectedItem->text();

    auto filter_map = getSelectedFilters(true);
    auto file_list = Euclid::PhosphorosUiDm::PhotometricCorrectionHandler::getCompatibleCorrectionFiles(filter_map);
    ui->cb_AnalysisCorrection->clear();

    for (auto file : file_list){
        ui->cb_AnalysisCorrection->addItem(QString::fromStdString(file));
    }
}


void FormAnalysis::on_btn_computeCorrections_clicked()
{
    DialogPhotometricCorrectionComputation* popup = new DialogPhotometricCorrectionComputation();
    popup->setData(ui->cb_AnalysisSurvey->currentText().toStdString(),
                   ui->cb_AnalysisModel->currentText().toStdString(),
                   ui->cb_CompatibleGrid->currentText().toStdString(),
                   getSelectedFilters());
    popup->exec();
}



