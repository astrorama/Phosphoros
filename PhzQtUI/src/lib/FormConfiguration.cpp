#include <qfiledialog.h>

#include "PhzQtUI/FormConfiguration.h"
#include "PhzQtUI/OptionModel.h"
#include "PhzUtils/Multithreading.h"
#include "ui_FormConfiguration.h"

#include "FileUtils.h"
#include "PhysicsUtils/CosmologicalParameters.h"
#include "PhzQtUI/DatasetRepository.h"
#include "PhzUtils/Multithreading.h"
#include "PreferencesUtils.h"
#include "XYDataset/AsciiParser.h"
#include "XYDataset/FileSystemProvider.h"

namespace Euclid {
namespace PhzQtUI {

FormConfiguration::FormConfiguration(QWidget* parent) : QWidget(parent), ui(new Ui::FormConfiguration) {
  ui->setupUi(this);
}

FormConfiguration::~FormConfiguration() {}

void FormConfiguration::on_btn_ToAnalysis_clicked() {

  navigateToComputeRedshift(do_need_reset);
  do_need_reset = false;
}
void FormConfiguration::on_btn_ToCatalog_clicked() {

  navigateToCatalog(do_need_reset);
  do_need_reset = false;
}

void FormConfiguration::on_btn_ToModel_clicked() {
  navigateToParameter(do_need_reset);
  do_need_reset = false;
}

void FormConfiguration::on_btn_ToPP_clicked() {
  navigateToPostProcessing(do_need_reset);
  do_need_reset = false;
}

void FormConfiguration::on_btn_exit_clicked() {

  quit(true);
}

void FormConfiguration::loadGeneralValues() {
  ui->txt_rootDir->setText(m_option_model_ptr->getRootPath());
  ui->txt_catDir->setText(m_option_model_ptr->getCatPath());
  ui->txt_auxDir->setText(m_option_model_ptr->getAuxPath());
  ui->txt_interDir->setText(m_option_model_ptr->getInterPath());
  ui->txt_resDir->setText(m_option_model_ptr->getResPath());
  ui->gb_thread->setChecked(m_option_model_ptr->getOverrideThread());
  ui->sb_thread->setValue(m_option_model_ptr->getThreadNb());
  ui->gb_memory->setChecked(m_option_model_ptr->getCapMemory());
  ui->sb_memory->setValue(m_option_model_ptr->getMaxMemory());
  ui->sb_buffer->setValue(m_option_model_ptr->getBufferSize());
  checkDirectories();
}

void FormConfiguration::loadCosmoValues() {
  ui->txt_hubble_param->setText(QString::number(m_option_model_ptr->getHubble(), 'g', 15));
  ui->txt_omega_matter->setText(QString::number(m_option_model_ptr->getOmegaM(), 'g', 15));
  ui->txt_omega_lambda->setText(QString::number(m_option_model_ptr->getOmegaLambda(), 'g', 15));
}

void FormConfiguration::loadOptionPage(std::shared_ptr<OptionModel> option_model_ptr) {
  m_option_model_ptr = option_model_ptr;

  ui->txt_hubble_param->setValidator(new QDoubleValidator(0, 1000, 20));
  ui->txt_omega_matter->setValidator(new QDoubleValidator(-10, 10, 20));
  ui->txt_omega_lambda->setValidator(new QDoubleValidator(-10, 10, 20));

  ui->widget_aux_Data->setRepositories(m_option_model_ptr->getFilterRepo(), m_option_model_ptr->getSedRepo(),
                                       m_option_model_ptr->getReddeningRepo(), m_option_model_ptr->getLuminosityRepo());
  ui->widget_aux_Data->loadManagementPage(0);

  ui->cb_logLevel->setCurrentIndex(ui->cb_logLevel->findText(m_option_model_ptr->getLogLevel()));

  loadGeneralValues();
  loadCosmoValues();

  connect(ui->txt_catDir, SIGNAL(textEdited(const QString&)), m_option_model_ptr.get(),
          SLOT(setCatalog(const QString&)));
  connect(ui->txt_auxDir, SIGNAL(textEdited(const QString&)), m_option_model_ptr.get(),
          SLOT(setAuxiliary(const QString&)));
  connect(ui->txt_interDir, SIGNAL(textEdited(const QString&)), m_option_model_ptr.get(),
          SLOT(setIntermediary(const QString&)));
  connect(ui->txt_resDir, SIGNAL(textEdited(const QString&)), m_option_model_ptr.get(),
          SLOT(setResult(const QString&)));
  connect(ui->gb_thread, SIGNAL(clicked(bool)), m_option_model_ptr.get(), SLOT(setDefaultThread(bool)));
  connect(ui->sb_thread, SIGNAL(valueChanged(int)), m_option_model_ptr.get(), SLOT(setThread(int)));

  connect(ui->gb_memory, SIGNAL(clicked(bool)), m_option_model_ptr.get(), SLOT(setCapMemory(bool)));
  connect(ui->sb_memory, SIGNAL(valueChanged(double)), m_option_model_ptr.get(), SLOT(setMaxMemory(bool)));

  connect(ui->txt_hubble_param, SIGNAL(textEdited(const QString&)), m_option_model_ptr.get(),
          SLOT(setHubble(const QString&)));
  connect(ui->txt_omega_matter, SIGNAL(textEdited(const QString&)), m_option_model_ptr.get(),
          SLOT(setOmegaM(const QString&)));
  connect(ui->txt_omega_lambda, SIGNAL(textEdited(const QString&)), m_option_model_ptr.get(),
          SLOT(setOmegaLambda(const QString&)));
}

void FormConfiguration::setGeneralControlEdition(bool edit) {
  ui->btn_cancelGeneral->setEnabled(edit);
  ui->btn_saveGeneral->setEnabled(edit);
}

void FormConfiguration::setCosmoControlEdition(bool edit) {
  ui->btn_cancel_cosmo->setEnabled(edit);
  ui->btn_save_cosmo->setEnabled(edit);
}

void FormConfiguration::on_btn_cancelGeneral_clicked() {
  m_option_model_ptr->cancel();
  loadGeneralValues();
  setGeneralControlEdition(false);
  endEdition();
}

void FormConfiguration::on_btn_saveGeneral_clicked() {
  m_option_model_ptr->save();
  loadGeneralValues();
  setGeneralControlEdition(false);
  // reload trees based on the new providers
  ui->widget_aux_Data->loadManagementPage(-1);
  do_need_reset = true;
  endEdition();
}

void FormConfiguration::on_btn_browseCat_clicked() {
  QFileDialog dialog(this);
  dialog.selectFile(m_option_model_ptr->getCatPath());
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  if (dialog.exec()) {
    startEdition(0);
    setGeneralControlEdition(true);

    QStringList fileNames = dialog.selectedFiles();
    ui->txt_catDir->setText(fileNames[0]);
    m_option_model_ptr->setCatalog(fileNames[0]);
    checkDirectories();
  }
}

void FormConfiguration::on_btn_browseAux_clicked() {
  QFileDialog dialog(this);
  dialog.selectFile(m_option_model_ptr->getAuxPath());
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  if (dialog.exec()) {
    startEdition(0);
    setGeneralControlEdition(true);

    QStringList fileNames = dialog.selectedFiles();
    ui->txt_auxDir->setText(fileNames[0]);
    m_option_model_ptr->setAuxiliary(fileNames[0]);
    checkDirectories();
  }
}

void FormConfiguration::on_btn_browseInter_clicked() {
  QFileDialog dialog(this);
  dialog.selectFile(m_option_model_ptr->getInterPath());
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  if (dialog.exec()) {
    startEdition(0);
    setGeneralControlEdition(true);

    QStringList fileNames = dialog.selectedFiles();
    ui->txt_interDir->setText(fileNames[0]);
    m_option_model_ptr->setIntermediary(fileNames[0]);
    checkDirectories();
  }
}

void FormConfiguration::on_btn_browseRes_clicked() {
  QFileDialog dialog(this);
  dialog.selectFile(m_option_model_ptr->getResPath());
  dialog.setFileMode(QFileDialog::DirectoryOnly);
  if (dialog.exec()) {
    startEdition(0);
    setGeneralControlEdition(true);

    QStringList fileNames = dialog.selectedFiles();
    ui->txt_resDir->setText(fileNames[0]);
    m_option_model_ptr->setResult(fileNames[0]);
    checkDirectories();
  }
}

void FormConfiguration::on_gb_thread_clicked() {
  startEdition(0);
  setGeneralControlEdition(true);
}

void FormConfiguration::on_sb_thread_valueChanged(int i) {
  if (i != m_option_model_ptr->getThreadNb()) {
    startEdition(0);
    setGeneralControlEdition(true);
  }
}

void FormConfiguration::on_gb_memory_clicked() {
  startEdition(0);
  setGeneralControlEdition(true);
}

void FormConfiguration::on_sb_memory_valueChanged(double value) {
  if (value != m_option_model_ptr->getMaxMemory()) {
    startEdition(0);
    setGeneralControlEdition(true);
  }
}

void FormConfiguration::on_cb_logLevel_currentIndexChanged(int index) {
  if (ui->cb_logLevel->currentText() != m_option_model_ptr->getLogLevel()) {
    startEdition(0);
    m_option_model_ptr->setLogLevel(ui->cb_logLevel->currentText());
    setGeneralControlEdition(true);
  }
}

void FormConfiguration::on_sb_buffer_valueChanged(int i) {
  if (m_option_model_ptr->getBufferSize() != i) {
    startEdition(0);
    m_option_model_ptr->setBufferSize(i);
    setGeneralControlEdition(true);
  }
}

void FormConfiguration::on_btn_default_clicked() {
  startEdition(0);
  setGeneralControlEdition(true);

  m_option_model_ptr->setCatalog("");
  ui->txt_catDir->setText(m_option_model_ptr->getCatPath());
  m_option_model_ptr->setAuxiliary("");
  ui->txt_auxDir->setText(m_option_model_ptr->getAuxPath());
  m_option_model_ptr->setIntermediary("");
  ui->txt_interDir->setText(m_option_model_ptr->getInterPath());
  m_option_model_ptr->setResult("");
  ui->txt_resDir->setText(m_option_model_ptr->getResPath());

  ui->gb_thread->setChecked(false);
  ui->sb_thread->setValue(PhzUtils::getThreadNumber());

  ui->gb_memory->setChecked(false);
  ui->sb_memory->setValue(16.0);

  m_option_model_ptr->setBufferSize(5000);
  ui->sb_buffer->setValue(5000);
  checkDirectories();
}

void FormConfiguration::on_btn_cancel_cosmo_clicked() {
  m_option_model_ptr->cancel();
  loadCosmoValues();
  setCosmoControlEdition(false);
  endEdition();
}

void FormConfiguration::on_btn_save_cosmo_clicked() {
  m_option_model_ptr->save();
  loadCosmoValues();
  setCosmoControlEdition(false);
  endEdition();
}

void FormConfiguration::on_btn_default_cosmo_clicked() {
  startEdition(2);
  setCosmoControlEdition(true);
  m_option_model_ptr->setHubble("-1");
  m_option_model_ptr->setOmegaM("-1");
  m_option_model_ptr->setOmegaLambda("-1");
  loadCosmoValues();
}

void FormConfiguration::on_txt_hubble_param_textEdited(const QString&) {
  startEdition(2);
  setCosmoControlEdition(true);
}

void FormConfiguration::on_txt_omega_matter_textEdited(const QString&) {
  startEdition(2);
  setCosmoControlEdition(true);
}

void FormConfiguration::on_txt_omega_lambda_textEdited(const QString&) {
  startEdition(2);
  setCosmoControlEdition(true);
}

void FormConfiguration::startEdition(int i) {
  for (int j = 0; j < 3; ++j) {
    if (i != j) {
      ui->tabWidget->setTabEnabled(j, false);
    }
  }
  ui->frm_nav->setEnabled(false);
}

void FormConfiguration::endEdition() {
  ui->tabWidget->setTabEnabled(0, true);
  ui->tabWidget->setTabEnabled(1, true);
  ui->tabWidget->setTabEnabled(2, true);
  ui->tabWidget->setEnabled(true);

  ui->frm_nav->setEnabled(true);
}

void FormConfiguration::checkDirectories() {

  if (m_option_model_ptr->isCatPathDefault()) {
    ui->txt_catDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
    ui->txt_catDir->setStyleSheet("QLineEdit { color: black }");
  }

  if (m_option_model_ptr->isAuxPathDefault()) {
    ui->txt_auxDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
    ui->txt_auxDir->setStyleSheet("QLineEdit { color: black }");
  }

  if (m_option_model_ptr->isInterPathDefault()) {
    ui->txt_interDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
    ui->txt_interDir->setStyleSheet("QLineEdit { color: black }");
  }

  if (m_option_model_ptr->isResPathDefault()) {
    ui->txt_resDir->setStyleSheet("QLineEdit { color: grey }");
  } else {
    ui->txt_resDir->setStyleSheet("QLineEdit { color: black }");
  }
}

}  // namespace PhzQtUI
}  // namespace Euclid
