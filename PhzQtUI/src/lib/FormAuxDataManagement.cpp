#include <QMessageBox>

#include "PhzQtUI/FormAuxDataManagement.h"
#include "ui_FormAuxDataManagement.h"
#include "PhzQtUI/DirectoryTreeModel.h"
#include "PhzQtUI/DialogImportAuxData.h"
#include "PhzQtUI/DialogCreatesSubGroup.h"
#include "FileUtils.h"


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


void FormAuxDataManagement::loadManagementPage(int index){

    DirectoryTreeModel* treeModel_filter = new DirectoryTreeModel();
    treeModel_filter->loadDirectory(FileUtils::getFilterRootPath(true),false, "Filters");
    treeModel_filter->selectRoot();
    treeModel_filter->setEnabled(true);

    ui->treeView_ManageFilter->setModel(treeModel_filter);
    ui->treeView_ManageFilter->collapseAll();
    ui->treeView_ManageFilter->expand( treeModel_filter->item(0,0)->index());
    connect( treeModel_filter, SIGNAL(itemChanged(QStandardItem*)), treeModel_filter,
                 SLOT(onItemChangedUniqueSelection(QStandardItem*)));

    DirectoryTreeModel* treeModel_Sed = new DirectoryTreeModel();
    treeModel_Sed->loadDirectory(FileUtils::getSedRootPath(true),false, "SEDs");
    treeModel_Sed->selectRoot();
    treeModel_Sed->setEnabled(true);
    ui->treeView_ManageSed->setModel(treeModel_Sed);
    ui->treeView_ManageSed->collapseAll();
    ui->treeView_ManageSed->expand(treeModel_Sed->item(0,0)->index());
    connect( treeModel_Sed, SIGNAL(itemChanged(QStandardItem*)), treeModel_Sed,
                 SLOT(onItemChangedUniqueSelection(QStandardItem*)));

    DirectoryTreeModel* treeModel_Red = new DirectoryTreeModel();
    treeModel_Red->loadDirectory(FileUtils::getRedCurveRootPath(true),false, "Reddening Curves");
    treeModel_Red->selectRoot();
    treeModel_Red->setEnabled(true);
    ui->treeView_ManageRed->setModel(treeModel_Red);
    ui->treeView_ManageRed->collapseAll();
    ui->treeView_ManageRed->expand(treeModel_Red->item(0,0)->index());
    connect( treeModel_Red, SIGNAL(itemChanged(QStandardItem*)), treeModel_Red,
                 SLOT(onItemChangedUniqueSelection(QStandardItem*)));


    DirectoryTreeModel* treeModel_Luminosity = new DirectoryTreeModel();
    treeModel_Luminosity->loadDirectory(FileUtils::getLuminosityCurveRootPath(true),false, "Luminosity Function Curves");
    treeModel_Luminosity->selectRoot();
    treeModel_Luminosity->setEnabled(true);
      ui->treeView_ManageLuminosity->setModel(treeModel_Luminosity);
      ui->treeView_ManageLuminosity->collapseAll();
      ui->treeView_ManageLuminosity->expand(treeModel_Luminosity->item(0,0)->index());
      connect( treeModel_Luminosity, SIGNAL(itemChanged(QStandardItem*)), treeModel_Luminosity,
                   SLOT(onItemChangedUniqueSelection(QStandardItem*)));

    ui->tab_Management->setCurrentIndex(index);
}



//  - Slot on this page

void FormAuxDataManagement::on_btn_RedImport_clicked()
{
    std::string title = "Import Reddening Curves";
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageRed->model());
    std::string group =  model->getGroup();
    DialogImportAuxData* popup= new DialogImportAuxData();
    popup->setData(title,group,model->getRelPath(group,"Reddening Curves"));
    if (popup->exec()){
        loadManagementPage(2);
    }
}

void FormAuxDataManagement::on_btn_RedSubGroup_clicked()
{
  unique_ptr<DialogCreateSubGroup> popup (new DialogCreateSubGroup());
  auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageRed->model());
  std::string group =  model->getGroup();
  popup->setParentFolder(group,model->getRelPath(group,"Reddening Curves"));
  if ( popup->exec()){
      loadManagementPage(2);
  }
}

