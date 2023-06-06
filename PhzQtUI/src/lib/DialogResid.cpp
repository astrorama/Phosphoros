#include "ElementsKernel/Logging.h"
#include "FileUtils.h"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <QScrollBar>
#include <QStringList>
#include <QTextStream>
#include <QThread>
#include <list>
#include <sstream>

#include "PhzUITools/CatalogColumnReader.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include "PhzQtUI/DialogResid.h"
#include "ui_DialogResid.h"
#include <QApplication>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("DialogResid");

DialogResid::DialogResid(QWidget* parent) : QDialog(parent), ui(new Ui::DialogResid) {
  ui->setupUi(this);
}

DialogResid::~DialogResid() {}

void DialogResid::setFolder(std::string output_folder) {
  m_folder = output_folder;
  ui->btn_close->show();
  ui->btn_compute->show();
  ui->btn_compute->setEnabled(true);
  ui->btn_cancel->hide();
  ui->lbl_warning->hide();
  checkComputePossible();
}

void DialogResid::checkComputePossible() {
  auto basepath = boost::filesystem::path(m_folder);
  if (!boost::filesystem::exists(basepath / "run_config.config")) {
    ui->btn_compute->setEnabled(false);
    ui->lbl_warning->show();
    ui->lbl_warning->setText(
        "The configuration file is not present in the folder: please run again the template fitting.");
    return;
  }
  if (!boost::filesystem::exists(basepath / "phz_cat.fits") && !boost::filesystem::exists(basepath / "phz_cat.txt")) {
    ui->btn_compute->setEnabled(false);
    ui->lbl_warning->show();
    ui->lbl_warning->setText("The result file is not present in the folder: please run again the template fitting.");
    return;
  }

  std::set<std::string> columns{};
  if (boost::filesystem::exists(basepath / "phz_cat.fits")) {
    auto column_reader = PhzUITools::CatalogColumnReader((basepath / "phz_cat.fits").string());
    columns            = column_reader.getColumnNames();
  } else {
    auto column_reader = PhzUITools::CatalogColumnReader((basepath / "phz_cat.txt").string());
    columns            = column_reader.getColumnNames();
  }
  if (std::find(columns.begin(), columns.end(), "SED-Index") == columns.end()) {
    ui->btn_compute->setEnabled(false);
    ui->lbl_warning->show();
    ui->lbl_warning->setText(
        "The result file do not contains the best fitted model: please run again the template fitting.");
  }
}

void DialogResid::on_btn_cancel_clicked() {
  if (m_processing) {
    m_P->terminate();
    m_processing = false;
    ui->out_cons->setPlainText("Processing stop by the user");
    logger.info() << "Processing stop by the user";
  }
  ui->btn_cancel->hide();
  ui->btn_close->show();
  ui->btn_compute->show();
}

void DialogResid::on_btn_compute_clicked() {
  if (m_processing) {
    m_P->terminate();
    m_processing = false;
  }
  ui->out_cons->setPlainText("");

  ui->btn_cancel->show();
  ui->btn_close->hide();
  ui->btn_compute->hide();
  ui->out_cons->show();
  ui->out_cons->setReadOnly(true);
  m_processing = true;
  qApp->processEvents();

  m_P = new QProcess(this);
  m_P->setProcessEnvironment(QProcessEnvironment::systemEnvironment());

  connect(m_P, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processingFinished(int, QProcess::ExitStatus)));

  QString cmd = QString("PhosphorosPlotFluxDiff");

  QStringList arguments;
  arguments << QString::fromStdString("--result-dir") << QString::fromStdString(m_folder)
     << QString::fromStdString("--intermediate-product-dir")
     << QString::fromStdString(FileUtils::getIntermediaryProductRootPath(false, ""))
     << QString::fromStdString("--z-limit") << QString::number(ui->sp_z_lim_frac->value() / 100.0)
     << QString::fromStdString("--vertical-sigma-limit") << QString::number(ui->dsp_sigma->value())
     << QString::fromStdString("--sliding-mean-sampling") << QString::number(ui->sb_sm_bins->value());


  logger.info() << "Processing cmd:" << "PhosphorosPlotFluxDiff " << arguments.join(" ").toStdString();

  m_P->setProcessChannelMode(QProcess::MergedChannels);
  m_P->start(cmd, arguments);

  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(updateOutCons()));
  m_timer->start(100);
}

void DialogResid::on_btn_close_clicked() {
  if (m_processing) {
    m_P->terminate();
    m_processing = false;
  }
  accept();
}

void DialogResid::processingFinished(int, QProcess::ExitStatus) {
  m_timer->stop();
  m_processing = false;
  logger.info() << "Processing Finished";
  updateOutCons();
  ui->btn_cancel->hide();
  ui->btn_close->show();
  ui->btn_compute->show();
  logger.info() << ui->out_cons->toPlainText().toStdString();
  ui->out_cons->setPlainText(ui->out_cons->toPlainText() + "Processing Finished");
  ui->out_cons->verticalScrollBar()->setValue(ui->out_cons->verticalScrollBar()->maximum());
}

void DialogResid::updateOutCons() {
  QString result_all = m_P->readAllStandardOutput();
  ui->out_cons->setPlainText(ui->out_cons->toPlainText() + result_all);
  ui->out_cons->verticalScrollBar()->setValue(ui->out_cons->verticalScrollBar()->maximum());
}

}  // namespace PhzQtUI
}  // namespace Euclid
