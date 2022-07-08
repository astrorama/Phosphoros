
#include <QMessageBox>

#include "FileUtils.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "PhzQtUI/DialogSedSelector.h"
#include "ui_DialogSedSelector.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {

DialogSedSelector::DialogSedSelector(DatasetRepo sed_repository, bool single_sed, QWidget* parent)
    : m_single_sed(single_sed), QDialog(parent), ui(new Ui::DialogSedSelector) {
  ui->setupUi(this);
  m_sed_repository = sed_repository;
}

DialogSedSelector::~DialogSedSelector() {}

void DialogSedSelector::setSed(std::string sed_names) {
  //// Setup the DataSetTreeModel withthe filters
  string            path_sed      = FileUtils::getSedRootPath(true);
  DataSetTreeModel* treeModel_sed = new DataSetTreeModel(m_sed_repository);
  treeModel_sed->load(true, m_single_sed);
  treeModel_sed->setEnabled(true);
  ui->treeView_sed->setModel(treeModel_sed);
  ui->treeView_sed->hideColumn(1);
  ui->treeView_sed->hideColumn(2);
  ui->treeView_sed->resizeColumnToContents(0);

  if (m_single_sed) {
    ui->treeView_sed->expandAll();
    connect(treeModel_sed, SIGNAL(itemChanged(QStandardItem*)), treeModel_sed,
            SLOT(onItemChangedSingleLeaf(QStandardItem*)));
  } else {
    connect(treeModel_sed, SIGNAL(itemChanged(QStandardItem*)), treeModel_sed, SLOT(onItemChanged(QStandardItem*)));
    connect(treeModel_sed, SIGNAL(onItemChanged(QStandardItem*)), treeModel_sed,
            SLOT(onItemChangedSingleLeaf(QStandardItem*)));
  }

  // set the current selected filter
  if (sed_names.size() > 0) {
    treeModel_sed->setState({sed_names});
  }

  if (!treeModel_sed->hasLeave()) {
    QMessageBox::warning(this, "No available SED...",
                         "There is no SED to select. "
                         "You can provide and manage SED in the \"Configuration/Aux. Data\" page.",
                         QMessageBox::Ok);
  }
}

void DialogSedSelector::on_btn_save_clicked() {
  auto seds = static_cast<DataSetTreeModel*>(ui->treeView_sed->model())->getSelectedLeaves();
  if (seds.size() == 0) {
    QMessageBox::warning(this, "Missing Data...", "Please select a SED.", QMessageBox::Ok);
    return;
  }

  // return the selected filter to the caller
  popupClosing(seds);
  accept();
}

void DialogSedSelector::on_btn_cancel_clicked() {
  reject();
}

}  // namespace PhzQtUI
}  // namespace Euclid
