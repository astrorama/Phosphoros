#include <QMessageBox>
#include <QStandardItemModel>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QDir>
#include <QFileDialog>
#include <QListView>
#include <QTreeView>
#include <QtGui/qdesktopservices.h>
#include <QtCore/qurl.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include "ElementsKernel/Logging.h"
#include "PreferencesUtils.h"

#include "AlexandriaKernel/memory_tools.h"
#include "PhzQtUI/FormAuxDataManagement.h"
#include "ui_FormAuxDataManagement.h"
#include "PhzQtUI/DataSetTreeModel.h"
#include "PhzQtUI/DialogImportAuxData.h"
#include "PhzQtUI/DialogCreatesSubGroup.h"
#include "PhzQtUI/DialogInterpolateSed.h"
#include "FileUtils.h"
#include "XYDataset/AsciiParser.h"
#include "PhzQtUI/filecopyer.h"
#include "PhzQtUI/DialogSedSelector.h"
#include "PhzQtUI/DialogSedParam.h"


namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("FormAuxDataManagement");


FormAuxDataManagement::FormAuxDataManagement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAuxDataManagement)
{
    ui->setupUi(this);
    m_planck_file = FileUtils::getAuxRootPath() +"/GalacticDustMap/PlanckEbv.fits";
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

void FormAuxDataManagement::displayFilter() {
	m_filter_del_buttons.clear();
	DataSetTreeModel* treeModel_filter = new DataSetTreeModel(m_filter_repository);
	treeModel_filter->load(false);
	treeModel_filter->setEnabled(true);
	ui->treeView_ManageFilter->setModel(treeModel_filter);
	ui->treeView_ManageFilter->collapseAll();
	ui->treeView_ManageFilter->header()->setStretchLastSection(false);
	ui->treeView_ManageFilter->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	for (int i = 0; i < treeModel_filter->rowCount(); i++) {
		addDeleteButtonsToItem(
				treeModel_filter->item(i),
				treeModel_filter,
				ui->treeView_ManageFilter,
				SLOT(deletFilterGroupButtonClicked(const QString&)),
				m_filter_del_buttons,
				1);
	}
}

void FormAuxDataManagement::displaySED() {
	m_message_buttons.clear();
	m_sed_del_buttons.clear();
	SedTreeModel* treeModel_Sed = new SedTreeModel(m_seds_repository);
	treeModel_Sed->load(false);
	treeModel_Sed->setEnabled(true);
	ui->treeView_ManageSed->setModel(treeModel_Sed);
	ui->treeView_ManageSed->collapseAll();
	ui->treeView_ManageSed->header()->setStretchLastSection(false);
	ui->treeView_ManageSed->header()->setSectionResizeMode(0, QHeaderView::Stretch);

	for (int i = 0; i < treeModel_Sed->rowCount(); i++) {
	  addButtonsToSedItem(treeModel_Sed->item(i), treeModel_Sed);
	  addDeleteButtonsToItem(
					treeModel_Sed->item(i),
					treeModel_Sed,
					ui->treeView_ManageSed,
					SLOT(deletSedGroupButtonClicked(const QString&)),
					m_sed_del_buttons,
					2);
	}

	ui->treeView_ManageSed->resizeColumnToContents(1);
}

void FormAuxDataManagement::displayRed() {
	m_red_del_buttons.clear();
    DataSetTreeModel* treeModel_Red = new DataSetTreeModel(m_redenig_curves_repository);
    treeModel_Red->load(false);
    treeModel_Red->setEnabled(true);
    ui->treeView_ManageRed->setModel(treeModel_Red);
    ui->treeView_ManageRed->collapseAll();
    ui->treeView_ManageRed->header()->setStretchLastSection(false);
    ui->treeView_ManageRed->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    for (int i = 0; i < treeModel_Red->rowCount(); i++) {
      	addDeleteButtonsToItem(
      			treeModel_Red->item(i),
				treeModel_Red,
  				ui->treeView_ManageRed,
  				SLOT(deletRedGroupButtonClicked(const QString&)),
				m_red_del_buttons,
  				1);
      }

}

void FormAuxDataManagement::displayLum() {
	m_lum_del_buttons.clear();
	DataSetTreeModel* treeModel_Luminosity = new DataSetTreeModel(m_luminosity_repository);
	treeModel_Luminosity->load(false);
	treeModel_Luminosity->setEnabled(true);
	ui->treeView_ManageLuminosity->setModel(treeModel_Luminosity);
	ui->treeView_ManageLuminosity->collapseAll();
	ui->treeView_ManageLuminosity->header()->setStretchLastSection(false);
	ui->treeView_ManageLuminosity->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	for (int i = 0; i < treeModel_Luminosity->rowCount(); i++) {
		addDeleteButtonsToItem(
				treeModel_Luminosity->item(i),
				treeModel_Luminosity,
				ui->treeView_ManageLuminosity,
				SLOT(deletLumGroupButtonClicked(const QString&)),
				m_lum_del_buttons,
				1);
    }
}

void FormAuxDataManagement::loadManagementPage(int index){
	displayFilter();
	displaySED();
	displayRed();
	displayLum();

    if (index>=0){
      ui->tab_Management->setCurrentIndex(index);
    }

    std::string sun_sed = PreferencesUtils::getUserPreference("AuxData","SUN_SED");
    ui->lbl_sun_sed->setText(QString::fromStdString(sun_sed));
}

void FormAuxDataManagement::on_btn_interp_clicked() {
     std::unique_ptr<DialogInterpolateSed> dialog(new DialogInterpolateSed(this));
     if (dialog->exec()) {
       copyingSEDFinished(true,{});
     }
}

void FormAuxDataManagement::getParameterInfoClicked(const QString& file) {
  logger.info() << "File selected :"<< file.toStdString();


  std::unique_ptr<DialogSedParam> dialog(new DialogSedParam(m_seds_repository));
    dialog->setSed(XYDataset::QualifiedName(file.toStdString()));
    dialog->exec();
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


         logger.info() << "Executing :"<< command.toStdString();

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

          logger.info() << "Executing :"<< command.toStdString();

          connect(lineAdder, SIGNAL(finished(int, QProcess::ExitStatus)), this,
                                      SLOT(sedProcessfinished(int, QProcess::ExitStatus)));
          connect(lineAdder, SIGNAL(started()), this, SLOT(sedProcessStarted()));

          lineAdder->start(command);
    } else {
      ui->labelMessage->setText("");
    }

}

