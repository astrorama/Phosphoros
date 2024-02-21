
#include "ElementsKernel/Logging.h"
#include "ElementsKernel/Temporary.h"
#include "FileUtils.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>
#include <QProcess>
#include <QScrollBar>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringList>
#include <QThread>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "PhzQtUI/DialogExtractZ.h"
#include "ui_DialogExtractZ.h"
#include <QApplication>

using namespace std;

namespace Euclid {
namespace PhzQtUI {

static Elements::Logging logger = Elements::Logging::getLogger("DialogExtractZ");

DialogExtractZ::DialogExtractZ(QWidget* parent) : QDialog(parent), ui(new Ui::DialogExtractZ) {
  ui->setupUi(this);
}

DialogExtractZ::~DialogExtractZ(){};

std::list<float> DialogExtractZ::getRedshiftList(){
	return m_zs;
}

void DialogExtractZ::setData(std::string input_cat, std::string col, int start, int max_num, double tol, bool do_scale) {
	m_input_cat = input_cat;
	ui->le_cat->setText(QString::fromStdString(input_cat));
	m_col = col;
	ui->le_col->setText(QString::fromStdString(col));
	m_start = start;
	ui->sb_start->setValue(start);
	m_max_num = max_num;
	ui->sb_number->setValue(max_num);
	m_tol=tol;
	ui->sb_tol->setValue(tol);
	m_do_scale=do_scale;
	ui->cb_scale->setChecked(do_scale);
	logger.info()<<"Data feed to the Dialog";
}

int DialogExtractZ::exec() {
	std::string output_file = (m_temp_dir.path()/"list.dat").string();

		QString     prog = QString::fromStdString("PhosphorosExtractZList");
		QStringList arguments;
		arguments << "--input_catalog" << QString::fromStdString(m_input_cat)
				  << "--ref_z_col" << QString::fromStdString(m_col)
				  << "--skip_n_first" << QString::number(m_start)
				  << "--max_record" << QString::number(m_max_num)
				  << "--min_z_step" << QString::number(m_tol)
				  << "--output_file" << QString::fromStdString(output_file);
		if (m_do_scale) {
			arguments << "--scale_min_z_step" << "1";
		}

		logger.info() << prog.toStdString() << " " << arguments.join(" ").toStdString();

		m_P = new QProcess(this);
		m_P->setProcessChannelMode(QProcess::MergedChannels);
		connect(m_P, SIGNAL(finished(int, QProcess::ExitStatus)), this,
				SLOT(processingFinished(int, QProcess::ExitStatus)));

		m_P->start(prog, arguments);
		m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(updateOutCons()));
		m_timer->start(100);

        return QDialog::exec();
}



void DialogExtractZ::updateOutCons() {
  QString result_all = m_P->readAllStandardOutput();
  ui->out_cons->setPlainText(ui->out_cons->toPlainText() + result_all);
  ui->out_cons->verticalScrollBar()->setValue(ui->out_cons->verticalScrollBar()->maximum());
}

void DialogExtractZ::processingFinished(int, QProcess::ExitStatus) {
  m_timer->stop();
  disconnect(m_timer, SIGNAL(timeout()), 0, 0);
  disconnect(m_P, SIGNAL(finished(int, QProcess::ExitStatus)), 0, 0);
  // read the result

  std::ifstream file((m_temp_dir.path()/"list.dat").string());
  std::string line;
  while (std::getline(file, line)) {
    	 m_zs.push_back(std::stof(line));
  }

  if (m_zs.front()!=0){
	  m_zs.push_front(0);
  }
  logger.info()<< "Found "<<m_zs.size()<< " distinct redshifts in catalog " << m_input_cat << " with the provided setting";
  if (m_zs.size()>600) {
	  if (QMessageBox::warning(this, "Large Number of redshifts...",
	 								   "The provided file contains " + QString::number(m_zs.size()) + " redshifts. This may generate a quite large grid."
	 								   "Do you want to continue? "
	 								   "(If not you can put a limit on the number of sources to be processed or raise the redshift tolerance)",
	 								   QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		  this->reject();
	  } else {
		  this->accept();
	  }
  } else {
      this->accept();
  }
}

void DialogExtractZ::on_btn_cancel_clicked() {
	m_P->kill();
	m_timer->stop();
	disconnect(m_timer, SIGNAL(timeout()), 0, 0);
	disconnect(m_P, SIGNAL(finished(int, QProcess::ExitStatus)), 0, 0);
	reject();
}


}  // namespace PhzQtUI
}  // namespace Euclid
