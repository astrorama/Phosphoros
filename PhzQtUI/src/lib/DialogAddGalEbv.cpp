
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include "PhzQtUI/DialogAddGalEbv.h"
#include "ui_DialogAddGalEbv.h"
#include "FileUtils.h"
#include "ElementsKernel/Logging.h"

using namespace std;

namespace Euclid {
namespace PhzQtUI {


static Elements::Logging logger = Elements::Logging::getLogger("DialogAddGalEbv");


DialogAddGalEbv::DialogAddGalEbv(QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogAddGalEbv){
  ui->setupUi(this);

  QRegExp rx("[\\w\\s]+");
  ui->txt_name->setValidator(new QRegExpValidator(rx));
  ui->label_process->setText(QString::fromStdString(""));

  connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
  connect (m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
  connect (m_process, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
  connect(m_process, SIGNAL(error(QProcess::ProcessError)),
      this, SLOT(getError(QProcess::ProcessError)));
}

DialogAddGalEbv::~DialogAddGalEbv() {
  disconnect(m_process, 0, 0, 0);
  delete m_process;
  m_process = NULL;

}

void DialogAddGalEbv::setInputs(std::string input_name, std::string ra_col, std::string dec_col) {
  m_input_name = input_name;
  m_ra_col = ra_col;
  m_dec_col = dec_col;
  m_name = m_input_name;
  ui->txt_name->setText(QString::fromStdString(m_name));
  ui->txt_name->setReadOnly(false);
  ui->btn_cancel->setEnabled(true);
  ui->btn_create->setEnabled(true);
}

void DialogAddGalEbv::on_btn_cancel_clicked() {
  m_process->kill();
  reject();
}

void DialogAddGalEbv::on_btn_create_clicked() {
  m_name = ui->txt_name->text().trimmed().toStdString();

  if (m_name.length()==0){
    QMessageBox::warning( this, "Empty Name...","Please enter a name.", QMessageBox::Ok );
    return;
  }

  QFileInfo info(QString::fromStdString(m_name));
  if (! info.exists() ||  QMessageBox::question(this,
           QString::fromStdString("Override Existing Catalog?"),
           QString::fromStdString("There is already a file with the name you provide: do you want to override it?"),
           QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Ok) {

      ui->label_process->setText(QString::fromStdString("Creating the E(B-V) column for the catalog..."));
      ui->txt_name->setReadOnly(true);
      ui->btn_cancel->setEnabled(true);
      ui->btn_create->setEnabled(false);

      // Call the python code
        std::string program = "AddGalDustToCatalog --input-catalog \""+m_input_name+"\" --output-catalog \""+m_name+"\" --ra "+m_ra_col+" --dec " + m_dec_col;
        std::string command = "";



        logger.info("Calling :"+program+" "+command);
        QStringList params;
        params << QString::fromStdString(command);



        m_process->start(QString::fromStdString(program));

    } else {
      return;
    }

}


void DialogAddGalEbv::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
  logger.info()<<"AddGalDustToCatalog: Exit code and status "<< exitCode<<"  "<<exitStatus;
  accept();
}

void DialogAddGalEbv::readError() {
  QByteArray byteArray = m_process->readAllStandardError();
  logger.info()<<"AddGalDustToCatalog :" << QString(byteArray).toStdString();
}
void DialogAddGalEbv::readOutput() {
  QByteArray byteArray = m_process->readAllStandardOutput();
  logger.info()<<"AddGalDustToCatalog :" << QString(byteArray).toStdString();
}


void DialogAddGalEbv::getError(QProcess::ProcessError error) {
  logger.info()<<"Error in the processing of AddGalDustToCatalog "<< error;
}


std::string DialogAddGalEbv::getOutputName() const {
  return m_name;
}


}
}