template<class TreeModel>
void FormAuxDataManagement::addDeleteButtonsToItem(QStandardItem* item,
		TreeModel* treeModel,
												   QTreeView * view,
												   const char* slot,
												   std::vector<std::unique_ptr<MessageButton>>& btn_vector,
												   int index) {

	if ( item->rowCount()>0) {
		auto name = treeModel->getFullGroupName(item);
		auto cartButton = Euclid::make_unique<MessageButton>(name, "Delete");
		cartButton->setMaximumSize(100,50);
		auto idx = item->index().sibling(item->index().row(), index);
		view->setIndexWidget(idx, cartButton.get());
	    connect(cartButton.get(), SIGNAL(MessageButtonClicked(const QString&)), this, slot);
		btn_vector.push_back(std::move(cartButton));
		for (int i = 0; i < item->rowCount(); i++) {
			addDeleteButtonsToItem(item->child(i), treeModel, view, slot, btn_vector, index);
		}
	}

}

void FormAuxDataManagement::addButtonsToSedItem(QStandardItem *item, SedTreeModel *treeModel_sed) {
  if (treeModel_sed->canAddEmissionLineToGroup(item) || treeModel_sed->canAddLpEmissionLineToGroup(item)) {
    auto name = treeModel_sed->getFullGroupName(item);

    auto cartButton = Euclid::make_unique<MessageButton>(name, "Add Emission Line to SEDs");
	cartButton->setMaximumSize(250,50);
	cartButton->setMinimumSize(190,10);

    auto index = item->index().sibling(item->index().row(), 1);

    ui->treeView_ManageSed->setIndexWidget(index, cartButton.get());

    connect(cartButton.get(), SIGNAL(MessageButtonClicked(const QString&)), this,
            SLOT(addEmissionLineButtonClicked(const QString&)));

    m_message_buttons.push_back(std::move(cartButton));
  }
  if (treeModel_sed->canOpenInfo(item)) {

            auto name = treeModel_sed->getFullGroupName(item);
            if (name != "") {
              name = name + "/";
            }

            name = name + item->text();


            MessageButton *cartButton = new MessageButton(name, item->text());
            m_message_buttons.emplace_back(cartButton);

            auto index = item->index();

            ui->treeView_ManageSed->setIndexWidget(index, cartButton);

            connect(cartButton, SIGNAL(MessageButtonClicked(const QString&)), this,
                            SLOT(getParameterInfoClicked(const QString&)));
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
      // reload the provider and the model
      std::unique_ptr <XYDataset::FileParser > sed_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(
          new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(true), std::move(sed_file_parser) });
      m_seds_repository->resetProvider(std::move(sed_provider));

      displaySED();

      ui->labelMessage->setText("Processing of SEDs completed.");
}

