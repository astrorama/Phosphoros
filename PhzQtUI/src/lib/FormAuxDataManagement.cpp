#include <QMessageBox>
#include <QProcess>
#include <QDir>
#include <QFileDialog>
#include <QListView>
#include <QtGui/qdesktopservices.h>
#include <QtCore/qurl.h>
#include <boost/filesystem/path.hpp>
#include "ElementsKernel/Logging.h"

#include "AlexandriaKernel/memory_tools.h"
#include "PhzQtUI/FormAuxDataManagement.h"
#include "ui_FormAuxDataManagement.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "PhzQtUI/DialogImportAuxData.h"
#include "PhzQtUI/DialogCreatesSubGroup.h"
#include "FileUtils.h"
#include "XYDataset/AsciiParser.h"
#include "PhzQtUI/filecopyer.h"


namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("FormAuxDataManagement");


FormAuxDataManagement::FormAuxDataManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuxDataManagement)
{
    ui->setupUi(this);
}

FormAuxDataManagement::~FormAuxDataManagement() {}

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

  QMessageBox msgBox;
  msgBox.setText("Add Emission Lines to SEDs in a folder..");
  msgBox.setInformativeText(QString::fromStdString("This action will create a new folder named ")+
        group+QString::fromStdString("_el/_lpel generated from SEDs from folder ") + group +
        QString::fromStdString(" with added emission lines. Which reciept do you want to use?"));

  auto model = static_cast<SedTreeModel*>(ui->treeView_ManageSed->model());
  auto selectedIndex =  ui->treeView_ManageSed->currentIndex();
  auto item = model->item(selectedIndex.row());

  msgBox.addButton(QMessageBox::Cancel);

  QPushButton *lePhareButton{ 0 };
  if (model->canAddLpEmissionLineToGroup(item)) {
    lePhareButton= msgBox.addButton(tr("Le Phare-like"), QMessageBox::ActionRole);
  }

  QPushButton *phosButton{ 0 };
  if (model->canAddEmissionLineToGroup(item)) {
    phosButton= msgBox.addButton(tr("Phosphoros"), QMessageBox::ActionRole);
  }


    msgBox.exec();

    if (msgBox.clickedButton() == phosButton) {
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
    } else if  (msgBox.clickedButton() == lePhareButton) {
      QProcess *lineAdder = new QProcess();
          lineAdder->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

          auto aux_path = FileUtils::getAuxRootPath();
          QString command = QString::fromStdString("PhosphorosAddEmissionLines --suffix _lpel --reference-factor 1.0e13 --uv-range 2100,2500 --emission-lines LePhare_lines.txt --sed-dir " + aux_path)
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


void FormAuxDataManagement::addButtonsToSedItem(QStandardItem *item, SedTreeModel *treeModel_sed) {
  if (treeModel_sed->canAddEmissionLineToGroup(item) || treeModel_sed->canAddLpEmissionLineToGroup(item)) {
    auto name = treeModel_sed->getFullGroupName(item);

    auto cartButton = Euclid::make_unique<MessageButton>(name, "Add Emission Line to SEDs");

    auto index = item->index().sibling(item->index().row(), 1);

    ui->treeView_ManageSed->setIndexWidget(index, cartButton.get());

    connect(cartButton.get(), SIGNAL(MessageButtonClicked(const QString&)), this,
            SLOT(addEmissionLineButtonClicked(const QString&)));

    m_message_buttons.push_back(std::move(cartButton));
  }


  for (int i = 0; i < item->rowCount(); i++) {
    addButtonsToSedItem(item->child(i), treeModel_sed);
  }
}



void FormAuxDataManagement::sedProcessStarted() {
  ui->labelMessage->setText("Adding emission Lines to the SEDs...");
  for (auto& button :m_message_buttons) {
    button->setEnabled(false);
  }
}

void FormAuxDataManagement::sedProcessfinished(int, QProcess::ExitStatus) {
      // remove the buttons
      m_message_buttons.clear();

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


void FormAuxDataManagement::copyingFilterFinished(bool success, QVector<QString> ){
 if (success){
    logger.info() << "files imported ";
    // reset repo
    std::unique_ptr <XYDataset::FileParser > filter_file_parser {new XYDataset::AsciiParser { } };
    std::unique_ptr<XYDataset::FileSystemProvider> filter_provider(
       new XYDataset::FileSystemProvider{FileUtils::getFilterRootPath(true), std::move(filter_file_parser) });
    m_filter_repository->resetProvider(std::move(filter_provider));
    DataSetTreeModel* treeModel_filter = new DataSetTreeModel(m_filter_repository);
    treeModel_filter->load(false);
    treeModel_filter->setEnabled(true);
    ui->treeView_ManageFilter->setModel(treeModel_filter);
    ui->treeView_ManageFilter->collapseAll();
 } else {
   logger.warn() << "Copy of the files failed";
 }
}

void FormAuxDataManagement::copyingSEDFinished(bool success, QVector<QString> ){
 if (success){
    m_message_buttons.clear();

    logger.info() << "files imported ";
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
 } else {
   logger.warn() << "Copy of the files failed";
 }
}

void FormAuxDataManagement::copyingRedFinished(bool success, QVector<QString> ){
 if (success){
    logger.info() << "files imported ";
    // reload the provider and the model
    std::unique_ptr <XYDataset::FileParser > red_file_parser {new XYDataset::AsciiParser { } };
    std::unique_ptr<XYDataset::FileSystemProvider> red_provider(
       new XYDataset::FileSystemProvider{FileUtils::getRedCurveRootPath(true), std::move(red_file_parser) });
    m_redenig_curves_repository->resetProvider(std::move(red_provider));
    DataSetTreeModel* treeModel_red = new DataSetTreeModel(m_redenig_curves_repository);
    treeModel_red->load(false);
    treeModel_red->setEnabled(true);
    ui->treeView_ManageRed->setModel(treeModel_red);
    ui->treeView_ManageRed->collapseAll();
 } else {
   logger.warn() << "Copy of the files failed";
 }
}

void FormAuxDataManagement::copyingLumFinished(bool success, QVector<QString> ){
   if (success){
      logger.info() << "files imported ";
      // reload the provider and the model
      std::unique_ptr <XYDataset::FileParser > lum_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> lum_provider(
         new XYDataset::FileSystemProvider{FileUtils::getLuminosityFunctionCurveRootPath(true), std::move(lum_file_parser) });
      m_luminosity_repository->resetProvider(std::move(lum_provider));
      DataSetTreeModel* treeModel_lum = new DataSetTreeModel(m_luminosity_repository);
      treeModel_lum->load(false);
      treeModel_lum->setEnabled(true);
      ui->treeView_ManageLuminosity->setModel(treeModel_lum);
      ui->treeView_ManageLuminosity->collapseAll();
   } else {
     logger.warn() << "Copy of the files failed";
   }
}

void FormAuxDataManagement::copyProgress(qint64 copy,qint64 total){
  logger.info() << "File copy progress => " << QString::number((qreal(copy) / qreal(total)) * 100.0).toStdString() << "%";
}

void FormAuxDataManagement::on_btn_import_filter_clicked() {
     QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
     if (dialog.exec()) {
       // Proces
       QString folder_input = dialog.selectedFiles()[0];
       QDir filters_folder = QDir(QString::fromStdString(FileUtils::getAuxRootPath() + "/Filters/"));

       // create the folder
       QString new_folder = "";
       if (folder_input.contains("/")) {
         auto chunks = folder_input.split("/");
         new_folder = chunks[chunks.length()-1];
       } else {
         new_folder = folder_input;
       }

       filters_folder.mkdir(new_folder);
       filters_folder.cd(new_folder);
       QString dest_folder = filters_folder.absolutePath();

       // list the content
       QDir copy_folder(folder_input);
       auto file_list = copy_folder.entryInfoList(QDir::Files);

       QVector<QString> input{};
       QVector<QString> output{};
       for (auto& file : file_list) {
         input.push_back(file.absoluteFilePath());
         QString dest = dest_folder + "/" + file.fileName();
         output.push_back(dest);
       }

       // launch the copy
       auto local = new QThread;
       auto worker = new FileCopyer(local);
       QObject::connect(worker, SIGNAL(finished(bool, QVector<QString>)), SLOT(copyingFilterFinished(bool, QVector<QString>)));
       QObject::connect(worker, SIGNAL(copyProgress(qint64, qint64)), SLOT(copyProgress(qint64, qint64)));
       worker->setSourcePaths(input);
       worker->setDestinationPaths(output);
       local->start();
     }
}

void FormAuxDataManagement::on_btn_import_sed_clicked() {
     QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
     if (dialog.exec()) {
       // Proces
       QString folder_input = dialog.selectedFiles()[0];

       QDir filters_folder = QDir(QString::fromStdString(FileUtils::getAuxRootPath() + "/SEDs/"));

       // create the folder
       QString new_folder = "";
       if (folder_input.contains("/")) {
         auto chunks = folder_input.split("/");
         new_folder = chunks[chunks.length()-1];
       } else {
         new_folder = folder_input;
       }

       filters_folder.mkdir(new_folder);
       filters_folder.cd(new_folder);
       QString dest_folder = filters_folder.absolutePath();

       // list the content
       QDir copy_folder(folder_input);
       auto file_list = copy_folder.entryInfoList(QDir::Files);

       QVector<QString> input{};
       QVector<QString> output{};
       for (auto& file : file_list) {
         input.push_back(file.absoluteFilePath());
         QString dest = dest_folder + "/" + file.fileName();
         output.push_back(dest);
       }

       // launch the copy
       auto local = new QThread;
       auto worker = new FileCopyer(local);
       QObject::connect(worker, SIGNAL(finished(bool, QVector<QString>)), SLOT(copyingSEDFinished(bool, QVector<QString>)));
       QObject::connect(worker, SIGNAL(copyProgress(qint64, qint64)), SLOT(copyProgress(qint64, qint64)));
       worker->setSourcePaths(input);
       worker->setDestinationPaths(output);
       local->start();
     }
}

void FormAuxDataManagement::on_btn_import_reddening_clicked() {
     QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
     if (dialog.exec()) {
       // Proces
       QString folder_input = dialog.selectedFiles()[0];

       QDir filters_folder = QDir(QString::fromStdString(FileUtils::getAuxRootPath() + "/ReddeningCurves/"));

       // create the folder
       QString new_folder = "";
       if (folder_input.contains("/")) {
         auto chunks = folder_input.split("/");
         new_folder = chunks[chunks.length()-1];
       } else {
         new_folder = folder_input;
       }

       filters_folder.mkdir(new_folder);
       filters_folder.cd(new_folder);
       QString dest_folder = filters_folder.absolutePath();

       // list the content
       QDir copy_folder(folder_input);
       auto file_list = copy_folder.entryInfoList(QDir::Files);

       QVector<QString> input{};
       QVector<QString> output{};
       for (auto& file : file_list) {
         input.push_back(file.absoluteFilePath());
         QString dest = dest_folder + "/" + file.fileName();
         output.push_back(dest);
       }

       // launch the copy
       auto local = new QThread;
       auto worker = new FileCopyer(local);
       QObject::connect(worker, SIGNAL(finished(bool, QVector<QString>)), SLOT(copyingRedFinished(bool, QVector<QString>)));
       QObject::connect(worker, SIGNAL(copyProgress(qint64, qint64)), SLOT(copyProgress(qint64, qint64)));
       worker->setSourcePaths(input);
       worker->setDestinationPaths(output);
       local->start();
     }
}

void FormAuxDataManagement::on_btn_import_luminosity_clicked() {
     QFileDialog dialog(this);
     dialog.setFileMode(QFileDialog::Directory);
     if (dialog.exec()) {
       // Proces
       QString folder_input = dialog.selectedFiles()[0];

       QDir filters_folder = QDir(QString::fromStdString(FileUtils::getAuxRootPath() + "/LuminosityFunctionCurves/"));

       // create the folder
       QString new_folder = "";
       if (folder_input.contains("/")) {
         auto chunks = folder_input.split("/");
         new_folder = chunks[chunks.length()-1];
       } else {
         new_folder = folder_input;
       }

       filters_folder.mkdir(new_folder);
       filters_folder.cd(new_folder);
       QString dest_folder = filters_folder.absolutePath();

       // list the content
       QDir copy_folder(folder_input);
       auto file_list = copy_folder.entryInfoList(QDir::Files);

       QVector<QString> input{};
       QVector<QString> output{};
       for (auto& file : file_list) {
         input.push_back(file.absoluteFilePath());
         QString dest = dest_folder + "/" + file.fileName();
         output.push_back(dest);
       }

       // launch the copy
       auto local = new QThread;
       auto worker = new FileCopyer(local);
       QObject::connect(worker, SIGNAL(finished(bool, QVector<QString>)), SLOT(copyingLumFinished(bool, QVector<QString>)));
       QObject::connect(worker, SIGNAL(copyProgress(qint64, qint64)), SLOT(copyProgress(qint64, qint64)));
       worker->setSourcePaths(input);
       worker->setDestinationPaths(output);
       local->start();
     }
}

void FormAuxDataManagement::on_bt_reloadDP_clicked(){
    m_dataPackHandler.reset(new DataPackHandler(this));
    connect(m_dataPackHandler.get(), SIGNAL(completed()), this, SLOT(reloadAuxData()));
    m_dataPackHandler->check(true);
}

void FormAuxDataManagement::reloadAuxData(){
   copyingFilterFinished(true, {});
   copyingSEDFinished(true, {});
   copyingRedFinished(true, {});
   copyingLumFinished(true, {});
}

}
}




