#include <QMessageBox>
#include <QtGui/qdesktopservices.h>
#include <QtCore/qurl.h>
#include <boost/filesystem/path.hpp>

#include "PhzQtUI/FormAuxDataManagement.h"
#include "ui_FormAuxDataManagement.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "PhzQtUI/DialogImportAuxData.h"
#include "PhzQtUI/DialogCreatesSubGroup.h"
#include "FileUtils.h"
#include "XYDataset/AsciiParser.h"


namespace Euclid {
namespace PhzQtUI {


FormAuxDataManagement::FormAuxDataManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuxDataManagement)
{
    ui->setupUi(this);
}

FormAuxDataManagement::~FormAuxDataManagement()
{
}

void FormAuxDataManagement::setRepositories(DatasetRepo filter_repository,
                                            DatasetRepo seds_repository,
                                            DatasetRepo redenig_curves_repository,
                                            DatasetRepo luminosity_repository){
  m_filter_repository = filter_repository;
  m_seds_repository=seds_repository;
  m_redenig_curves_repository=redenig_curves_repository;
  m_luminosity_repository = luminosity_repository;
}


void FormAuxDataManagement::loadManagementPage(int index){

    DataSetTreeModel* treeModel_filter = new DataSetTreeModel(m_filter_repository);
    treeModel_filter->load(false);
    treeModel_filter->setEnabled(true);
    ui->treeView_ManageFilter->setModel(treeModel_filter);
    ui->treeView_ManageFilter->collapseAll();

    DataSetTreeModel* treeModel_Sed = new DataSetTreeModel(m_seds_repository);
    treeModel_Sed->load(false);
    treeModel_Sed->setEnabled(true);
    ui->treeView_ManageSed->setModel(treeModel_Sed);
    ui->treeView_ManageSed->collapseAll();

    DataSetTreeModel* treeModel_Red = new DataSetTreeModel(m_redenig_curves_repository);
    treeModel_Red->load(false);
    treeModel_Red->setEnabled(true);
    ui->treeView_ManageRed->setModel(treeModel_Red);
    ui->treeView_ManageRed->collapseAll();


    DataSetTreeModel* treeModel_Luminosity = new DataSetTreeModel(m_luminosity_repository);
    treeModel_Luminosity->load(false);
    treeModel_Luminosity->setEnabled(true);
    ui->treeView_ManageLuminosity->setModel(treeModel_Luminosity);
    ui->treeView_ManageLuminosity->collapseAll();


    if (index>=0){
      ui->tab_Management->setCurrentIndex(index);
    }
}

namespace {

void openAuxFilesDir(const std::string& dir) {
  auto path_map = FileUtils::readPath();
  boost::filesystem::path path = path_map["AuxiliaryData"];
  path = path / dir;
  QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(path.string())));
}

}

void FormAuxDataManagement::on_btn_ShowFilesFilter_clicked() {
  openAuxFilesDir("Filters");
}

void FormAuxDataManagement::on_btn_ShowFilesSed_clicked() {
  openAuxFilesDir("SEDs");
}

void FormAuxDataManagement::on_btn_ShowFilesRedCurve_clicked() {
  openAuxFilesDir("ReddeningCurves");
}

void FormAuxDataManagement::on_btn_ShowFilesLumFunc_clicked() {
  openAuxFilesDir("LuminosityFunctionCurves");
}

}
}



/* Copied from Dialog configuration: to be used as template here
void DialogModelSet::addButtonsToSedItem(QStandardItem* item, SedTreeModel* treeModel_sed){
  if (treeModel_sed->canAddEmissionLineToGroup(item)) {
             auto name = treeModel_sed->getFullGroupName(item);

             MessageButton *cartButton = new MessageButton(name, "Add Emission Line to SEDs");
             m_message_buttons.push_back(cartButton);

             auto index = item->index().sibling(item->index().row(), 1);

             ui->treeView_Sed->setIndexWidget(index, cartButton);

             connect(cartButton, SIGNAL(MessageButtonClicked(const QString&)), this,
                             SLOT(addEmissionLineButtonClicked(const QString&)));
  }

  for (int i = 0; i < item->rowCount(); i++) {
    addButtonsToSedItem(item->child(i),treeModel_sed);
  }
}*/