void FormAuxDataManagement::copyingFilterFinished(bool success, QVector<QString> ){
 if (success){
    logger.info() << "files modified ";
    // reset repo
    std::unique_ptr <XYDataset::FileParser > filter_file_parser {new XYDataset::AsciiParser { } };
    std::unique_ptr<XYDataset::FileSystemProvider> filter_provider(
       new XYDataset::FileSystemProvider{FileUtils::getFilterRootPath(true), std::move(filter_file_parser) });
    m_filter_repository->resetProvider(std::move(filter_provider));
    displayFilter();
 } else {
   logger.warn() << "Modification of the files failed";
 }
}

void FormAuxDataManagement::copyingSEDFinished(bool success, QVector<QString> ){
 if (success){
    m_message_buttons.clear();

    logger.info() << "files modified ";
    // reload the provider and the model
    std::unique_ptr <XYDataset::FileParser > sed_file_parser {new XYDataset::AsciiParser { } };
    std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(
       new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(true), std::move(sed_file_parser) });
    m_seds_repository->resetProvider(std::move(sed_provider));
    displaySED();
 } else {
   logger.warn() << "Modification of the files failed";
 }
}

void FormAuxDataManagement::copyingRedFinished(bool success, QVector<QString> ){
 if (success){
    logger.info() << "files modified ";
    // reload the provider and the model
    std::unique_ptr <XYDataset::FileParser > red_file_parser {new XYDataset::AsciiParser { } };
    std::unique_ptr<XYDataset::FileSystemProvider> red_provider(
       new XYDataset::FileSystemProvider{FileUtils::getRedCurveRootPath(true), std::move(red_file_parser) });
    m_redenig_curves_repository->resetProvider(std::move(red_provider));
    displayRed();
 } else {
   logger.warn() << "Modification of the files failed";
 }
}

void FormAuxDataManagement::copyingLumFinished(bool success, QVector<QString> ){
   if (success){
      logger.info() << "files modified ";
      // reload the provider and the model
      std::unique_ptr <XYDataset::FileParser > lum_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> lum_provider(
         new XYDataset::FileSystemProvider{FileUtils::getLuminosityFunctionCurveRootPath(true), std::move(lum_file_parser) });
      m_luminosity_repository->resetProvider(std::move(lum_provider));
      displayLum();
   } else {
     logger.warn() << "Modification of the files failed";
   }
}

void FormAuxDataManagement::copyProgress(qint64 copy,qint64 total){
  logger.info() << "File copy progress => " << QString::number((qreal(copy) / qreal(total)) * 100.0).toStdString() << "%";
}

void FormAuxDataManagement::deletFilterGroupButtonClicked(const QString& group) {
	logger.info() << "Filter Group " << group.toStdString() << " deletion requested.";
	QMessageBox msgBox;
	msgBox.setText("Delete a Filter Group..");
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setInformativeText(QString::fromStdString("You are about to delete the filter group ")+
		group+QString::fromStdString(". \n\nThis is an irreversible action: the corresponding folder will be deleted from your file system.") +
		QString::fromStdString(" \n\nIf you where using some of these filters in any Catalog no further processing of it will be possible!"));

	msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Apply);
	if (msgBox.exec() == QMessageBox::Apply) {
		std::string path = FileUtils::getFilterRootPath(false)+"/"+group.toStdString();
		boost::filesystem::remove_all(path);
		copyingFilterFinished(true,{});
	}
}

void FormAuxDataManagement::deletSedGroupButtonClicked(const QString& group){
	logger.info() << "SED Group " << group.toStdString() << " deletion requested.";
	QMessageBox msgBox;
	msgBox.setText("Delete a SED Group..");
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setInformativeText(QString::fromStdString("You are about to delete the SED group ")+
		group+QString::fromStdString(". \n\nThis is an irreversible action: the corresponding folder will be deleted from your file system.") +
		QString::fromStdString(" \n\nIf you where using some of these SEDs in any Parameter Space no further processing of it will be possible!"));

	msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Apply);
	if (msgBox.exec() == QMessageBox::Apply) {
		std::string path = FileUtils::getSedRootPath(false)+"/"+group.toStdString();
		boost::filesystem::remove_all(path);
		copyingSEDFinished(true,{});
	}
}

void FormAuxDataManagement::deletRedGroupButtonClicked(const QString& group){
	logger.info() << "Reddening Curve Group " << group.toStdString() << " deletion requested.";
	QMessageBox msgBox;
	msgBox.setText("Delete a Reddening Curve Group..");
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setInformativeText(QString::fromStdString("You are about to delete the Reddening Curve group ")+
		group+QString::fromStdString(". \n\nThis is an irreversible action: the corresponding folder will be deleted from your file system.") +
		QString::fromStdString(" \n\nIf you where using some of these Reddening Curves in any Parameter Space no further processing of it will be possible!"));

	msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Apply);
	if (msgBox.exec() == QMessageBox::Apply) {
		std::string path = FileUtils::getRedCurveRootPath(false)+"/"+group.toStdString();
		boost::filesystem::remove_all(path);
		copyingRedFinished(true,{});
	}
}

