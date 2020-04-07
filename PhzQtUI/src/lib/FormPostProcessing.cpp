#include <QMessageBox>
#include <QSignalMapper>
#include <QHBoxLayout>
#include <QPushButton>
#include "PhzQtUI/FormPostProcessing.h"

#include "PhzQtUI/ResultModel.h"
#include "PhzQtUI/ResultRunModel.h"
#include "PhzQtUI/DialogPOP.h"
#include "PhzQtUI/DialogPSC.h"
#include "ui_FormPostProcessing.h"
#include "FileUtils.h"

#include "ElementsKernel/Exception.h"
#include "PreferencesUtils.h"
#include "ElementsKernel/Logging.h"


namespace po = boost::program_options;

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("FormPostProcessing");

FormPostProcessing::FormPostProcessing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPostProcessing) {
    ui->setupUi(this);
    ResultRunModel* result_run_model = new ResultRunModel();
    ui->table_res_file->setModel(result_run_model);
}

FormPostProcessing::~FormPostProcessing() {}



//  - Slots on this page
void FormPostProcessing::on_btn_ToAnalysis_clicked() {
  navigateToComputeRedshift(false);
}
void FormPostProcessing::on_btn_ToOption_clicked() {
  navigateToConfig();
}
void FormPostProcessing::on_btn_ToCatalog_clicked() {
  navigateToCatalog(false);
}

void FormPostProcessing::on_btn_ToModel_clicked(){
  navigateToParameter(false);
}

void FormPostProcessing::on_btn_exit_clicked() {
  quit(true);
}


void FormPostProcessing::loadPostProcessingPage(std::shared_ptr<SurveyModel> survey_model_ptr){
      logger.info()<< "Load the PostProcessing Page";
      m_survey_model_ptr = survey_model_ptr;
      updateSelection(true);
}


void FormPostProcessing::loadCbCatalog(std::string selected){
  m_disconnect_cb = true;
  ui->cb_catalog->clear();
  int index = 0;
  for (auto& catalog_name : m_survey_model_ptr->getSurveyList()) {
         ui->cb_catalog->addItem(catalog_name);
         if (selected == catalog_name.toStdString()) {
             ui->cb_catalog->setCurrentIndex(index);
         }
         ++index;
  }
  m_disconnect_cb = false;
}


void FormPostProcessing::updateSelection(bool force_reload_cb){
  logger.info() << "PostProcessing catalog selected " << m_survey_model_ptr->getSelectedSurvey().getName();

  if (force_reload_cb || (m_survey_model_ptr->getSelectedRow()>=0 &&
          ui->cb_catalog->currentText().toStdString() != m_survey_model_ptr->getSelectedSurvey().getName())) {
    loadCbCatalog(m_survey_model_ptr->getSelectedSurvey().getName());
  }

  std::string path = FileUtils::getResultRootPath(false, m_survey_model_ptr->getSelectedSurvey().getName(), "");
  (static_cast < ResultRunModel* >(ui->table_res_file->model()))->load(path);
  ui->table_res_file->setColumnHidden(2, true);
  ui->table_res_file->setColumnHidden(3, true);
  ui->table_res_file->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->table_res_file->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->table_res_file->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  QHeaderView *verticalHeader =  ui->table_res_file->verticalHeader();
  verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
  verticalHeader->setDefaultSectionSize(45);


  QSignalMapper *signalMapper_one = new QSignalMapper(this);
  QSignalMapper *signalMapper_two = new QSignalMapper(this);

  for( int i=0; i<ui->table_res_file->model()->rowCount(); i++ ) {

      //make new button for this row
      QModelIndex index = ui->table_res_file->model()->index(i, 1);

      QWidget *widget = new QWidget();
      QHBoxLayout *layout = new QHBoxLayout;
      QPushButton *button1 = new QPushButton("PDF stat");

      QPushButton *button2 = new QPushButton("Plots");
      layout->addWidget(button1);
      layout->addWidget(button2);

      widget->setLayout(layout);

      ui->table_res_file->setIndexWidget(index, widget);

      signalMapper_one->setMapping(button1, i);
      connect(button1, SIGNAL(clicked(bool)), signalMapper_one, SLOT(map()));

      signalMapper_two->setMapping(button2, i);
      connect(button2, SIGNAL(clicked(bool)), signalMapper_two, SLOT(map()));
  }

  connect(signalMapper_one, SIGNAL(mapped(int)), this, SLOT(computePdfStat(int)));
  connect(signalMapper_two, SIGNAL(mapped(int)), this, SLOT(plotZVsZref(int)));


}



void FormPostProcessing::on_cb_catalog_currentIndexChanged(const QString &){
  if (!m_disconnect_cb) {
            logger.info()<< "Selected Catalog changed to:" << ui->cb_catalog->currentText().toStdString();
            m_survey_model_ptr->selectSurvey(ui->cb_catalog->currentText());
            updateSelection();
  }
}




void FormPostProcessing::computePdfStat(int row){
  auto folder = ui->table_res_file->model()->data(ui->table_res_file->model()->index(row,2)).toString().toStdString();
  std::unique_ptr<DialogPOP> dialog(new DialogPOP());
  dialog->setFolder(folder);
  if (dialog->exec()) {

  }
}

void FormPostProcessing::plotZVsZref(int row){
    auto folder = ui->table_res_file->model()->data(ui->table_res_file->model()->index(row,2)).toString().toStdString();
    std::unique_ptr<DialogPSC> dialog(new DialogPSC());
    dialog->setDefaultColumn(m_survey_model_ptr->getSelectedSurvey().getSourceIdColumn(), m_survey_model_ptr->getSelectedSurvey().getRefZColumn());
    dialog->setFolder(folder);
    if (dialog->exec()) {

    }
}




}
}