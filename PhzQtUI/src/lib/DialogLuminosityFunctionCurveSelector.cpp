
#include <QMessageBox>

#include "PhzQtUI/DialogLuminosityFunctionCurveSelector.h"
#include "ui_DialogLuminosityFunctionCurveSelector.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogLuminosityFunctionCurveSelector::DialogLuminosityFunctionCurveSelector(
    DatasetRepo luminosity_repository, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLuminosityFunctionCurveSelector){
  ui->setupUi(this);
  m_luminosity_repository = luminosity_repository;
}

DialogLuminosityFunctionCurveSelector::~DialogLuminosityFunctionCurveSelector(){}


void DialogLuminosityFunctionCurveSelector::setCurve(std::string curve_name){
  string path_curve = FileUtils::getLuminosityFunctionCurveRootPath(true);

  DataSetTreeModel* treeModel_curve = new DataSetTreeModel(m_luminosity_repository);
  treeModel_curve->load(true,true);
  treeModel_curve->setEnabled(true);
  ui->treeView_filter->setModel(treeModel_curve);
  ui->treeView_filter->expandAll();

  connect( treeModel_curve,
           SIGNAL(itemChanged(QStandardItem*)),
           treeModel_curve,
           SLOT(onItemChangedSingleLeaf(QStandardItem*)));



  if (!treeModel_curve->hasLeave()){
    QMessageBox::warning(this, "No available function...",
            "There is no luminosity function curve to select. "
            "You can provide and manage luminosity function curves in the \"Configuration/Aux. Data\" page.",
            QMessageBox::Ok);
  } else if (curve_name.length()>0){
    std::vector<std::string> curveList{curve_name};
    treeModel_curve->setState(curveList);
  }
}


void DialogLuminosityFunctionCurveSelector::on_btn_save_clicked(){
  auto curve = static_cast<DataSetTreeModel*>(ui->treeView_filter->model())->getSelectedLeaves();
  if (curve.size()==0){
    QMessageBox::warning( this,
                          "Missing Data...",
                          "Please select a curve.",
                          QMessageBox::Ok );
    return;
  }

  // return the curve to the caller
  popupClosing(curve[0]);
  accept();
}

void DialogLuminosityFunctionCurveSelector::on_btn_cancel_clicked(){
  reject();
}

}
}
