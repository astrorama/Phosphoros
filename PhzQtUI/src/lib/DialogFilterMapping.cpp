
#include <QMessageBox>

#include "PhzQtUI/DialogFilterMapping.h"
#include "ui_DialogFilterMapping.h"
#include "PhzQtUI/XYDataSetTreeModel.h"
#include "FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogFilterMapping::DialogFilterMapping(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFilterMapping){
  ui->setupUi(this);
}

DialogFilterMapping::~DialogFilterMapping(){}

void DialogFilterMapping::setFilters(const std::vector<std::string>& filters){

  //// Load the XYDataSetTreeModel with the filters
  string path_filter = FileUtils::getFilterRootPath(true);
  XYDataSetTreeModel* treeModel_filter = new XYDataSetTreeModel();
  treeModel_filter->loadDirectory(path_filter,true,"Filters");
  treeModel_filter->setEnabled(true);
  ui->treeView_filter->setModel(treeModel_filter);
 // ui->treeView_filter->expand(ui->treeView_filter->model()->index(0,0));
  ui->treeView_filter->expandAll();

  treeModel_filter->setState(filters);

  if (treeModel_filter->rowCount()==0){
      QMessageBox::warning(this, "No available filter...",
              "There is no filter transmission curve to select. "
              "You can provide and manage filter transmission curves in the \"Configuration/Aux. Data\" page.",
              QMessageBox::Ok);
    }
}


void DialogFilterMapping::on_btn_save_clicked(){
  popupClosing(static_cast<XYDataSetTreeModel*>(ui->treeView_filter->model())->getSelectedLeaves());
  accept();
}

void DialogFilterMapping::on_btn_cancel_clicked(){
    reject();
}

}
}
