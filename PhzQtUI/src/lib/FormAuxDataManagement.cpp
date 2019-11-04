#include <QMessageBox>
#include <QProcess>
#include <QDir>
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

    SedTreeModel* treeModel_Sed = new SedTreeModel(m_seds_repository);
    treeModel_Sed->load(false);
    treeModel_Sed->setEnabled(true);
    ui->treeView_ManageSed->setModel(treeModel_Sed);
    ui->treeView_ManageSed->collapseAll();
    ui->treeView_ManageSed->setColumnWidth(0, 500);

    for (int i = 0; i < treeModel_Sed->rowCount(); i++) {
      addButtonsToSedItem(treeModel_Sed->item(i), treeModel_Sed);
    }


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

void FormAuxDataManagement::addEmissionLineButtonClicked(const QString& group) {
  if (QMessageBox::question(this, "Add emission lines to SEDs in a folder...",
      QString::fromStdString("This action will create a new folder named ")+
      group+QString::fromStdString("_el generated from SEDs from folder ") + group +
      QString::fromStdString(" with added emission lines?"), QMessageBox::Ok|QMessageBox::Cancel)
    == QMessageBox::Ok) {
    // do the procesing
    QProcess *lineAdder = new QProcess();
    lineAdder->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

    auto aux_path = FileUtils::getAuxRootPath();
    QString command = QString::fromStdString("PhosphorosAddEmissionLines --sed-dir " + aux_path)
                      + QDir::separator() + QString::fromStdString("SEDs")
                      + QDir::separator() + group;




    connect(lineAdder, SIGNAL(finished(int, QProcess::ExitStatus)), this,
                                SLOT(sedProcessfinished(int, QProcess::ExitStatus)));
    connect(lineAdder, SIGNAL(started()), this, SLOT(sedProcessStarted()));

    lineAdder->start(command);
  } else {
    ui->labelMessage->setText("");
  }
}



void FormAuxDataManagement::addButtonsToSedItem(QStandardItem* item, SedTreeModel* treeModel_sed){
  if (treeModel_sed->canAddEmissionLineToGroup(item)) {
             auto name = treeModel_sed->getFullGroupName(item);

             MessageButton *cartButton = new MessageButton(name, "Add Emission Line to SEDs");
             m_message_buttons.push_back(cartButton);

             auto index = item->index().sibling(item->index().row(), 1);

             ui->treeView_ManageSed->setIndexWidget(index, cartButton);

             connect(cartButton, SIGNAL(MessageButtonClicked(const QString&)), this,
                             SLOT(addEmissionLineButtonClicked(const QString&)));
  }

  for (int i = 0; i < item->rowCount(); i++) {
    addButtonsToSedItem(item->child(i),treeModel_sed);
  }
}



void FormAuxDataManagement::sedProcessStarted() {
    ui->labelMessage->setText("Adding emission Lines to the SEDs...");
    for (auto button :m_message_buttons){
      button->setEnabled(false);
    }

}

void FormAuxDataManagement::sedProcessfinished(int, QProcess::ExitStatus) {
      // remove the buttons
      for (auto button :m_message_buttons){
        delete button;
      }

      m_message_buttons = std::vector<MessageButton*>();




      // reload the provider and the model
      std::unique_ptr <XYDataset::FileParser > sed_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(
          new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(true), std::move(sed_file_parser) });
      m_seds_repository->resetProvider(std::move(sed_provider));


      SedTreeModel* treeModel_Sed = new SedTreeModel(m_seds_repository);
        treeModel_Sed->load(false);
        treeModel_Sed->setEnabled(true);
        ui->treeView_ManageSed->setModel(treeModel_Sed);
        ui->treeView_ManageSed->collapseAll();
        ui->treeView_ManageSed->setColumnWidth(0, 500);

        for (int i = 0; i < treeModel_Sed->rowCount(); i++) {
          addButtonsToSedItem(treeModel_Sed->item(i), treeModel_Sed);
        }

      ui->labelMessage->setText("Processing of SEDs completed.");
}



}
}