void FormAuxDataManagement::on_btn_RedDelete_clicked()
{
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageRed->model());
    std::string selection =  model->getRootSelection().second;
    if (QMessageBox::question( this, "Confirm deletion...",
                                  QString::fromStdString("Do you really want to delete the reddening curve(s) '"+model->getRelPath(selection,"Reddening Curves")+"'?"),
                                  QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes){
        FileUtils::removeDir(QString::fromStdString(selection));
        loadManagementPage(2);
    }
}

void FormAuxDataManagement::on_btn_SedImport_clicked()
{
    std::string title = "Import SED(s)";

    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageSed->model());
    std::string group =  model->getGroup();
    unique_ptr<DialogImportAuxData> popup(new DialogImportAuxData());
    popup->setData(title,group,model->getRelPath(group,"SEDs"));
    if (popup->exec()){
        loadManagementPage(1);
    }
}

void FormAuxDataManagement::on_btn_SedSubGroup_clicked()
{
    unique_ptr<DialogCreateSubGroup> popup (new DialogCreateSubGroup());
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageSed->model());
    std::string group =  model->getGroup();
    popup->setParentFolder(group,model->getRelPath(group,"SEDs"));
    if ( popup->exec()){
        loadManagementPage(1);
    }
}

void FormAuxDataManagement::on_btn_SedDelete_clicked()
{
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageSed->model());
    std::string selection =  model->getRootSelection().second;
   if (QMessageBox::question( this, "Confirm deletion...",
                                 QString::fromStdString("Do you really want to delete the SED(s) '"+model->getRelPath(selection,"SEDs")+"'?"),
                                 QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes){
       FileUtils::removeDir(QString::fromStdString(selection));
       loadManagementPage(1);
   }
}

void FormAuxDataManagement::on_btn_FilterImport_clicked()
{
    std::string title = "Import Filter Transmission Curve(s)";
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageFilter->model());
    std::string group =  model->getGroup();
    unique_ptr<DialogImportAuxData> popup( new DialogImportAuxData());
    popup->setData(title,group,model->getRelPath(group,"Filters"));
    if (popup->exec()){
        loadManagementPage(0);
    }
}

void FormAuxDataManagement::on_btn_FilterSubGroup_clicked()
{
  unique_ptr<DialogCreateSubGroup> popup (new DialogCreateSubGroup());
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageFilter->model());
    std::string group =  model->getGroup();
    popup->setParentFolder(group,model->getRelPath(group,"Filters"));
    if ( popup->exec()){
        loadManagementPage(0);
    }
}

void FormAuxDataManagement::on_btn_FilterDelete_clicked()
{
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageFilter->model());
    std::string selection =  model->getRootSelection().second;
   if (QMessageBox::question( this, "Confirm deletion...",
                                 QString::fromStdString("Do you really want to delete the Filter Transmission Curve(s) '"+model->getRelPath(selection,"Filters")+"'?"),
                                 QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes){
       FileUtils::removeDir(QString::fromStdString(selection));
       loadManagementPage(0);
   }
}

void FormAuxDataManagement::on_btn_LuminosityImport_clicked()
{
    std::string title = "Import Luminosity Function Curve(s)";
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageLuminosity->model());
    std::string group =  model->getGroup();
    unique_ptr<DialogImportAuxData> popup( new DialogImportAuxData());
    popup->setData(title,group,model->getRelPath(group,"Luminosity Function Curve"));
    if (popup->exec()){
        loadManagementPage(3);
    }
}

void FormAuxDataManagement::on_btn_LuminositySubGroup_clicked()
{
  unique_ptr<DialogCreateSubGroup> popup (new DialogCreateSubGroup());
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageLuminosity->model());
    std::string group =  model->getGroup();
    popup->setParentFolder(group,model->getRelPath(group,"Luminosity Function Curve"));
    if ( popup->exec()){
        loadManagementPage(3);
    }
}

void FormAuxDataManagement::on_btn_LuminosityDelete_clicked()
{
    auto model = static_cast<DirectoryTreeModel*>(ui->treeView_ManageLuminosity->model());
    std::string selection =  model->getRootSelection().second;
   if (QMessageBox::question( this, "Confirm deletion...",
                                 QString::fromStdString("Do you really want to delete the Luminosity Function Curve(s) '"+model->getRelPath(selection,"Luminosity Function Curve")+"'?"),
                                 QMessageBox::Yes|QMessageBox::No )==QMessageBox::Yes){
       FileUtils::removeDir(QString::fromStdString(selection));
       loadManagementPage(3);
   }
}

}
}
