#include <qfiledialog.h>

#include "PhzQtUI/FormConfiguration.h"
#include "ui_FormConfiguration.h"
#include "FileUtils.h"
#include "PreferencesUtils.h"
#include "PhzUtils/Multithreading.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include "XYDataset/AsciiParser.h"
#include "PhzQtUI/DatasetRepository.h"
#include "XYDataset/FileSystemProvider.h"



namespace Euclid {
namespace PhzQtUI {

FormConfiguration::FormConfiguration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormConfiguration)
{
    ui->setupUi(this);
}

FormConfiguration::~FormConfiguration()
{
}


void FormConfiguration::loadOptionPage(DatasetRepo seds_repository,
    DatasetRepo redenig_curves_repository) {

  m_seds_repository = seds_repository;
  m_redenig_curves_repository = redenig_curves_repository;

  ui->txt_hubble_param->setValidator(new QDoubleValidator(0, 1000, 20));
  ui->txt_omega_matter->setValidator(new QDoubleValidator(-10, 10, 20));
  ui->txt_omega_lambda->setValidator(new QDoubleValidator(-10, 10, 20));

  ui->widget_aux_Data->setRepositories(m_seds_repository, m_redenig_curves_repository);
  ui->widget_aux_Data->loadManagementPage(0);

  auto path_map = FileUtils::readPath();
  ui->txt_rootDir->setText(QString::fromStdString(FileUtils::getRootPath(false)));
  ui->txt_catDir->setText(QString::fromStdString(path_map["Catalogs"]));
  ui->txt_auxDir->setText(QString::fromStdString(path_map["AuxiliaryData"]));
  ui->txt_interDir->setText(QString::fromStdString(path_map["IntermediateProducts"]));
  ui->txt_resDir->setText(QString::fromStdString(path_map["Results"]));
  checkDirectories();

  int thread_value = PreferencesUtils::getThreadNumberOverride();
  ui->gb_thread->setChecked(thread_value > 0);
  if (thread_value > 0) {
    ui->sb_thread->setValue(thread_value);
  } else {
    ui->sb_thread->setValue(PhzUtils::getThreadNumber());
  }

  auto cosmology = PreferencesUtils::getCosmologicalParameters();
  ui->txt_hubble_param->setText(
      QString::number(cosmology.getHubbleConstant(), 'g', 15));
  ui->txt_omega_matter->setText(
      QString::number(cosmology.getOmegaM(), 'g', 15));
  ui->txt_omega_lambda->setText(
      QString::number(cosmology.getOmegaLambda(), 'g', 15));
}




//  - Slots on this page
void FormConfiguration::on_btn_ConfigToHome_clicked()
{
     navigateToHome(do_need_reset);
     do_need_reset=false;
}

void FormConfiguration::on_btn_ConfbackHome_clicked()
{
     navigateToHome(do_need_reset);
     do_need_reset=false;
}




void FormConfiguration::on_btn_editGeneral_clicked(){
  startEdition(0);
     ui->btn_editGeneral->setEnabled(false);
     ui->btn_cancelGeneral->setEnabled(true);
     ui->btn_saveGeneral->setEnabled(true);

     ui->btn_browseCat->setEnabled(true);
     ui->btn_browseAux->setEnabled(true);
     ui->btn_browseInter->setEnabled(true);
     ui->btn_browseRes->setEnabled(true);

     ui->btn_defCat->setEnabled(true);
     ui->btn_defAux->setEnabled(true);
     ui->btn_defInter->setEnabled(true);
     ui->btn_defRes->setEnabled(true);
     ui->gb_thread->setEnabled(true);
}


void FormConfiguration::on_btn_cancelGeneral_clicked(){
  auto path_map = FileUtils::readPath();
      ui->txt_rootDir->setText(QString::fromStdString(FileUtils::getRootPath(false)));
      ui->txt_catDir->setText(QString::fromStdString(path_map["Catalogs"]));
      ui->txt_auxDir->setText(QString::fromStdString(path_map["AuxiliaryData"]));
      ui->txt_interDir->setText(QString::fromStdString(path_map["IntermediateProducts"]));
      ui->txt_resDir->setText(QString::fromStdString(path_map["Results"]));
      checkDirectories();

      int thread_value = PreferencesUtils::getThreadNumberOverride();
      ui->gb_thread->setChecked(thread_value>0);
      if (thread_value>0){
        ui->sb_thread->setValue(thread_value);
      } else {
        ui->sb_thread->setValue(PhzUtils::getThreadNumber());
      }

      ui->btn_editGeneral->setEnabled(true);
      ui->btn_cancelGeneral->setEnabled(false);
      ui->btn_saveGeneral->setEnabled(false);


      ui->btn_browseCat->setEnabled(false);
      ui->btn_browseAux->setEnabled(false);
      ui->btn_browseInter->setEnabled(false);
      ui->btn_browseRes->setEnabled(false);

      ui->btn_defCat->setEnabled(false);
      ui->btn_defAux->setEnabled(false);
      ui->btn_defInter->setEnabled(false);
      ui->btn_defRes->setEnabled(false);
      ui->gb_thread->setEnabled(false);

      endEdition();
}


void FormConfiguration::on_btn_saveGeneral_clicked(){
  std::map<std::string,std::string> map{};

      map.insert(std::make_pair("LastUsed",FileUtils::getLastUsedPath()));

      map.insert(std::make_pair("Catalogs",ui->txt_catDir->text().toStdString()));

      map.insert(std::make_pair("AuxiliaryData",ui->txt_auxDir->text().toStdString()));

      map.insert(std::make_pair("IntermediateProducts",ui->txt_interDir->text().toStdString()));

      map.insert(std::make_pair("Results",ui->txt_resDir->text().toStdString()));


      FileUtils::savePath(map);

      int thread_value=0;
      if (ui->gb_thread->isChecked()){
        thread_value=ui->sb_thread->value();
      }  else {
        ui->sb_thread->setValue(PhzUtils::getThreadNumber());
      }
      PreferencesUtils::setThreadNumberOverride(thread_value);

      ui->btn_editGeneral->setEnabled(true);
      ui->btn_cancelGeneral->setEnabled(false);
      ui->btn_saveGeneral->setEnabled(false);


      ui->btn_browseCat->setEnabled(false);
      ui->btn_browseAux->setEnabled(false);
      ui->btn_browseInter->setEnabled(false);
      ui->btn_browseRes->setEnabled(false);

      ui->btn_defCat->setEnabled(false);
      ui->btn_defAux->setEnabled(false);
      ui->btn_defInter->setEnabled(false);
      ui->btn_defRes->setEnabled(false);
      ui->gb_thread->setEnabled(false);

      std::unique_ptr <XYDataset::FileParser > sed_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> sed_provider(new XYDataset::FileSystemProvider{FileUtils::getSedRootPath(false), std::move(sed_file_parser) });
      m_seds_repository->resetProvider(std::move(sed_provider));

      std::unique_ptr <XYDataset::FileParser > reddening_file_parser {new XYDataset::AsciiParser { } };
      std::unique_ptr<XYDataset::FileSystemProvider> red_curve_provider(new XYDataset::FileSystemProvider{  FileUtils::getRedCurveRootPath(false), std::move(reddening_file_parser) });
      m_redenig_curves_repository->resetProvider(std::move(red_curve_provider));


      do_need_reset=true;
      endEdition();
}


void FormConfiguration::on_btn_browseCat_clicked(){
  auto root_path= QString::fromStdString(FileUtils::getCatalogRootPath(false,""));
  QFileDialog dialog(this);
  dialog.selectFile(root_path);
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  if (dialog.exec()){
      QStringList fileNames=dialog.selectedFiles();



      ui->txt_catDir->setText(fileNames[0]);
  }
  checkDirectories();
}

void FormConfiguration::on_btn_browseAux_clicked(){
  auto root_path= QString::fromStdString(FileUtils::getAuxRootPath());
   QFileDialog dialog(this);
   dialog.selectFile(root_path);
   dialog.setFileMode(QFileDialog::DirectoryOnly);
   if (dialog.exec()){
       QStringList fileNames=dialog.selectedFiles();


      ui->txt_auxDir->setText(fileNames[0]);
  }
  checkDirectories();
}

void FormConfiguration::on_btn_browseInter_clicked(){
  auto root_path= QString::fromStdString(FileUtils::getIntermediaryProductRootPath(false,""));
   QFileDialog dialog(this);
   dialog.selectFile(root_path);
   dialog.setFileMode(QFileDialog::DirectoryOnly);
   if (dialog.exec()){
       QStringList fileNames=dialog.selectedFiles();


      ui->txt_interDir->setText(fileNames[0]);
  }
  checkDirectories();
}

void FormConfiguration::on_btn_browseRes_clicked(){
  auto root_path= QString::fromStdString(FileUtils::getResultRootPath(false,"",""));
   QFileDialog dialog(this);
   dialog.selectFile(root_path);
   dialog.setFileMode(QFileDialog::DirectoryOnly);
   if (dialog.exec()){
       QStringList fileNames=dialog.selectedFiles();


      ui->txt_resDir->setText(fileNames[0]);
  }
  checkDirectories();
}


void FormConfiguration::on_btn_defCat_clicked(){
  auto def = QString::fromStdString(FileUtils::getDefaultCatalogRootPath());
  ui->txt_catDir->setText(def);
  checkDirectories();
}

void FormConfiguration::on_btn_defAux_clicked(){
  auto def = QString::fromStdString(FileUtils::getDefaultAuxRootPath());
  ui->txt_auxDir->setText(def);
  checkDirectories();
}

void FormConfiguration::on_btn_defInter_clicked(){
  auto def = QString::fromStdString(FileUtils::getDefaultIntermediaryProductRootPath());
  ui->txt_interDir->setText(def);
  checkDirectories();
}

void FormConfiguration::on_btn_defRes_clicked(){
  auto def = QString::fromStdString(FileUtils::getDefaultResultsRootPath());
  ui->txt_resDir->setText(def);
  checkDirectories();
}


void FormConfiguration::on_btn_edit_cosmo_clicked(){
  startEdition(2);

  ui->txt_hubble_param->setEnabled(true);
  ui->txt_omega_matter->setEnabled(true);
  ui->txt_omega_lambda->setEnabled(true);
  ui->btn_edit_cosmo->setEnabled(false);
  ui->btn_cancel_cosmo->setEnabled(true);
  ui->btn_save_cosmo->setEnabled(true);
  ui->btn_default_cosmo->setEnabled(true);
}

void FormConfiguration::on_btn_cancel_cosmo_clicked(){

  auto cosmology = PreferencesUtils::getCosmologicalParameters();
  ui->txt_hubble_param->setText(QString::number(cosmology.getHubbleConstant(),'g',15));
  ui->txt_omega_matter->setText(QString::number(cosmology.getOmegaM(),'g',15));
  ui->txt_omega_lambda->setText(QString::number(cosmology.getOmegaLambda(),'g',15));

  ui->txt_hubble_param->setEnabled(false);
  ui->txt_omega_matter->setEnabled(false);
  ui->txt_omega_lambda->setEnabled(false);
  ui->btn_edit_cosmo->setEnabled(true);
  ui->btn_cancel_cosmo->setEnabled(false);
  ui->btn_save_cosmo->setEnabled(false);
  ui->btn_default_cosmo->setEnabled(false);

  endEdition();

}

void FormConfiguration::on_btn_save_cosmo_clicked(){
  double hubble = ui->txt_hubble_param->text().toDouble();
  double omega_m = ui->txt_omega_matter->text().toDouble();
  double omega_l = ui->txt_omega_lambda->text().toDouble();
  PhysicsUtils::CosmologicalParameters cosmology{omega_m, omega_l, hubble};
  PreferencesUtils::setCosmologicalParameters(cosmology);
  ui->txt_hubble_param->setText(QString::number(cosmology.getHubbleConstant(),'g',15));
  ui->txt_omega_matter->setText(QString::number(cosmology.getOmegaM(),'g',15));
  ui->txt_omega_lambda->setText(QString::number(cosmology.getOmegaLambda(),'g',15));

  ui->txt_hubble_param->setEnabled(false);
  ui->txt_omega_matter->setEnabled(false);
  ui->txt_omega_lambda->setEnabled(false);
  ui->btn_edit_cosmo->setEnabled(true);
  ui->btn_cancel_cosmo->setEnabled(false);
  ui->btn_save_cosmo->setEnabled(false);
  ui->btn_default_cosmo->setEnabled(false);
  endEdition();
}

void FormConfiguration::on_btn_default_cosmo_clicked(){
  PhysicsUtils::CosmologicalParameters cosmology{};
  ui->txt_hubble_param->setText(QString::number(cosmology.getHubbleConstant(),'g',15));
  ui->txt_omega_matter->setText(QString::number(cosmology.getOmegaM(),'g',15));
  ui->txt_omega_lambda->setText(QString::number(cosmology.getOmegaLambda(),'g',15));
}


void FormConfiguration::startEdition(int i){
  for (int j=0;j<3;++j){
      if (i!=j){
        ui->tabWidget->setTabEnabled(j,false);
      }
  }

  ui->btn_ConfbackHome->setEnabled(false);
  ui->btn_ConfigToHome->setEnabled(false);
}


void FormConfiguration::endEdition(){

  ui->tabWidget->setTabEnabled(0,true);
  ui->tabWidget->setTabEnabled(1,true);
  ui->tabWidget->setTabEnabled(2,true);
  ui->tabWidget->setEnabled(true);
  ui->btn_ConfbackHome->setEnabled(true);
  ui->btn_ConfigToHome->setEnabled(true);
}

void FormConfiguration::checkDirectories(){

  if (ui->txt_catDir->text().toStdString()==FileUtils::getDefaultCatalogRootPath()){
    ui->txt_catDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
    ui->txt_catDir->setStyleSheet("QLineEdit { color: black }");
  }

  if (ui->txt_auxDir->text().toStdString()==FileUtils::getDefaultAuxRootPath()){
      ui->txt_auxDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
      ui->txt_auxDir->setStyleSheet("QLineEdit { color: black }");
  }

  if (ui->txt_interDir->text().toStdString()==FileUtils::getDefaultIntermediaryProductRootPath()){
      ui->txt_interDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
      ui->txt_interDir->setStyleSheet("QLineEdit { color: black }");
  }

  if (ui->txt_resDir->text().toStdString()==FileUtils::getDefaultResultsRootPath()){
      ui->txt_resDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
      ui->txt_resDir->setStyleSheet("QLineEdit { color: black }");
  }
}












}
}
