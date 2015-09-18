
#include <QMessageBox>

#include "PhzQtUI/DialogFilterSelector.h"
#include "ui_DialogFilterSelector.h"
#include "PhzQtUI/XYDataSetTreeModel.h"
#include "FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogFilterSelector::DialogFilterSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFilterSelector){
  ui->setupUi(this);
}

DialogFilterSelector::~DialogFilterSelector(){}

void DialogFilterSelector::setFilter(std::string filter_name){
  //// Setup the XYDataSetTreeModel withthe filters
  string path_filter = FileUtils::getFilterRootPath(true);
  XYDataSetTreeModel* treeModel_filter = new XYDataSetTreeModel();
  treeModel_filter->loadDirectory(path_filter,true,"Filters");
  treeModel_filter->setEnabled(true);
  ui->treeView_filter->setModel(treeModel_filter);
  ui->treeView_filter->expandAll();

  connect( treeModel_filter,
           SIGNAL(itemChanged(QStandardItem*)),
           treeModel_filter,
           SLOT(onItemChangedSingleLeaf(QStandardItem*)));
  // set the current selected filter
  if (filter_name.length()>0){
    treeModel_filter->setState(filter_name,vector<string>());
  }
}

void DialogFilterSelector::on_btn_save_clicked(){
  auto filter_res = static_cast<XYDataSetTreeModel*>(ui->treeView_filter->model())->getRootSelection();
  if (!filter_res.first){
    QMessageBox::warning( this,
                          "Missing Data...",
                          "Please select a filter.",
                          QMessageBox::Ok );
    return;
  }

  // return the selected filter to the caller
  popupClosing(filter_res.second);
  accept();
}

void DialogFilterSelector::on_btn_cancel_clicked(){
  reject();
}

}
}
