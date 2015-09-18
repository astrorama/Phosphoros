
#include <QMessageBox>

#include "PhzQtUI/DialogLuminosityFunctionCurveSelector.h"
#include "ui_DialogLuminosityFunctionCurveSelector.h"
#include "PhzQtUI/XYDataSetTreeModel.h"
#include "FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogLuminosityFunctionCurveSelector::DialogLuminosityFunctionCurveSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLuminosityFunctionCurveSelector){
  ui->setupUi(this);
}

DialogLuminosityFunctionCurveSelector::~DialogLuminosityFunctionCurveSelector(){}


void DialogLuminosityFunctionCurveSelector::setCurve(std::string curve_name){
  string path_curve = FileUtils::getLuminosityFunctionCurveRootPath(true);

  XYDataSetTreeModel* treeModel_curve = new XYDataSetTreeModel();
  treeModel_curve->loadDirectory(path_curve,true,"Filters");
  treeModel_curve->setEnabled(true);
  ui->treeView_filter->setModel(treeModel_curve);
  ui->treeView_filter->expandAll();

  connect( treeModel_curve,
           SIGNAL(itemChanged(QStandardItem*)),
           treeModel_curve,
           SLOT(onItemChangedSingleLeaf(QStandardItem*)));

  if (curve_name.length()>0){
    treeModel_curve->setState(curve_name,vector<string>());
  }
}


void DialogLuminosityFunctionCurveSelector::on_btn_save_clicked(){
  auto curve_res = static_cast<XYDataSetTreeModel*>(ui->treeView_filter->model())->getRootSelection();
  if (!curve_res.first){
    QMessageBox::warning( this,
                          "Missing Data...",
                          "Please select a curve.",
                          QMessageBox::Ok );
    return;
  }

  // return the curve to the caller
  popupClosing(curve_res.second);
  accept();
}

void DialogLuminosityFunctionCurveSelector::on_btn_cancel_clicked(){
  reject();
}

}
}