void FormAuxDataManagement::deletLumGroupButtonClicked(const QString& group){
	logger.info() << "Luminosity Function Curve Group " << group.toStdString() << " deletion requested.";
	QMessageBox msgBox;
	msgBox.setText("Delete a Luminosity Function Curve Group..");
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setInformativeText(QString::fromStdString("You are about to delete the Luminosity Function Curve group ")+
		group+QString::fromStdString(". \n\nThis is an irreversible action: the corresponding folder will be deleted from your file system.") +
		QString::fromStdString(" \n\nIf you where using some of these Luminosity Function Curve in any Luminosity Prior configuration no further processing with it will be possible!"));

	msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Apply);
	if (msgBox.exec() == QMessageBox::Apply) {
		std::string path = FileUtils::getLuminosityFunctionCurveRootPath(false)+"/"+group.toStdString();
		boost::filesystem::remove_all(path);
		copyingLumFinished(true,{});
	}
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

void FormAuxDataManagement::on_btn_sun_sed_clicked() {
    std::unique_ptr<DialogSedSelector> dialog(new DialogSedSelector(m_seds_repository));
    dialog->setSed(PreferencesUtils::getUserPreference("AuxData","SUN_SED"));

    connect(dialog.get(),
            SIGNAL(popupClosing(std::string)),
            this,
            SLOT(sunSedPopupClosing(std::string)));
    dialog->exec();
}

void FormAuxDataManagement::sunSedPopupClosing(std::string sed){
  PreferencesUtils::setUserPreference("AuxData","SUN_SED", sed);
  ui->lbl_sun_sed->setText(QString::fromStdString(sed));
}

void FormAuxDataManagement::on_btn_planck_clicked(){
  m_httpRequestAborted = false;
  bool file_exists = boost::filesystem::exists(m_planck_file);

  QString end_message=file_exists?"The file is already present: clicking OK will owerride it":"This is needed before using the Milky Way absorption correction option that you choose.";

  if (QMessageBox::Ok == QMessageBox::question(this, "Dust map file Download...",
     "You are about to download the file containing the Milky Way dust map \n" + QString::fromStdString(m_planck_url)+
     ". \n to \n" + QString::fromStdString(m_planck_file)+
     " \n\n " + end_message,
     QMessageBox::Ok | QMessageBox::Cancel)) {


      QDir enclosing_folder = QFileInfo(QString::fromStdString(m_planck_file)).absoluteDir();
      if (!enclosing_folder.exists()) {
        enclosing_folder.mkpath(".");
      }


      m_network_manager = new QNetworkAccessManager(this);
      QUrl url(QString::fromStdString(m_planck_url));
      m_downloaded_file = new QFile(QString::fromStdString(m_planck_file));
      if (!m_downloaded_file->open(QIODevice::WriteOnly)) {
             QMessageBox::information(this, tr("HTTP"),
                           tr("Unable to save the file %1: %2.")
                           .arg(QString::fromStdString(m_planck_file)).arg(m_downloaded_file->errorString()));
         delete m_downloaded_file;
         m_downloaded_file = 0;
         return;
       }

      if (m_progress_dialog) {
         delete m_progress_dialog;
         m_progress_dialog = 0;
      }
      m_progress_dialog = new QProgressDialog(this);
      m_progress_dialog->setWindowTitle(tr("Dowwnloading... "));
      m_progress_dialog->setLabelText(tr("Downloading Planck dust map"));
      m_progress_dialog->setWindowModality(Qt::WindowModal);

      connect(m_progress_dialog, SIGNAL(canceled()), this, SLOT(cancelDownloadPlanck()));


      m_reply = m_network_manager->get(QNetworkRequest(url));
      connect(m_reply, SIGNAL(readyRead()),
                 this, SLOT(httpReadyPlanckRead()));
      connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)),
                  this, SLOT(updateDownloadProgress(qint64, qint64)));


    } else {
      return;
    }


}

void FormAuxDataManagement::updateDownloadProgress(qint64 bytesRead, qint64 totalBytes) {
  if (m_httpRequestAborted) {
          return;
  }

  m_progress_dialog->setMaximum(totalBytes);
  m_progress_dialog->setValue(bytesRead);
}

void FormAuxDataManagement::httpReadyPlanckRead() {
  if (m_downloaded_file)
    m_downloaded_file->write(m_reply->readAll());
}

void FormAuxDataManagement::cancelDownloadPlanck() {
  m_httpRequestAborted = true;
  if (m_reply) {
    m_reply->abort();
  }
}

}
}
