
#include <QMessageBox>

#include "PhzQtUI/DialogFilterSelector.h"
#include "ui_DialogFilterSelector.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogFilterSelector::DialogFilterSelector(DatasetRepo filter_repository, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFilterSelector){
  ui->setupUi(this);
  m_filter_repository = filter_repository;
}

DialogFilterSelector::~DialogFilterSelector(){}

void DialogFilterSelector::setFilter(std::string filter_name){
  //// Setup the DataSetTreeModel withthe filters
  string path_filter = FileUtils::getFilterRootPath(true);
  DataSetTreeModel* treeModel_filter = new DataSetTreeModel(m_filter_repository);
  treeModel_filter->load(true,true);
  treeModel_filter->setEnabled(true);
  ui->treeView_filter->setModel(treeModel_filter);
  ui->treeView_filter->expandAll();

  connect( treeModel_filter,
           SIGNAL(itemChanged(QStandardItem*)),
           treeModel_filter,
           SLOT(onItemChangedSingleLeaf(QStandardItem*)));
  // set the current selected filter
  if (filter_name.length()>0){
    std::vector<std::string> filterList{filter_name};
    treeModel_filter->setState(filterList);
  }

  if (!treeModel_filter->item(0,0)->hasChildren()){
      QMessageBox::warning(this, "No available filter...",
              "There is no filter transmission curve to select. "
              "You can provide and manage filter transmission curves in the \"Configuration/Aux. Data\" page.",
              QMessageBox::Ok);
    }
}

void DialogFilterSelector::on_btn_save_clicked(){
  auto filter = static_cast<DataSetTreeModel*>(ui->treeView_filter->model())->getSelectedLeaves();
  if (filter.size()==0 ){
    QMessageBox::warning( this,
                          "Missing Data...",
                          "Please select a filter.",
                          QMessageBox::Ok );
    return;
  }

  // return the selected filter to the caller
  popupClosing(filter[0]);
  accept();
}

void DialogFilterSelector::on_btn_cancel_clicked(){
  reject();
}

}
}
