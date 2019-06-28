#include <QMessageBox>
#include <QSignalMapper>
#include <QHBoxLayout>
#include <QPushButton>
#include "PhzQtUI/FormPostProcessing.h"

#include "PhzQtUI/ResultModel.h"
#include "PhzQtUI/ResultRunModel.h"
#include "PhzQtUI/DialogPOP.h"
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
    ResultModel* result_model = new ResultModel();
    ui->table_res_cat->setModel(result_model);
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



void FormPostProcessing::on_btn_refresh_clicked(){
  disconnect(ui->table_res_cat->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), 0, 0);

  (static_cast < ResultModel* >(ui->table_res_cat->model()))->load();
  ui->table_res_cat->setColumnHidden(2, true);
  ui->table_res_cat->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->table_res_cat->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->table_res_cat->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  connect(
        ui->table_res_cat->selectionModel(),
        SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
        SLOT(catalogSelectionChanged(QModelIndex, QModelIndex)));

}


void FormPostProcessing::catalogSelectionChanged(QModelIndex new_index, QModelIndex){
  if (new_index.isValid()) {
      std::string path = ui->table_res_cat->model()->data(ui->table_res_cat->model()->index(new_index.row(), 2)).toString().toStdString();

     (static_cast < ResultRunModel* >(ui->table_res_file->model()))->load(path);
    } else {
      (static_cast < ResultRunModel* >(ui->table_res_file->model()))->load("");
    }
  ui->table_res_file->setColumnHidden(2, true);
  ui->table_res_file->setColumnHidden(3, true);
  ui->table_res_file->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->table_res_file->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->table_res_file->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  QHeaderView *verticalHeader =  ui->table_res_file->verticalHeader();
  verticalHeader->setResizeMode(QHeaderView::Fixed);
  verticalHeader->setDefaultSectionSize(45);


  QSignalMapper *signalMapper_one = new QSignalMapper(this);
  QSignalMapper *signalMapper_two = new QSignalMapper(this);

  for( int i=0; i<ui->table_res_file->model()->rowCount(); i++ ) {

       //make new button for this row
       QModelIndex index = ui->table_res_file->model()->index(i, 1);

       QWidget *widget = new QWidget();
       QHBoxLayout *layout = new QHBoxLayout;
       QPushButton *button1 = new QPushButton("PDF stat");

       QPushButton *button2 = new QPushButton("Plot Z against Zref");
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

void FormPostProcessing::computePdfStat(int row){
  auto folder = ui->table_res_file->model()->data(ui->table_res_file->model()->index(row,2)).toString().toStdString();
  std::unique_ptr<DialogPOP> dialog(new DialogPOP());
  dialog->setFolder(folder);
  if (dialog->exec()) {

  }
}

void FormPostProcessing::plotZVsZref(int){

}




}
}
