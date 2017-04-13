
#include <QMessageBox>

#include "PhzQtUI/DialogFilterMapping.h"
#include "ui_DialogFilterMapping.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "FileUtils.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


DialogFilterMapping::DialogFilterMapping(DatasetRepo filter_repository, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFilterMapping){
  ui->setupUi(this);
  m_filter_repository = filter_repository;
}

DialogFilterMapping::~DialogFilterMapping(){}

void DialogFilterMapping::setFilters(const std::vector<std::string>& filters){

   //// Load the DataSetTreeModel with the filters
   DataSetTreeModel* treeModel_filter = new DataSetTreeModel(m_filter_repository);
   treeModel_filter->load(true);
   treeModel_filter->setEnabled(true);
   connect(treeModel_filter, SIGNAL(itemChanged(QStandardItem*)), treeModel_filter, SLOT(onItemChanged(QStandardItem*)));
   ui->treeView_filter->setModel(treeModel_filter);
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
  popupClosing(static_cast<DataSetTreeModel*>(ui->treeView_filter->model())->getSelectedLeaves());
  accept();
}

void DialogFilterMapping::on_btn_cancel_clicked(){
    reject();
}

}
}
